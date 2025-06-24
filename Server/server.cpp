// #include "server.h"
// #include <QDebug>

// Server::Server(QObject *parent)
//     : QTcpServer(parent)
// {
//     users = new Users(this);
//     account = new Account(users, this);

// }

// void Server::startServer()
// {
//     if (listen(QHostAddress::Any, 8080)) {
//         qDebug() << "Server started on port 8080";
//     } else {
//         qDebug() << "Failed to start server";
//     }
// }

// void Server::incomingConnection(qintptr socketDescriptor)
// {
//     qDebug() << "New connection";

//     QTcpSocket *clientSocket = new QTcpSocket(this);
//     clientSocket->setSocketDescriptor(socketDescriptor);

//     chanells *channel = new chanells(clientSocket, this);
//     clients.append(channel);


//     connect(channel, &chanells::messageReceived, this, &Server::handleMessage);
//     connect(channel, &chanells::disconnected, this, &Server::handleDisconnection);

//     channel->start();
// }

// void Server::handleMessage(chanells* source, QString msg)
// {
//     qDebug() << "From client:" << source << "->" << msg;
//     QJsonParseError error;
//     QJsonDocument doc = QJsonDocument::fromJson(msg.toUtf8(), &error);
//     if (error.error != QJsonParseError::NoError) {
//         qDebug() << "Invalid JSON format";
//         return;
//     }

//     QJsonObject obj = doc.object();
//     QString type = obj["type"].toString();
//     QJsonObject response;

//     try {
//         if (type == "login") {
//             response = account->login(obj);
//         }
//         else if (type == "signup") {
//             response = account->signup(obj);
//         }
//         else if (type == "forgetPassword") {
//             response = account->forgetpass(obj);
//         }
//         else if (type == "start_game") {
//             // handel game
//             response = QJsonObject{
//                 {"type", "start_game"},
//                 {"status", "not_implemented"}
//             };
//         }
//         else {
//             response = QJsonObject{
//                 {"type", "error"},
//                 {"message", "Unknown request type"}
//             };
//         }
//     }
//     catch (const UserException& ex) {
//         response = QJsonObject{
//             {"type", type},
//             {"status", "error"},
//             {"message", ex.what()}
//         };
//     }

//     QJsonDocument docRes(response);
//     source->sendMessage(QString::fromUtf8(docRes.toJson(QJsonDocument::Compact)));
// }


// void Server::handleDisconnection()
// {
//     chanells *channel = qobject_cast<chanells*>(sender());
//     if (channel)
//     {
//         qDebug() << "Client disconnected. Removing from list.";
//         clients.removeOne(channel);
//         channel->deleteLater();
//     }
// }



#include "server.h"
#include <QDebug>
#include<QString>
#include <QThread>

Server::Server(QObject *parent)
    : QTcpServer(parent)
{
    users = new Users(this);
    account = new Account(users, this);
}

void Server::startServer()
{
    if (listen(QHostAddress::Any, 8080)) {
        qDebug() << "Server started on port" << serverPort();
    } else {
        qDebug() << "Failed to start server";
    }
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "New connection";

    QTcpSocket *clientSocket = new QTcpSocket();
    clientSocket->setSocketDescriptor(socketDescriptor);

    chanells *channel = new chanells(clientSocket);
    QThread *thread = new QThread();

    channel->moveToThread(thread);
    clientSocket->moveToThread(thread);

    connect(thread, &QThread::started, channel, &chanells::start);
    connect(channel, &chanells::messageReceived, this, &Server::handleMessage);
    connect(channel, &chanells::disconnected, this, &Server::handleDisconnection);

    connect(channel, &chanells::disconnected, thread, &QThread::quit);
    connect(thread, &QThread::finished, channel, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    clients.append(channel);
    thread->start();
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
    QJsonObject response;

    try {
        if (type == "login") {
            response = account->login(obj);
        }
        else if (type == "signup") {
            response = account->signup(obj);
        }
        else if (type == "forgetPassword") {
            response = account->forgetpass(obj);
        }
        else if (type == "start_game") {
            response = QJsonObject{
                {"type", "start_game"},
                {"status", "not_implemented"}
            };
        }
        else {
            response = QJsonObject{
                {"type", "error"},
                {"message", "Unknown request type"}
            };
        }
    }
    catch (const UserException& ex) {
        response = QJsonObject{
            {"type", type},
            {"status", "error"},
            {"message", ex.what()}
        };
    }

    QJsonDocument docRes(response);
    source->sendMessage(QString::fromUtf8(docRes.toJson(QJsonDocument::Compact)));
}

void Server::handleDisconnection()
{
    chanells *channel = qobject_cast<chanells*>(sender());
    if (channel)
    {
        qDebug() << "Client disconnected. Removing from list.";
        clients.removeOne(channel);
    }
}
