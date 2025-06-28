#include "GameManager.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>

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
        return;
    }
    if (m_waitingClients.size() >= 4) {
        qDebug() << "GameManager: Waiting room is full, cannot add" << username;
        return;
    }

    m_waitingClients.insert(username, clientChannel);
    qDebug() << "GameManager: Player" << username << "added to waiting room. Current count:" << m_waitingClients.size();

    tryStartGame();
}

void GameManager::removePlayerFromWaitingRoom(const QString& username) {
    if (m_waitingClients.contains(username)) {
        m_waitingClients.remove(username);
        qDebug() << "GameManager: Player" << username << "removed from waiting room. Current count:" << m_waitingClients.size();
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
        } catch (const std::exception& ex) {
            qWarning() << "GameManager: Unexpected error starting GameSession:" << ex.what();
            for (const QString& username : playerUsernames) {
                m_waitingClients.insert(username, playerChannelsForSession.value(username));
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
