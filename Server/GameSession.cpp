#include "GameSession.h"
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QRandomGenerator>
#include <algorithm>
#include <stdexcept>
#include <chrono>

GameSession::GameSession(const QStringList& playerUsernames, QMap<QString, chanells*>& playerChannels, Users* usersRef, QObject *parent)
    : QObject(parent),
    m_deck(),
    m_currentRound(0),
    m_startingPlayer(nullptr),
    m_currentPlayerTurn(nullptr),
    m_cardsSelectedInCurrentSequence(0),
    m_usersRef(usersRef)
{
    if (playerUsernames.size() != 4) {
        throw GameException("GameSession must be initialized with exactly 4 players.");
    }
    qDebug() << "GameSession constructor called. Initializing game for players:";
    for (const QString& username : playerUsernames) {
        if (playerChannels.contains(username)) {
            chanells* channel = playerChannels.value(username);
            PlayerInGame* player = new PlayerInGame(username, channel, this);
            m_players.append(player);
            m_playersMap.insert(username, player);
            qDebug() << "- Player added:" << username;


            connect(player, &PlayerInGame::cardSelected, this, &GameSession::handlePlayerCardSelection);
            connect(player, &PlayerInGame::turnTimedOut, this, &GameSession::handlePlayerTurnTimeout);
        } else {
            throw GameException(QString("GameSession: Channel not found for username: %1").arg(username));
        }
    }

    connect(&m_turnTimer, &QTimer::timeout, this, [this]() {
        if (m_currentPlayerTurn) {
            handlePlayerTurnTimeout(m_currentPlayerTurn);
        }
    });

    qDebug() << "GameSession initialized with" << m_players.size() << "players.";
}

QMap<QString, PlayerInGame*> GameSession::getPlayersMap() const {
    return m_playersMap;
}

void GameSession::processClientAction(const QString& username, const QJsonObject& actionData) {
    if (!m_playersMap.contains(username)) {
        throw GameException(QString("Action received for non-existent player: %1").arg(username));
    }
    PlayerInGame* player = m_playersMap.value(username);

    QString type = actionData["type"].toString();

    if (type == "Player_Selected_Card") {
        QJsonObject cardObj = actionData["card"].toObject();
        Card selectedCard(cardObj["suit"].toInt(), cardObj["rank"].toInt());

        handlePlayerCardSelection(player, selectedCard);
    }
    else {
        qWarning() << "GameSession: Unknown client action type received:" << type;
    }
}

void GameSession::startGame(){
    m_currentRound = 1;
    startRound();

    QJsonObject gameStartMessage;
    gameStartMessage["type"] = "Game_Started";
    gameStartMessage["status"] = "success";
    gameStartMessage["message"] = "The game has officially started! Good luck!";

    QJsonArray playersArray;
    for (PlayerInGame* player : m_players) {
        playersArray.append(player->getUsername());
    }
    gameStartMessage["players_in_game"] = playersArray;

    QJsonDocument doc(gameStartMessage);
    QString msg = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));

    qDebug() << "GameSession: Sending Game_Started message to all players.";
    for(PlayerInGame* player : m_players){
        if (player->getClientChannel()) {
            player->getClientChannel()->sendMessage(msg);
        } else {
            qWarning() << "GameSession: Player" << player->getUsername() << "has no client channel. Cannot send Game_Started message.";
        }
    }
}

