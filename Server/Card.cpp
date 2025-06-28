#include "Card.h"
#include <QDebug>
#include <QJsonObject>

Card::Card() : m_suit(CardSuit::Diamond), m_rank(CardRank::Two) {}

Card::Card(CardSuit suit, CardRank rank)
    : m_suit(suit), m_rank(rank)
{
}

Card::Card(int suitValue, int rankValue) {
    m_suit = static_cast<CardSuit>(suitValue);
    m_rank = static_cast<CardRank>(rankValue);
}

CardSuit Card::getSuit() const {
    return m_suit;
}

CardRank Card::getRank() const {
    return m_rank;
}

int Card::getValue() const {
    return static_cast<int>(m_rank);
}

bool Card::isSameSuit(const Card& other) const {
    return m_suit == other.getSuit();
}

bool Card::isSameRank(const Card& other) const {
    return m_rank == other.getRank();
}

QString Card::toString() const {
    QString suitStr;
    switch (m_suit) {
    case CardSuit::Diamond: suitStr = "Diamond"; break;
    case CardSuit::Dollar:  suitStr = "Dollar"; break;
    case CardSuit::Coin:    suitStr = "Coin"; break;
    case CardSuit::Gold:    suitStr = "Gold"; break;
    }

    QString rankStr;
    switch (m_rank) {
    case CardRank::Bitcoin: rankStr = "Bitcoin"; break;
    case CardRank::King:    rankStr = "King"; break;
    case CardRank::Queen:   rankStr = "Queen"; break;
    case CardRank::Soldier: rankStr = "Soldier"; break;
    case CardRank::Ten:     rankStr = "10"; break;
    case CardRank::Nine:    rankStr = "9"; break;
    case CardRank::Eight:   rankStr = "8"; break;
    case CardRank::Seven:   rankStr = "7"; break;
    case CardRank::Six:     rankStr = "6"; break;
    case CardRank::Five:    rankStr = "5"; break;
    case CardRank::Four:    rankStr = "4"; break;
    case CardRank::Three:   rankStr = "3"; break;
    case CardRank::Two:     rankStr = "2"; break;
    }
    return rankStr + " of " + suitStr;
}

bool Card::operator==(const Card& other) const {
    return (m_suit == other.m_suit) && (m_rank == other.m_rank);
}

QJsonObject Card::toJson() const {
    QJsonObject obj;
    obj["suit"] = static_cast<int>(m_suit);
    obj["rank"] = static_cast<int>(m_rank);
    return obj;
}
