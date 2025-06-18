#include "chanells.h"
#include <QDebug>

chanells::chanells(QTcpSocket *_socket, QObject *parent)
    : QObject(parent), socket(_socket)
{}

chanells::~chanells()
{
    stop();
}

void chanells::start()
{
    running = true;
    t = std::thread(&chanells::communication, this);
}

void chanells::stop()
{
    running = false;
    if (t.joinable())
        t.join();
}

void chanells::communication()
{
    while (running)
    {
        if (socket->waitForReadyRead(100))
        {
            QByteArray data = socket->readAll();
            QString msg = QString::fromUtf8(data);
            qDebug() << "Received:" << msg;

            emit messageReceived(msg);
        }

        if (socket->state() == QAbstractSocket::UnconnectedState)
        {
            qDebug() << "Client disconnected";
            emit disconnected();
            running = false;
        }
    }
}