void GameSession::startRound(){
    qDebug() << "Starting Round" << m_currentRound;
    m_deck.resetDeck();
    m_deck.shuffle();

    for (PlayerInGame* player : m_players) {
        player->clearHand();
    }
    m_cardsSelectedInCurrentSequence = 0;
    m_currentSelections.clear();

    determineStartingPlayer();
    dealInitialCards();

    QJsonObject roundStartMsg;
    roundStartMsg["type"] = "Round_Start";
    roundStartMsg["round_number"] = m_currentRound;
    roundStartMsg["starting_player"] = m_startingPlayer->getUsername();
    QJsonDocument doc(roundStartMsg);
    QString msg = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));

    for (PlayerInGame* player : m_players) {
        if (player->getClientChannel()) {
            player->getClientChannel()->sendMessage(msg);
        }
    }

    m_currentPlayerTurn = m_startingPlayer;
    QJsonObject turnMsg;
    turnMsg["type"] = "Your_Turn";
    turnMsg["message"] = "It's your turn to select a card!";
    QJsonArray cardsInHandArray;
    for (const Card& card : m_currentPlayerTurn->getHand()->getCards()) {
        cardsInHandArray.append(card.toJson());
    }
    turnMsg["cards_in_hand"] = cardsInHandArray;

    QJsonDocument turnDoc(turnMsg);
    m_currentPlayerTurn->getClientChannel()->sendMessage(QString::fromUtf8(turnDoc.toJson(QJsonDocument::Compact)));
    m_turnTimer.start(20 * 1000);
}

void GameSession::determineStartingPlayer() {
    qDebug() << "Determining starting player for Round" << m_currentRound;
    QList<QPair<PlayerInGame*, Card>> diamondCardsForComparison;

    QList<Card> allDiamondSuitCards;
    for(int i = static_cast<int>(CardRank::Two); i <= static_cast<int>(CardRank::Bitcoin); ++i) {
        allDiamondSuitCards.append(Card(CardSuit::Diamond, static_cast<CardRank>(i)));
    }
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine rng(seed);
    std::shuffle(allDiamondSuitCards.begin(), allDiamondSuitCards.end(), rng);

    if (allDiamondSuitCards.size() < m_players.size()) {
        throw GameException("Not enough diamond cards to determine starting player (deck error).");
    }

    for (int i=0; i < m_players.size(); ++i) {
        Card dealtDiamond = allDiamondSuitCards.takeFirst();
        diamondCardsForComparison.append({m_players[i], dealtDiamond});
        qDebug() << "Player" << m_players[i]->getUsername() << "gets Diamond for comparison:" << dealtDiamond.toString();
    }

    PlayerInGame* roundStarter = nullptr;
    Card highestDiamondCard(CardSuit::Diamond, static_cast<CardRank>(CardRank::Two));

    for (const auto& pair : diamondCardsForComparison) {
        if (pair.second.getValue() > highestDiamondCard.getValue()) {
            highestDiamondCard = pair.second;
            roundStarter = pair.first;
        } else if (pair.second.getValue() == highestDiamondCard.getValue()) {
            if (PofferRankEvaluator::getSuitHierarchyValue(pair.second.getSuit()) > PofferRankEvaluator::getSuitHierarchyValue(highestDiamondCard.getSuit())) {
                highestDiamondCard = pair.second;
                roundStarter = pair.first;
            }
        }
    }

    if (roundStarter) {
        m_startingPlayer = roundStarter;
        qDebug() << "Starting player for Round" << m_currentRound << ":" << m_startingPlayer->getUsername();
    } else {
        throw GameException("Failed to determine starting player.");
    }
}


void GameSession::dealInitialCards() {
    qDebug() << "Dealing initial cards.";
    if (m_startingPlayer == nullptr) {
        throw GameException("Cannot deal initial cards: Starting player not determined.");
    }

    QList<Card> initialSeven = m_deck.dealCards(7);
    m_startingPlayer->receiveCards(initialSeven);

    QJsonObject handMsg;
    handMsg["type"] = "Initial_Hand";
    handMsg["message"] = "Here are your 7 cards to start the sequence.";
    QJsonArray cardsArray;
    for (const Card& card : initialSeven) {
        cardsArray.append(card.toJson());
    }
    handMsg["cards"] = cardsArray;
    QJsonDocument doc(handMsg);
    m_startingPlayer->getClientChannel()->sendMessage(QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));

    m_currentPlayerTurn = m_startingPlayer;
    m_cardsSelectedInCurrentSequence = 0;
    m_currentSelections.clear();

    QJsonObject turnMsg;
    turnMsg["type"] = "Your_Turn";
    turnMsg["message"] = "It's your turn to select a card from your 7 cards!";
    turnMsg["cards_in_hand"] = cardsArray;
    QJsonDocument turnDoc(turnMsg);
    m_currentPlayerTurn->getClientChannel()->sendMessage(QString::fromUtf8(turnDoc.toJson(QJsonDocument::Compact)));
    m_turnTimer.start(20 * 1000);
}

