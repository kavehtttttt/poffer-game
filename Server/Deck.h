#ifndef DECK_H
#define DECK_H
#include"Card.h"
#include<QList>
#include <algorithm>
#include <random>
#include <chrono>


class Deck
{
public:
    Deck();
    void initializeDeck();
    void shuffle();
    int size() const;
    bool isEmpty() const;
    void resetDeck();
    QList<Card>dealCards(int count);
private:
    QList<Card> Cards;
};

#endif // DECK_H
