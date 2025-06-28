#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include <QMap>
#include <QList>
#include <QStringList>
#include "chanells.h"
#include "GameSession.h"
#include "Users.h"

class GameManager : public QObject
{
    Q_OBJECT
public:
    explicit GameManager(Users* usersRef, QObject *parent = nullptr);
    ~GameManager();

    void addPlayerToWaitingRoom(chanells* clientChannel, const QString& username);
    void removePlayerFromWaitingRoom(const QString& username);
    void tryStartGame();

    QList<GameSession*> getActiveGameSessions() const;

signals:
    void waitingListUpdated(QJsonArray waitingPlayers, int currentCount, int requiredCount);
    void gameSessionStarted(const QStringList& playerUsernames);

private slots:
    void handleGameSessionEnded(QString winnerUsername);
    void handleGameSessionDestroyed(GameSession* session);

private:
    QMap<QString, chanells*> m_waitingClients;
    QList<GameSession*> m_activeGameSessions;

    Users* m_usersRef;
};

#endif // GAMEMANAGER_H
