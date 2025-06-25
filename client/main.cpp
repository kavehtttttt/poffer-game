#include "mainmenu.h"
#include "server.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTcpSocket *sharedSocket = new QTcpSocket();

    // تلاش برای اتصال به سرور
    sharedSocket->connectToHost("127.0.0.1", 8080);

    // نمایش MainMenu با سوکت
    MainMenu *mainWindow = new MainMenu(nullptr, sharedSocket);

    ///Server server;
    mainWindow->show();
    return a.exec();
}
