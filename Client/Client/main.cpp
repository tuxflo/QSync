#include <QCoreApplication>
#include "include/client.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Client client;
    client.connect_to_server("127.0.0.1", 8888);
    return a.exec();
}
