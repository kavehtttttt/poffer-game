#include "GameManager.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

GameManager::GameManager(Users* usersRef, QObject *parent)
    : QObject(parent),
    m_usersRef(usersRef)
{
    qDebug() << "GameManager initialized.";
}

GameManager::~GameManager() {
    qDebug() << "GameManager destructor called. Cleaning up active game sessions.";
    for (GameSession* session : m_activeGameSessions) {
        if (session) {
            session->disconnect(this);
            session->deleteLater();
        }
    }
    m_activeGameSessions.clear();
}

QList<GameSession*> GameManager::getActiveGameSessions() const {
    return m_activeGameSessions;
}

void GameManager::addPlayerToWaitingRoom(chanells* clientChannel, const QString& username) {
    if (m_waitingClients.contains(username)) {
        qDebug() << "GameManager: Player" << username << "already in waiting room.";
        QJsonObject response;
        response["type"] = "Waiting_Room_Status";
        response["status"] = "info";
        response["message"] = "You are already in the waiting room.";
        response["current_players"] = m_waitingClients.size();
        response["required_players"] = 4;
        QJsonDocument doc(response);
        clientChannel->sendMessage(QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
        return;
    }
    if (m_waitingClients.size() >= 4) {
        qDebug() << "GameManager: Waiting room is full, cannot add" << username;
        QJsonObject response;
        response["type"] = "Waiting_Room_Status";
        response["status"] = "error";
        response["message"] = "Waiting room is full. Please try again later.";
        response["current_players"] = m_waitingClients.size();
        response["required_players"] = 4;
        QJsonDocument doc(response);
        clientChannel->sendMessage(QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
        return;
    }

    m_waitingClients.insert(username, clientChannel);
    qDebug() << "GameManager: Player" << username << "added to waiting room. Current count:" << m_waitingClients.size();

    QJsonObject waitingStatusMessage;
    waitingStatusMessage["type"] = "Waiting_Room_Status";
    waitingStatusMessage["current_players"] = m_waitingClients.size();
    waitingStatusMessage["required_players"] = 4;
    waitingStatusMessage["message"] = QString("%1 player(s) in waiting room. Waiting for %2 more.").arg(m_waitingClients.size()).arg(4 - m_waitingClients.size());

    QJsonDocument doc(waitingStatusMessage);
    QString msg = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));

    for (chanells* channel : m_waitingClients.values()) {
        if (channel) {
            channel->sendMessage(msg);
        }
    }

    tryStartGame();
}

void GameManager::removePlayerFromWaitingRoom(const QString& username) {
    if (m_waitingClients.contains(username)) {
        m_waitingClients.remove(username);
        qDebug() << "GameManager: Player" << username << "removed from waiting room. Current count:" << m_waitingClients.size();

        QJsonObject waitingStatusMessage;
        waitingStatusMessage["type"] = "Waiting_Room_Status";
        waitingStatusMessage["current_players"] = m_waitingClients.size();
        waitingStatusMessage["required_players"] = 4;
        waitingStatusMessage["message"] = QString("%1 player(s) in waiting room. Waiting for %2 more.").arg(m_waitingClients.size()).arg(4 - m_waitingClients.size());

        QJsonDocument doc(waitingStatusMessage);
        QString msg = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));

        for (chanells* channel : m_waitingClients.values()) {
            if (channel) {
                channel->sendMessage(msg);
            }
        }
    }
    for (GameSession* session : m_activeGameSessions) {
        if (session->getPlayersMap().contains(username)) {
            qDebug() << "GameManager: Player" << username << "was in an active game session. Notifying session.";
        }
    }
}

void GameManager::tryStartGame() {
    qDebug() << "GameManager: Trying to start game. Current waiting:" << m_waitingClients.size();
    if (m_waitingClients.size() == 4) {
        qDebug() << "GameManager: Enough players (4) to start a new game session!";

        QStringList playerUsernames;
        QMap<QString, chanells*> playerChannelsForSession;

        for (const QString& username : m_waitingClients.keys()) {
            playerUsernames.append(username);
            playerChannelsForSession.insert(username, m_waitingClients.value(username));
        }

        m_waitingClients.clear();

        try {
            GameSession* newGameSession = new GameSession(playerUsernames, playerChannelsForSession, m_usersRef, this);
            m_activeGameSessions.append(newGameSession);

            connect(newGameSession, &GameSession::gameEnded, this, &GameManager::handleGameSessionEnded);
            connect(newGameSession, &GameSession::gameSessionDestroyed, this, &GameManager::handleGameSessionDestroyed);

            newGameSession->startGame();
            qDebug() << "GameManager: New GameSession started with players:" << playerUsernames.join(", ");

            emit gameSessionStarted(playerUsernames);

        } catch (const GameException& ex) {
            qWarning() << "GameManager: Failed to start GameSession:" << ex.what();
            for (const QString& username : playerUsernames) {
                m_waitingClients.insert(username, playerChannelsForSession.value(username));
            }
            QJsonObject errorMsg;
            errorMsg["type"] = "Game_Start_Failed";
            errorMsg["status"] = "error";
            errorMsg["message"] = QString("Failed to start game: %1. Please try again.").arg(ex.what());
            QJsonDocument doc(errorMsg);
            QString msg = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
            for (chanells* channel : playerChannelsForSession.values()) {
                if (channel) {
                    channel->sendMessage(msg);
                }
            }
        } catch (const std::exception& ex) {
            qWarning() << "GameManager: Unexpected error starting GameSession:" << ex.what();
            for (const QString& username : playerUsernames) {
                m_waitingClients.insert(username, playerChannelsForSession.value(username));
            }
            QJsonObject errorMsg;
            errorMsg["type"] = "Game_Start_Failed";
            errorMsg["status"] = "error";
            errorMsg["message"] = QString("An unexpected error occurred while starting the game: %1. Please try again.").arg(ex.what());
            QJsonDocument doc(errorMsg);
            QString msg = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
            for (chanells* channel : playerChannelsForSession.values()) {
                if (channel) {
                    channel->sendMessage(msg);
                }
            }
        }
    }
}

void GameManager::handleGameSessionEnded(QString winnerUsername) {
    qDebug() << "GameManager: Game Session ended. Winner:" << winnerUsername;
}

void GameManager::handleGameSessionDestroyed(GameSession* session) {
    qDebug() << "GameManager: GameSession destroyed signal received for session:" << session;
    m_activeGameSessions.removeOne(session);
}
