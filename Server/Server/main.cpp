#include <QCoreApplication>
#include "include/server.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Server server("/workspace/tmp/", 8888);
    return a.exec();
}
