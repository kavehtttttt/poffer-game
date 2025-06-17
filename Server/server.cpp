#include "server.h"
#include <QDebug>

Server::Server(QObject *parent)
    : QTcpServer(parent)
{
}

void Server::startServer()
{
    if (listen(QHostAddress::Any, 1234)) {
        qDebug() << "Server started on port 1234";
    } else {
        qDebug() << "Failed to start server";
    }
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "New connection";

    QTcpSocket *clientSocket = new QTcpSocket(this);
    clientSocket->setSocketDescriptor(socketDescriptor);

    chanells *channel = new chanells(clientSocket, this);
    clients.append(channel);


    connect(channel, &chanells::messageReceived, this, &Server::handleMessage);
    connect(channel, &chanells::disconnected, this, &Server::handleDisconnection);

    channel->start();
}

void Server::handleMessage(QString msg)
{
    qDebug() << "Server received message:" << msg;
    // in this place i will handle message
}

void Server::handleDisconnection()
{
    chanells *channel = qobject_cast<chanells*>(sender());
    if (channel)
    {
        qDebug() << "Client disconnected. Removing from list.";
        clients.removeOne(channel);
        channel->deleteLater();
    }
}
