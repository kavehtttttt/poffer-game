#include "Hand.h"
#include <QDebug>
#include <algorithm>
#include "PofferRankEvaluator.h"

Hand::Hand(QObject *parent) : QObject(parent) {}
Hand::Hand(const QList<Card>& initialCards, QObject *parent) : QObject(parent), m_cards(initialCards)  {}

void Hand::addCard(const Card& card)
{
    m_cards.append(card);
    qDebug() << "Card added to hand. Current hand size:" << m_cards.size();
}


bool Hand::removeCard(const Card& card)
{
    bool removed = m_cards.removeOne(card);
    if (removed) {
        qDebug() << "Card removed from hand. Current hand size:" << m_cards.size();
    } else {
        qDebug() << "Failed to remove card from hand (card not found):" << card.toString();
    }
    return removed;
}


QString Hand::getRank() const
{
    if (m_cards.size() != 5) {
        qWarning() << "Hand::getRank() called with incorrect number of cards:" << m_cards.size() << ". Expected 5 for rank evaluation.";
        return "Invalid Hand Size for Rank Evaluation";
    }

    PofferRankEvaluator::HandRankResult result = PofferRankEvaluator::evaluateHand(m_cards);

    return result.description;
}

QList<Card> Hand::getCards() const
{
    return m_cards;
}
