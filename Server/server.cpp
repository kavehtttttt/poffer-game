#include "server.h"
#include <QDebug>

Server::Server(QObject *parent)
    : QTcpServer(parent)
{
}

void Server::startServer()
{
    if (listen(QHostAddress::Any, 8080)) {
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

void Server::handleMessage(chanells* source, QString msg)
{
    qDebug() << "From client:" << source << "->" << msg;
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(msg.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug() << "Invalid JSON format";
        return;
    }

    QJsonObject obj = doc.object();
    QString type = obj["type"].toString();

    if (type == "login") {
       //I will make class name account to handle sign in and sign up getting just obj Json
    }
    else if (type == "signup") {
        //I will make class name account to handle sign in and sign up getting just obj Json
    }
    else if (type == "start_game") {
       //It will be done in game class
    }
    else {
        qDebug() << "Unknown message type";
    }
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
