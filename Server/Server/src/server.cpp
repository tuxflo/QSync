#include "include/server.h"

Server::Server(QString directory, quint16 port, QObject *parent) :
    QObject(parent),
    _directory_path(directory),
    _num_files(-1)
{
    qDebug() << "creating checksums...";
    if(!_create_checksums())
    {
        //something went wrong while creating the sha1 sums
        qDebug() << "Error while creating checksums!";
        exit(-1);
    }
    qDebug() << "checksums ok!";
    qDebug() << "Starting server on port: " << port;
    _server = new QTcpServer(this);
    if(!_server->listen(QHostAddress::Any, port))
    {
        qDebug() << "Could not start server! (Maybe an other application is using the port?)";
        exit(-2);
    }
    qDebug() << "Server is running on port: " << port;

    connect(_server, SIGNAL(newConnection()), this, SLOT(send_dir_name()));
}

Server::~Server()
{
    delete _server;
}

void Server::send_dir_name()
{
    //The function sends the name of working directory to the client
    _client = _server->nextPendingConnection();
    qDebug() << "Client connected, sending name of working directory: " << QDir(_directory_path).dirName();
    _client->write(QDir(_directory_path).dirName().toAscii());
    _client->flush();
    connect(_client, SIGNAL(readyRead()), this, SLOT(send_files()));
}

void Server::send_files()
{
    QByteArray buf;
    QDataStream stream(&buf, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_4_0);
    if(_num_files == _filenames.size()-1)
    {
        qDebug() << "Finished syncronisation!";
        stream << (QString)"done";
        _client->write(buf);
        _client->flush();
        connect(_client, SIGNAL(disconnected()), this, SLOT(quitting_server()));
        _client->disconnectFromHost();
        return;
    }
    QString string = _client->readAll();
    int choose = string.toInt();
    switch(choose)
    {
    case OK:
        qDebug() << "received OK so continue with the next file";
        _num_files++;
        stream << _filenames.at(_num_files);
        stream << _sha_sums.at(_num_files);
        _client->write(buf);
        _client->flush();
        break;
    case DOWNLOAD:
        qDebug() << "received DOWNLOAD so send the file";
        if(!_send_current_file())
            qDebug() << "Error while sending file: " << _filenames.at(_num_files);
        _client->flush();
        break;
    default:
        qDebug() << "Error while syncing files!";
        _client->disconnectFromHost();
        exit(-3);
        break;
    }
}

void Server::quitting_server()
{
    _client->close();
    _server->close();
    delete this;
}

bool Server::_create_checksums()
{
    QDir working_dir;
    QFile file;
    working_dir.setFilter(QDir::Files);
    working_dir.setCurrent(_directory_path);
    QDirIterator it(working_dir, QDirIterator::Subdirectories);
    while(it.hasNext())
    {
        it.next();
        file.setFileName(it.filePath());
        if(!file.open(QIODevice::ReadOnly))
        {
            qDebug() << "Error while opening file: " << it.filePath();
            return false;
        }
        _filenames.push_back(it.filePath());
        _sha_sums.push_back(QCryptographicHash::hash(file.readAll(), QCryptographicHash::Sha1));
        file.close();
    }
    return true;
}

bool Server::_send_current_file()
{
    QFile file(_filenames.at(_num_files));
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Error while opening file: " << _filenames.at(_num_files);
        return false;
    }

    QByteArray buf;
    QDataStream stream(&buf, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_4_0);

    //put a placeholder at the front of the Bytearray
    stream << (quint32)0;

    //push the filename into the Bytearray
    stream << _filenames.at(_num_files);

    //push the data of the file into the Bytearray
    stream << file.readAll();

    //set the filesize
    stream.device()->seek(0);
    stream << (quint32)(buf.size() - sizeof(quint32));
    _client->write(buf);
    return true;
}
