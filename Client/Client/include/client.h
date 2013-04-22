#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QDir>
#include <QTcpSocket>
#include <QHostAddress>
#include <QCryptographicHash>

enum State {OK,DOWNLOAD};
class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = 0);
    void connect_to_server(QString address, quint16 port);
signals:
    
public slots:
    void receive_dir_name();
    void receive_file();
    void check_files();
private:
    bool _compare_checksum(QString filename, QByteArray sha1sum);
    quint32 _blocksize;
    QTcpSocket *_server;
    
};

#endif // CLIENT_H
