#include "include/client.h"

Client::Client(QObject *parent) :
    QObject(parent),
    _blocksize(0)
{
}

void Client::connect_to_server(QString address, quint16 port)
{
    _server = new QTcpSocket(this);
    QHostAddress addr(address);
    qDebug() << "trying to connect to " << addr << "on port: " << port;
    _server->connectToHost(addr, port);
    connect(_server, SIGNAL(readyRead()), this, SLOT(receive_dir_name()));
    _server->flush();

}

void Client::receive_dir_name()
{
    qDebug() << "Connection established!";
    QDir working_dir;
    working_dir.setCurrent(".");
    qDebug() << "receiving directory name";
    QString dir_name = _server->readAll();
    qDebug() << "received directory name: " << dir_name;
    if(!QDir(dir_name).exists())
        working_dir.mkpath(dir_name);
    if(!working_dir.setCurrent(dir_name))
        qDebug() << "Failed to set the working directory to: " << dir_name;
    _server->write("0");
    _server->flush();
    connect(_server, SIGNAL(readyRead()), this, SLOT(check_files()));
    disconnect(_server, SIGNAL(readyRead()), this, SLOT(receive_dir_name()));
}

void Client::receive_file()
{
    QDataStream stream(_server);
    stream.setVersion(QDataStream::Qt_4_0);
    qDebug() << "now downloading file...";
    if(_blocksize == 0)
    {
        if(_server->bytesAvailable() < (int)sizeof(quint32))
            return;
        stream >> _blocksize;
        qDebug() << "blocksize = " << _blocksize;
    }

    if(_server->bytesAvailable() < _blocksize)
        return;
    QByteArray buf;
    QString filename;
    stream >> filename;

    qDebug() << "received filename: " << filename;
    QFileInfo file_info(filename);
    QDir dir(file_info.filePath());
    if(!dir.exists())
        dir.mkpath(file_info.absoluteDir().path());

    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "failed to open " << filename;
        _server->disconnectFromHost();
        exit(-1);
    }
    stream >> buf;
    file.write(buf);
    file.close();
    qDebug() << "receiving file was successful!";
    _blocksize = 0;
    _server->write("0");
    disconnect(_server, SIGNAL(readyRead()), this, SLOT(receive_file()));
    connect(_server, SIGNAL(readyRead()), this, SLOT(check_files()));
    return;

}

void Client::check_files()
{
    //Check if file was sent
    if(_server->bytesAvailable() > 100)
    {
        disconnect(_server, SIGNAL(readyRead()), this, SLOT(check_files()));
        connect(_server, SIGNAL(readyRead()), this, SLOT(receive_file()));
        receive_file();
        _server->flush();
    }
    else
    {
        QByteArray buf = _server->readAll();
        QString filename;
        QByteArray sha1sum;
        QDataStream stream(&buf, QIODevice::ReadOnly);
        stream >> filename;
        stream >> sha1sum;
        if(filename == QString("done"))
        {
            qDebug() << "finished sycning files!";
            _server->disconnectFromHost();
            _server->close();
            exit(0);
        }
        qDebug() << "received filename: " <<  filename << "received sha1 sum: " << sha1sum.toHex();
        if(!_compare_checksum(filename, sha1sum))
        {
            //The file does not exist so send DOWNLOAD to receive it
            _server->write("1");
            _server->flush();
        }
        else
        {
            _server->write("0");
            _server->flush();
        }

    }
}

bool Client::_compare_checksum(QString filename, QByteArray sha1sum)
{
    QFile file(filename);
    if(!file.exists())
        return false;
    QByteArray sum = QCryptographicHash::hash(file.readAll(), QCryptographicHash::Sha1);
    if(sum == sha1sum)
        return true;
    return false;
}
