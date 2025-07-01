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

public slots:
    void handleSwapRequest(const QString& requestFromUsername, const QString& requestToUsername, const QJsonObject& cardToSwapJson);
    void handleSwapResponse(const QString& responseFromUsername, const QString& requestFromUsername, bool accepted, const QJsonObject& cardToSwapBackJson);


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
    bool isSwapAllowed() const;
    void executeSwap(PlayerInGame* player1, const Card& card1, PlayerInGame* player2, const Card& card2);

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
    int m_currentSequenceNumber;
    QMap<QString, QJsonObject> m_pendingSwapRequests; // Key: request_to_username, Value: original Swap_Request JSON from Server::handleMessage
    QMap<QString, int> m_playerSwapsInitiatedThisRound; // Key: username, Value: count of swaps initiated by player in current round

    Users* m_usersRef;

};

#endif // GAMESESSION_H
