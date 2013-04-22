#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QCryptographicHash>
#include <QVector>

enum State{OK, DOWNLOAD};
class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QString directory, quint16 port = 8888, QObject *parent = 0);
    ~Server();
    
signals:
    
public slots:
    void send_dir_name();
    void send_files();
    void quitting_server();
    
private:
    QTcpServer *_server;
    QTcpSocket *_client;
    bool _create_checksums();
    bool _send_current_file();
    QString _directory_path;
    int _num_files;
    QVector<QString> _filenames;
    QVector<QByteArray> _sha_sums;
};

#endif // SERVER_H
