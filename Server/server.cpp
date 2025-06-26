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
    qDebug() << "From client:" << source << "->" << msg;
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
            // Remove from waiting list if logged out
            if (waitingClients.contains(source->getUsername())) {
                waitingClients.remove(source->getUsername());
                notifyWaitingClients();
            }
            source->setUsername("unknown");
        }
        else if (type == "signup") {
            response = account->signup(obj);
        }
        else if (type == "forgetPassword") {
            response = account->forgetpass(obj);
        }
        // User data edit operations
        else if (type == "Edit_username") {
            QString oldUsername = source->getUsername(); // Get current username from channel
            response = account->editUsername(obj);
            if(response["status"]=="success"){
                QString newUsername = obj["new_username"].toString();
                // Update username in waitingClients if present
                if (waitingClients.contains(oldUsername)) {
                    chanells* channel = waitingClients.take(oldUsername); // Remove old, get pointer
                    channel->setUsername(newUsername); // Update channel's username
                    waitingClients.insert(newUsername, channel); // Insert with new username
                    notifyWaitingClients();
                } else {
                    source->setUsername(newUsername); // Update channel's username for non-waiting client
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
        // Game History operations
        else if (type == "Get_History") {
            response = account->getGameHistory(obj);
        }
        else if (type == "Add_History") {
            response = account->addGameHistory(obj);
        }
        // Start Game / Waiting Room
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
                    {"message", "You are already in the waiting room."}
                };
            } else if (waitingClients.size() >= 4) { // NEW: Check if waiting room is full
                response = QJsonObject{
                    {"type", "start_game"},
                    {"status", "error"},
                    {"message", "Waiting room is full. Please try again later."}
                };
            }
            else {
                waitingClients.insert(username, source);
                notifyWaitingClients(); // Notify all clients in waiting room
                response = QJsonObject{
                    {"type", "start_game"},
                    {"status", "success"},
                    {"message", "Added to waiting room. Waiting for other players..."}
                };

                // Check if enough players to start game (4 players for 4-player game)
                if (waitingClients.size() == 4) { // Assuming 4 players as per requirement
                    // Here you would implement game starting logic
                    // For now, just notify waiting clients that game is starting (placeholder)
                    QJsonObject gameStartResponse = QJsonObject{
                        {"type", "Game_Start"},
                        {"status", "success"},
                        {"message", "Game starting! Get ready!"},
                        {"players", QJsonArray::fromStringList(waitingClients.keys())}
                    };
                    QJsonDocument gameStartDoc(gameStartResponse);
                    for (chanells* waitingChannel : waitingClients.values()) {
                        waitingChannel->sendMessage(QString::fromUtf8(gameStartDoc.toJson(QJsonDocument::Compact)));
                    }
                    waitingClients.clear(); // Clear waiting list once game starts
                    // Further game setup/lobby management would go here
                }
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

    QJsonDocument docRes(response);
    source->sendMessage(QString::fromUtf8(docRes.toJson(QJsonDocument::Compact)));
}

void Server::handleDisconnection()
{
    chanells *channel = qobject_cast<chanells*>(sender());
    if (channel)
    {
        qDebug() << "Client disconnected. Removing from list.";
        // Remove from waiting list if disconnected
        if (waitingClients.contains(channel->getUsername())) {
            waitingClients.remove(channel->getUsername());
            notifyWaitingClients(); // Notify remaining clients
        }
        clients.removeOne(channel);
        channel->deleteLater();
    }
}

// New method to notify all clients in the waiting room
void Server::notifyWaitingClients() {
    QJsonArray waitingPlayersArray;
    for (const QString& username : waitingClients.keys()) {
        waitingPlayersArray.append(username);
    }

    QJsonObject notification = QJsonObject{
        {"type", "Waiting_List_Update"},
        {"waiting_players", waitingPlayersArray},
        {"current_players_count", waitingClients.size()},
        {"required_players_count", 4} // As per doc, 4 players for 4-player game
    };
    QJsonDocument doc(notification);
    QString notificationMsg = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));

    for (chanells* clientChannel : waitingClients.values()) {
        clientChannel->sendMessage(notificationMsg);
    }
    qDebug() << "Notified waiting clients. Current waiting list:" << waitingClients.keys();
}
