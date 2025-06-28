#ifndef CARD_H
#define CARD_H

#include <QString>
#include <QJsonObject>

enum class CardSuit {
    Diamond = 0,
    Dollar = 1,
    Coin = 2,
    Gold = 3
};

enum class CardRank {
    Bitcoin = 14,
    King = 13,
    Queen = 12,
    Soldier = 11,
    Ten = 10,
    Nine = 9,
    Eight = 8,
    Seven = 7,
    Six = 6,
    Five = 5,
    Four = 4,
    Three = 3,
    Two = 2
};

class Card
{
public:
    Card();
    Card(CardSuit suit, CardRank rank);
    Card(int suitValue, int rankValue);

    CardSuit getSuit() const;
    CardRank getRank() const;
    int getValue() const;

    bool isSameSuit(const Card& other) const;
    bool isSameRank(const Card& other) const;

    QString toString() const;
    QJsonObject toJson() const;

    bool operator==(const Card& other) const;

private:
    CardSuit m_suit;
    CardRank m_rank;
};

#endif // CARD_H
