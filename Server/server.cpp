#include "server.h"
#include <QDebug>
#include <QString>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

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
    qDebug() << "From client:" << source->getUsername() << "->" << msg;
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(msg.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug() << "Invalid JSON format";
        QJsonObject response = QJsonObject{{"type", "error"}, {"message", "Invalid JSON format"}};
        QJsonDocument docRes(response);
        source->sendMessage(QString::fromUtf8(docRes.toJson(QJsonDocument::Compact)));
        return;
    }

    QJsonObject obj = doc.object();
    QString type = obj["type"].toString();
    QJsonObject response;

    try {
        if (type == "login") {
            response = account->login(obj);
            if(response["status"]=="success"){
                source->setUsername(obj["username"].toString());
            }
        }
        else if (type == "logout") {
            response = account->logout(obj);
            if (waitingClients.contains(source->getUsername())) {
                waitingClients.remove(source->getUsername());
            }
            source->setUsername("unknown");
        }
        else if (type == "signup") {
            response = account->signup(obj);
        }
        else if (type == "forgetPassword") {
            response = account->forgetpass(obj);
        }
        else if (type == "Edit_username") {
            QString oldUsername = source->getUsername();
            response = account->editUsername(obj);
            if(response["status"]=="success"){
                QString newUsername = obj["new_username"].toString();
                if (waitingClients.contains(oldUsername)) {
                    chanells* channel = waitingClients.take(oldUsername);
                    channel->setUsername(newUsername);
                    waitingClients.insert(newUsername, channel);
                } else {
                    source->setUsername(newUsername);
                }
            }
        }
        else if (type == "Edit_password") {
            response = account->editPassword(obj);
        }
        else if (type == "Edit_email") {
            response = account->editEmail(obj);
        }
        else if (type == "Edit_Phone") {
            response = account->editPhone(obj);
        }
        else if (type == "Edit_fName") {
            response = account->editFirstName(obj);
        }
        else if (type == "Edit_Lname") {
            response = account->editLastName(obj);
        }
        else if (type == "Get_History") {
            response = account->getGameHistory(obj);
        }
        else if (type == "Add_History") {
            response = account->addGameHistory(obj);
        }
        else if (type == "start_game") {
            QString username = source->getUsername();
            if (username.isEmpty() || username == "unknown") {
                response = QJsonObject{
                    {"type", "start_game"},
                    {"status", "error"},
                    {"message", "Please log in first to start a game."}
                };
            } else if (waitingClients.contains(username)) {
                response = QJsonObject{
                    {"type", "start_game"},
                    {"status", "info"},
                    {"message", "You are already in the game queue. Waiting for other players..."}
                };
            } else {
                waitingClients.insert(username, source);
                qDebug() << "Player" << username << "added to game queue. Current players:" << waitingClients.size();

                if (waitingClients.size() == 4) {
                    QJsonObject gameStartResponse = QJsonObject{
                        {"type", "Game_Start"},
                        {"status", "success"},
                        {"message", "Game starting! Get ready!"},
                        {"players", QJsonArray::fromStringList(waitingClients.keys())}
                    };
                    QJsonDocument gameStartDoc(gameStartResponse);
                    QString gameStartMsg = QString::fromUtf8(gameStartDoc.toJson(QJsonDocument::Compact));

                    qDebug() << "Enough players (4) to start game. Sending Game_Start to all.";
                    for (chanells* waitingChannel : waitingClients.values()) {
                        waitingChannel->sendMessage(gameStartMsg);
                    }
                    waitingClients.clear();
                    response = QJsonObject{};
                } else {
                    response = QJsonObject{
                        {"type", "start_game"},
                        {"status", "success"},
                        {"message", QString("You have joined the game queue. Waiting for %1 more players...").arg(4 - waitingClients.size())}
                    };
                }
            }
        }
        else if (type == "leave_waiting_room") {
            QString username = source->getUsername();
            if (!username.isEmpty() && username != "unknown" && waitingClients.contains(username)) {
                waitingClients.remove(username);
                response = QJsonObject{
                    {"type", "leave_waiting_room"},
                    {"status", "success"},
                    {"message", "You have left the game queue."}
                };
            } else {
                response = QJsonObject{
                    {"type", "leave_waiting_room"},
                    {"status", "error"},
                    {"message", "Could not leave game queue (not in it or not logged in)."}
                };
            }
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

    if (!response.isEmpty()) {
        QJsonDocument docRes(response);
        qDebug() << "Server sending to client" << source->getUsername() << "data:" << QString::fromUtf8(docRes.toJson(QJsonDocument::Compact));
        source->sendMessage(QString::fromUtf8(docRes.toJson(QJsonDocument::Compact)));
    }
}

void Server::handleDisconnection()
{
    chanells *channel = qobject_cast<chanells*>(sender());
    if (channel)
    {
        qDebug() << "Client" << channel->getUsername() << "disconnected. Removing from list.";
        if (waitingClients.contains(channel->getUsername())) {
            waitingClients.remove(channel->getUsername());
        }
        clients.removeOne(channel);
        channel->deleteLater();
    }
}