void GameSession::collectPlayerSelections() {
}


void GameSession::handlePlayerCardSelection(PlayerInGame* player, const Card& selectedCard) {
    if (player != m_currentPlayerTurn) {
        qWarning() << "GameSession: Received card selection from wrong player:" << player->getUsername() << ". Expected:" << m_currentPlayerTurn->getUsername();
        return;
    }
    if (!player->getHand()->removeCard(selectedCard)) {
        qWarning() << "GameSession: Player" << player->getUsername() << "tried to select a card not in hand:" << selectedCard.toString();
        QJsonObject invalidMsg;
        invalidMsg["type"] = "Invalid_Selection";
        invalidMsg["message"] = "Selected card is not in your hand. Please choose again.";
        QJsonDocument doc(invalidMsg);
        player->getClientChannel()->sendMessage(QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
        return;
    }

    m_turnTimer.stop();
    m_currentSelections.append({player, selectedCard});
    m_cardsSelectedInCurrentSequence++;

    qDebug() << "Player" << player->getUsername() << "selected card:" << selectedCard.toString();

    QJsonObject selectionUpdateMsg;
    selectionUpdateMsg["type"] = "Player_Selection_Update";
    selectionUpdateMsg["username"] = player->getUsername();
    selectionUpdateMsg["card_selected"] = selectedCard.toJson();
    QJsonDocument updateDoc(selectionUpdateMsg);
    QString updateMsg = QString::fromUtf8(updateDoc.toJson(QJsonDocument::Compact));
    for (PlayerInGame* p : m_players) {
        if (p->getClientChannel()) {
            p->getClientChannel()->sendMessage(updateMsg);
        }
    }

    if (m_cardsSelectedInCurrentSequence < 4) {
        int currentPlayerIndex = m_players.indexOf(player);
        PlayerInGame* nextPlayerInSequence = m_players[(currentPlayerIndex + 1) % m_players.size()];

        QList<Card> cardsToPass = player->getHand()->getCards();
        player->clearHand();
        nextPlayerInSequence->receiveCards(cardsToPass);

        qDebug() << "Passing cards from" << player->getUsername() << "to" << nextPlayerInSequence->getUsername();

        m_currentPlayerTurn = nextPlayerInSequence;
        QJsonObject turnMsg;
        turnMsg["type"] = "Your_Turn";
        turnMsg["message"] = "It's your turn to select a card!";
        QJsonArray cardsInHandArray;
        for (const Card& card : m_currentPlayerTurn->getHand()->getCards()) {
            cardsInHandArray.append(card.toJson());
        }
        turnMsg["cards_in_hand"] = cardsInHandArray;
        QJsonDocument turnDoc(turnMsg);
        m_currentPlayerTurn->getClientChannel()->sendMessage(QString::fromUtf8(turnDoc.toJson(QJsonDocument::Compact)));
        m_turnTimer.start(20 * 1000);
    }
    else if (m_cardsSelectedInCurrentSequence == m_players.size() * 5) {
        qDebug() << "All 20 cards selected for current round. Evaluating round.";
        evaluateRound();
    } else {
        int currentPlayerIndex = m_players.indexOf(player);
        m_currentPlayerTurn = m_players[(currentPlayerIndex + 1) % m_players.size()];

        QJsonObject turnMsg;
        turnMsg["type"] = "Your_Turn";
        turnMsg["message"] = "It's your turn to select a card for the next sequence!";
        QJsonArray cardsInHandArray;
        for (const Card& card : m_currentPlayerTurn->getHand()->getCards()) {
            cardsInHandArray.append(card.toJson());
        }
        turnMsg["cards_in_hand"] = cardsInHandArray;
        QJsonDocument turnDoc(turnMsg);
        m_currentPlayerTurn->getClientChannel()->sendMessage(QString::fromUtf8(turnDoc.toJson(QJsonDocument::Compact)));
        m_turnTimer.start(20 * 1000);
    }
}


void GameSession::evaluateRound() {
    qDebug() << "Evaluating Round" << m_currentRound << "hands.";
    QMap<PlayerInGame*, PofferRankEvaluator::HandRankResult> playerRanks;

    for (PlayerInGame* player : m_players) {
        if (player->getHand()->getCards().size() != 5) {
            throw GameException(QString("Player %1 has incorrect hand size for evaluation in round %2.").arg(player->getUsername()).arg(m_currentRound));
        }
        playerRanks[player] = PofferRankEvaluator::evaluateHand(player->getHand()->getCards());
        qDebug() << "Player" << player->getUsername() << "hand rank:" << playerRanks[player].description;
    }

    PlayerInGame* roundWinner = nullptr;
    PofferRankEvaluator::HandRankResult highestRank;
    bool firstPlayer = true;

    for (PlayerInGame* player : m_players) {
        if (firstPlayer) {
            highestRank = playerRanks[player];
            roundWinner = player;
            firstPlayer = false;
        } else {
            if (playerRanks[player] > highestRank) {
                highestRank = playerRanks[player];
                roundWinner = player;
            }
        }
    }

    if (roundWinner) {
        roundWinner->incrementRoundsWon();
        qDebug() << "Round" << m_currentRound << "winner:" << roundWinner->getUsername();

        QMap<QString, QString> roundResults;
        for (PlayerInGame* player : m_players) {
            roundResults[player->getUsername()] = (player == roundWinner) ? "Win" : "Loss";
        }

        emit roundEnded(m_currentRound, roundWinner->getUsername(), roundResults);

        if (roundWinner->getRoundsWon() == 2) {
            endGame(roundWinner);
        } else if (m_currentRound == 3) {
            PlayerInGame* gameWinner = nullptr;
            int maxRoundsWon = -1;
            for (PlayerInGame* player : m_players) {
                if (player->getRoundsWon() > maxRoundsWon) {
                    maxRoundsWon = player->getRoundsWon();
                    gameWinner = player;
                } else if (player->getRoundsWon() == maxRoundsWon && maxRoundsWon > 0) {
                    gameWinner = nullptr;
                }
            }
            endGame(gameWinner);
        } else {
            m_currentRound++;
            startRound();
        }
    } else {
        qWarning() << "No winner determined for Round" << m_currentRound << ". This indicates an evaluation error or a draw scenario not handled.";
        endGame(nullptr, true);
    }
}

void GameSession::endGame(PlayerInGame* winner, bool earlyExit) {
    qDebug() << "GameSession: Game ended.";
    m_turnTimer.stop();

    QString winnerUsername = "No Winner";
    if (winner) {
        winnerUsername = winner->getUsername();
        qDebug() << "Game Winner:" << winnerUsername;
    } else if (earlyExit) {
        qDebug() << "Game ended early due to exit/disconnection/error.";
    }

    QMap<QString, QString> finalRoundResults;
    saveGameHistory(finalRoundResults, winner);

    emit gameEnded(winnerUsername);

    emit gameSessionDestroyed(this);
}

void GameSession::saveGameHistory(const QMap<QString, QString>& finalRoundResults, PlayerInGame* winner) {
    qDebug() << "GameSession: Saving game history.";
    QString winnerUsername = winner ? winner->getUsername() : "N/A";

    for (PlayerInGame* player : m_players) {
        GameHistoryEntry entry;
        entry.dateOfPlay = QDateTime::currentDateTime();

        QStringList opponents;
        for (PlayerInGame* otherPlayer : m_players) {
            if (otherPlayer != player) {
                opponents.append(otherPlayer->getUsername());
            }
        }
        entry.opponentUsername = opponents.join(", ");

        if (player == winner) {
            entry.finalResult = "Win";
        } else if (!winner && finalRoundResults.isEmpty() && m_currentRound < 3 && !m_playerInactivityCount.contains(player->getUsername())) {
            entry.finalResult = "Draw/Incomplete";
        }
        else {
            entry.finalResult = "Loss";
        }

        entry.roundResults.clear();
        for(int i = 0; i < player->getRoundsWon(); ++i) {
            entry.roundResults.append(QString("Won Round %1").arg(i+1));
        }
        if (player->getRoundsWon() == 0 && entry.finalResult != "Loss") {
            entry.roundResults.append("No Rounds Won");
        }


        try {
            m_usersRef->addGameHistory(player->getUsername(), entry);
            qDebug() << "Game history saved for" << player->getUsername();
        } catch (const UserException& ex) {
            qWarning() << "Failed to save game history for" << player->getUsername() << ":" << ex.what();
        }
    }
}

void GameSession::handlePlayerDisconnected(chanells* channel) {
    qDebug() << "GameSession: Player disconnected:" << channel->getUsername();
    PlayerInGame* disconnectedPlayer = nullptr;
    for (PlayerInGame* player : m_players) {
        if (player->getClientChannel() == channel) {
            disconnectedPlayer = player;
            break;
        }
    }

    if (disconnectedPlayer) {
        qDebug() << "Player" << disconnectedPlayer->getUsername() << "disconnected. Game ending for all.";

        m_players.removeOne(disconnectedPlayer);
        m_playersMap.remove(disconnectedPlayer->getUsername());

        QJsonObject disconnectMsg;
        disconnectMsg["type"] = "Player_Disconnected";
        disconnectMsg["username"] = disconnectedPlayer->getUsername();
        disconnectMsg["message"] = QString("%1 has disconnected. Game ending for all players.").arg(disconnectedPlayer->getUsername());
        QJsonDocument doc(disconnectMsg);
        QString msg = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
        for (PlayerInGame* player : m_players) {
            if (player->getClientChannel()) {
                player->getClientChannel()->sendMessage(msg);
            }
        }
        endGame(nullptr, true);
    }
}

void GameSession::handlePlayerExit(PlayerInGame* player) {
    qDebug() << "GameSession: Player exited:" << player->getUsername();

    m_players.removeOne(player);
    m_playersMap.remove(player->getUsername());

    QJsonObject exitMsg;
    exitMsg["type"] = "Player_Exited";
    exitMsg["username"] = player->getUsername();
    exitMsg["message"] = QString("%1 has exited the game. Game ending for all players.").arg(player->getUsername());
    QJsonDocument doc(exitMsg);
    QString msg = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
    for (PlayerInGame* p : m_players) {
        if (p->getClientChannel()) {
            p->getClientChannel()->sendMessage(msg);
        }
    }
    endGame(nullptr, true);
}

void GameSession::handlePlayerTurnTimeout(PlayerInGame* player) {
    qDebug() << "GameSession: Player turn timeout for" << player->getUsername();
    m_playerInactivityCount[player->getUsername()]++;

    if (m_playerInactivityCount[player->getUsername()] == 1) {
        QJsonObject warningMsg;
        warningMsg["type"] = "Inactivity_Warning";
        warningMsg["message"] = "You have 10 seconds to select a card or one will be chosen randomly!";
        QJsonDocument doc(warningMsg);
        player->getClientChannel()->sendMessage(QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
        m_turnTimer.start(10 * 1000);
    } else if (m_playerInactivityCount[player->getUsername()] == 2) {
        qDebug() << "Player" << player->getUsername() << "timed out twice. Choosing random card.";
        if (!player->getHand()->getCards().isEmpty()) {
            QList<Card> currentHandCards = player->getHand()->getCards();
            Card randomCard = currentHandCards.at(QRandomGenerator::global()->bounded(currentHandCards.size()));

            handlePlayerCardSelection(player, randomCard);
        } else {
            qWarning() << "Player" << player->getUsername() << "timed out twice but has no cards! Ending game.";
            endGame(nullptr, true);
        }
        m_playerInactivityCount[player->getUsername()] = 0;
    } else if (m_playerInactivityCount[player->getUsername()] > 2) {
        qDebug() << "Player" << player->getUsername() << "lost due to repeated inactivity.";
        endGame(nullptr, true);
    }
}
