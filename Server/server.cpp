#include "server.h"
#include <QDebug>
#include <QString>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>

#include "GameManager.h"
#include "GameSession.h"

Server::Server(QObject *parent)
    : QTcpServer(parent)
{
    users = new Users(this);
    account = new Account(users, this);
    gameManager = new GameManager(users, this);
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
        qDebug() << "Invalid JSON format received. Error:" << error.errorString();
        QJsonObject response = QJsonObject{{"type", "error"}, {"message", "Invalid JSON format"}};
        QJsonDocument docRes(response);
        source->sendMessage(QString::fromUtf8(docRes.toJson(QJsonDocument::Compact)));
        return;
    }

    QJsonObject obj = doc.object();
    QString type = obj["type"].toString();
    QJsonObject response;

    qDebug() << "Processing message type:" << type;

    try {
        if (type == "login") {
            response = account->login(obj);
            if(response["status"]=="success"){
                source->setUsername(obj["username"].toString());
            }
        }
        else if (type == "logout") {
            response = account->logout(obj);
            gameManager->removePlayerFromWaitingRoom(source->getUsername());
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
                gameManager->removePlayerFromWaitingRoom(oldUsername);
                source->setUsername(newUsername);
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
            } else {
                gameManager->addPlayerToWaitingRoom(source, username);
                return;
            }
        }
        else if (type == "leave_waiting_room") {
            QString username = source->getUsername();
            if (!username.isEmpty() && username != "unknown") {
                gameManager->removePlayerFromWaitingRoom(username);
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
        else if (type == "Player_Selected_Card") {
            QString username = source->getUsername();
            QJsonObject cardObj = obj["card"].toObject();
            if (cardObj.isEmpty()) {
                qWarning() << "Received Player_Selected_Card, but 'card' object is missing or invalid.";
                response = QJsonObject{{"type", "Player_Selected_Card"}, {"status", "error"}, {"message", "Invalid card data."}};
            } else {
                bool handled = false;
                for (GameSession* session : gameManager->getActiveGameSessions()) {
                    if (session->getPlayersMap().contains(username)) {
                        session->processClientAction(username, obj);
                        handled = true;
                        break;
                    }
                }
                if (!handled) {
                    qWarning() << "Received card selection from" << username << "but no active game session found for them.";
                    response = QJsonObject{{"type", "Player_Selected_Card"}, {"status", "error"}, {"message", "No active game session."}};
                    QJsonDocument docRes(response);
                    source->sendMessage(QString::fromUtf8(docRes.toJson(QJsonDocument::Compact)));
                }
                return;
            }
        }
        else if (type == "Swap_Request") {
            QString requestFromUsername = obj["request_from_username"].toString();
            QString requestToUsername = obj["request_to_username"].toString();
            QJsonObject cardToSwapJson = obj["card_to_swap"].toObject();

            bool handled = false;
            for (GameSession* session : gameManager->getActiveGameSessions()) {
                if (session->getPlayersMap().contains(requestFromUsername)) {
                    session->handleSwapRequest(requestFromUsername, requestToUsername, cardToSwapJson);
                    handled = true;
                    break;
                }
            }
            if (!handled) {
                qWarning() << "Swap_Request from" << requestFromUsername << "not handled: No active game session found.";
                QJsonObject errorMsg;
                errorMsg["type"] = "Swap_Notification";
                errorMsg["status"] = "error";
                errorMsg["message"] = "Swap request failed: Not in an active game session.";
                source->sendMessage(QString::fromUtf8(QJsonDocument(errorMsg).toJson(QJsonDocument::Compact)));
            }
            return;
        }
        else if (type == "Swap_Response") {
            QString responseFromUsername = obj["response_from_username"].toString();
            QString requestFromUsername = obj["request_from_username"].toString();
            bool accepted = obj["accepted"].toBool();
            QJsonObject cardToSwapBackJson = obj["card_to_swap_back"].toObject();

            bool handled = false;
            for (GameSession* session : gameManager->getActiveGameSessions()) {
                if (session->getPlayersMap().contains(responseFromUsername)) {
                    session->handleSwapResponse(responseFromUsername, requestFromUsername, accepted, cardToSwapBackJson);
                    handled = true;
                    break;
                }
            }
            if (!handled) {
                qWarning() << "Swap_Response from" << responseFromUsername << "not handled: No active game session found.";
            }
            return;
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
    catch (const GameException& ex) {
        response = QJsonObject{
            {"type", type},
            {"status", "error"},
            {"message", QString("Game Error: %1").arg(ex.what())}
        };
    }
    catch (const std::exception& ex) {
        response = QJsonObject{
            {"type", type},
            {"status", "error"},
            {"message", QString("Server Error: %1").arg(ex.what())}
        };
    }

    if (!response.isEmpty() && type != "Player_Selected_Card" && type != "start_game" && type != "Swap_Request" && type != "Swap_Response") {
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
        gameManager->removePlayerFromWaitingRoom(channel->getUsername());
        for (GameSession* session : gameManager->getActiveGameSessions()) {
            if (session->getPlayersMap().contains(channel->getUsername())) {
            }
        }
        clients.removeOne(channel);
        channel->deleteLater();
    }
}
