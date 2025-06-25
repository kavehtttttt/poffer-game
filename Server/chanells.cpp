#include "chanells.h"
#include <QDebug>

chanells::chanells(QTcpSocket *_socket, QObject *parent)
    : QObject(parent), socket(_socket)
{
    Username="unknown";
    socket->setParent(this);
}

chanells::~chanells()
{

    if (socket) {
        socket->disconnectFromHost();
        }
    qDebug() << "Chanells object destroyed";
}

void chanells::start()
{

    qDebug() << "Chanell communication started in main thread.";


    connect(socket, &QTcpSocket::readyRead, this, &chanells::readyRead);
    connect(socket, &QTcpSocket::disconnected, this, &chanells::disconnectedSlot);
}

void chanells::readyRead()
{
    QByteArray data = socket->readAll();
    QString msg = QString::fromUtf8(data);
    qDebug() << "Received:" << msg;

    emit messageReceived(this, msg);
}

void chanells::disconnectedSlot()
{
    qDebug() << "Client disconnected";
    emit disconnected();
    }

void chanells::sendMessage(const QString& msg)
{
    if (socket && socket->isOpen()) {
        socket->write(msg.toUtf8());
        socket->flush();
    }
}
void chanells::setUsername(QString u){
    Username=u;
}
QString chanells::getUsernamme(){
    return Username;
}
