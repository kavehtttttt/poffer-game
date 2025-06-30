#ifndef GAMESESSION_H
#define GAMESESSION_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QTimer>
#include "PlayerInGame.h"
#include "Deck.h"
#include "GameHistoryEntry.h"
#include "Users.h"
#include "chanells.h"
#include "GameException.h"
#include"Pofferrankevaluator.h"

class GameSession : public QObject
{
    Q_OBJECT
public:
    explicit GameSession(const QStringList& playerUsernames, QMap<QString, chanells*>& playerChannels, Users* usersRef, QObject *parent = nullptr);

    void startGame();
    void startRound();

    void processClientAction(const QString& username, const QJsonObject& actionData);

    QMap<QString, PlayerInGame*> getPlayersMap() const;

signals:
    void gameEnded(QString winnerUsername);
    void roundEnded(int roundNumber, QString winnerUsername, QMap<QString, QString> roundResults);
    void gameSessionDestroyed(GameSession* session);

private slots:
    void handlePlayerCardSelection(PlayerInGame* player, const Card& card);
    void handlePlayerTurnTimeout(PlayerInGame* player);
    void handlePlayerDisconnected(chanells* channel);
    void handlePlayerExit(PlayerInGame* player);

private:
    void determineStartingPlayer();
    void dealInitialCards();
    void collectPlayerSelections();
    void evaluateRound();
    void endGame(PlayerInGame* winner = nullptr, bool earlyExit = false);
    void saveGameHistory(const QMap<QString, QString>& finalRoundResults, PlayerInGame* winner, bool earlyExit);

    QList<PlayerInGame*> m_players;
    QMap<QString, PlayerInGame*> m_playersMap;
    Deck m_deck;
    int m_currentRound;
    PlayerInGame* m_startingPlayer;
    PlayerInGame* m_currentPlayerTurn;
    int m_cardsSelectedInCurrentSequence;
    QList<QPair<PlayerInGame*, Card>> m_currentSelections;
    QTimer m_turnTimer;
    QMap<QString, int> m_playerInactivityCount;

    Users* m_usersRef;
};

#endif // GAMESESSION_H
