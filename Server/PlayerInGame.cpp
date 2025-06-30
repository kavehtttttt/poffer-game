#include "PlayerInGame.h"
#include <QDebug>

PlayerInGame::PlayerInGame(const QString& username, chanells* clientChannel, QObject *parent)
    : QObject(parent), m_username(username), m_clientChannel(clientChannel), m_roundsWon(0)
{
}

QString PlayerInGame::getUsername() const {
    return m_username;
}

Hand* PlayerInGame::getHand() {
    return &m_hand;
}

const Hand* PlayerInGame::getHand() const {
    return &m_hand;
}

Hand* PlayerInGame::getFinalHand() {
    return &m_finalHand;
}

const Hand* PlayerInGame::getFinalHand() const {
    return &m_finalHand;
}

chanells* PlayerInGame::getClientChannel() const {
    return m_clientChannel;
}

int PlayerInGame::getRoundsWon() const {
    return m_roundsWon;
}

void PlayerInGame::incrementRoundsWon() {
    m_roundsWon++;
    qDebug() << "Player" << m_username << "won a round. Total rounds won:" << m_roundsWon;
}

void PlayerInGame::clearHand() {
    m_hand.getCards().clear();
    qDebug() << "Player" << m_username << "current hand cleared.";
}

void PlayerInGame::clearFinalHand() {
    m_finalHand.getCards().clear();
    qDebug() << "Player" << m_username << "final hand cleared.";
}

void PlayerInGame::receiveCards(const QList<Card>& cards) {
    m_hand.getCards().clear();
    for (const Card& card : cards) {
        m_hand.addCard(card);
    }
    qDebug() << "Player" << m_username << "received" << cards.size() << "cards for current hand. Hand size:" << m_hand.getCards().size();
}
