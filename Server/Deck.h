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
    QList<Card>dealCards(int count);
private:
    QList<Card> Cards;
};

#endif // DECK_H
