#ifndef PLAYERINGAME_H
#define PLAYERINGAME_H

#include <QObject>
#include <QString>
#include <QList>
#include "Hand.h"
#include "chanells.h"

class PlayerInGame : public QObject
{
    Q_OBJECT

public:
    explicit PlayerInGame(const QString& username, chanells* clientChannel, QObject *parent = nullptr);

    QString getUsername() const;
    Hand* getHand();
    const Hand* getHand() const;
    Hand* getFinalHand();
    const Hand* getFinalHand() const;
    chanells* getClientChannel() const;
    int getRoundsWon() const;

    void incrementRoundsWon();
    void clearHand();
    void clearFinalHand();
    void receiveCards(const QList<Card>& cards);

signals:
    void cardSelected(PlayerInGame* player, const Card& card);
    void turnTimedOut(PlayerInGame* player);

private:
    QString m_username;
    Hand m_hand;
    Hand m_finalHand;   // The 5 cards collected for round evaluation
    chanells* m_clientChannel;
    int m_roundsWon;
};

#endif // PLAYERINGAME_H
