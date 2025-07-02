#ifndef GAMESESSION_H
#define GAMESESSION_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QTimer>
#include <QDateTime>
#include "Deck.h"
#include "PlayerInGame.h"
#include "PofferRankEvaluator.h"
#include "GameHistoryEntry.h"
#include "Users.h"
#include "chanells.h"

class GameSession : public QObject
{
    Q_OBJECT

public:
    explicit GameSession(const QStringList& playerUsernames, QMap<QString, chanells*>& playerChannels, Users* usersRef, QObject *parent = nullptr);
    ~GameSession() override = default;

    void startGame();
    QMap<QString, PlayerInGame*> getPlayersMap() const;
    void processClientAction(const QString& username, const QJsonObject& actionData);

signals:
    void roundEnded(int roundNumber, const QString& winnerUsername, const QMap<QString, QString>& roundResults);
    void gameEnded(const QString& winnerUsername);
    void gameSessionDestroyed(GameSession* session);

public slots:
    void handlePlayerDisconnected(chanells* channel);
    void handlePlayerExit(PlayerInGame* player);
    void handleSwapRequest(PlayerInGame* player, const QString& requestToUsername, const QJsonObject& cardToSwapJson);
    void handleSwapResponse(PlayerInGame* player, const QString& requestFromUsername, bool accepted, const QJsonObject& cardToSwapBackJson);
    void handlePauseRequest(PlayerInGame* player);
    void handleResumeRequest(PlayerInGame* player);

private slots:
    void startRound();
    void determineStartingPlayer();
    void dealInitialCards();
    void collectPlayerSelections();
    void handlePlayerCardSelection(PlayerInGame* player, const Card& selectedCard);
    void handlePlayerTurnTimeout(PlayerInGame* player);
    void handlePauseTimeout();

private:
    void executeSwap(PlayerInGame* player1, const Card& card1, PlayerInGame* player2, const Card& card2);
    bool isSwapAllowed() const;
    void evaluateRound();
    void endGame(PlayerInGame* winner, bool earlyExit = false, PlayerInGame* losingPlayerOnPauseTimeout = nullptr);
    void saveGameHistory(const QMap<QString, QString>& finalRoundResults, PlayerInGame* winner, bool earlyExit, PlayerInGame* losingPlayerOnPauseTimeout);

    Deck m_deck;
    QList<PlayerInGame*> m_players;
    QMap<QString, PlayerInGame*> m_playersMap;
    int m_currentRound;
    PlayerInGame* m_startingPlayer;
    PlayerInGame* m_currentPlayerTurn;
    QTimer m_turnTimer;
    QMap<QString, int> m_playerInactivityCount;
    int m_cardsSelectedInCurrentSequence;
    QList<QPair<PlayerInGame*, Card>> m_currentSelections;
    int m_currentSequenceNumber;
    QMap<QString, int> m_playerSwapsInitiatedThisRound;
    QMap<QString, QJsonObject> m_pendingSwapRequests;
    Users* m_usersRef;

    bool m_isPaused;
    QTimer m_pauseTimer;
    QString m_pauseInitiatorUsername;
    QMap<QString, int> m_playerPauseCounts;
    int m_remainingTurnTime;
};

#endif // GAMESESSION_H
