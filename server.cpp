#include "server.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

Server::Server(QObject *parent)
    : QObject(parent)
{
    tcpServer = new QTcpServer(this);

    if (!tcpServer->listen(QHostAddress::Any, 1234)) {
        qDebug() << "Server could not start!";
    } else {
        qDebug() << "Server started on port 1234...";
        connect(tcpServer, &QTcpServer::newConnection, this, &Server::onNewConnection);
    }
}

void Server::onNewConnection()
{
    clientSocket = tcpServer->nextPendingConnection();
    connect(clientSocket, &QTcpSocket::readyRead, this, &Server::onReadyRead);
    qDebug() << "Client connected!";
}

void Server::onReadyRead()
{
    QByteArray requestData = clientSocket->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(requestData);

    if (!doc.isNull() && doc.isObject()) {
        QJsonObject obj = doc.object();
        QString requestType = obj.value("request_type").toString();

        if (requestType == "sign_up") {
            QString username = obj.value("username").toString();
            QString response = QString("User %1 signed up successfully!").arg(username);
            clientSocket->write(response.toUtf8());
            clientSocket->flush();
        }

        else if (requestType == "login") {
            // ✅ Always accept login
            qDebug() << "Login request received from:"
                     << obj.value("username").toString();
            clientSocket->write("login_success");
            clientSocket->flush();
        }

        else {
            clientSocket->write("Unknown request");
            clientSocket->flush();
        }
    } else {
        clientSocket->write("Invalid JSON data");
        clientSocket->flush();
    }
}
