#include "mainmenu.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTcpSocket *sharedSocket = new QTcpSocket();

    sharedSocket->connectToHost("127.0.0.1", 8080);

    MainMenu *mainWindow = new MainMenu(nullptr, sharedSocket);

    mainWindow->show();
    return a.exec();
}
