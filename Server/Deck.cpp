#include "Deck.h"
#include<QDebug>
Deck::Deck() {}
void Deck::initializeDeck(){
    Cards.clear();
    for(int i=0;i<4;++i)
        for(int j=2;j<15;++j)
            Cards.append(Card(i,j));
    qDebug() << "Deck initialized with" << Cards.size() << "cards.";
}
void Deck::shuffle(){
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine rng(seed);
    std::shuffle(Cards.begin(), Cards.end(), rng);
    qDebug() << "Deck shuffled.";
}

int Deck::size() const {
    return Cards.size();
}

bool Deck::isEmpty() const {
    return Cards.isEmpty();
}

void Deck::resetDeck() {
    initializeDeck();
    shuffle();
    qDebug() << "Deck reset and shuffled.";
}
QList<Card> Deck::dealCards(int count){
    QList<Card> deel;
    int cardsToDeal = qMin(count, Cards.size());
    for(int n=0;n<cardsToDeal;++n){
        deel.append(Cards.takeFirst());
    }
    qDebug() << "Dealt" << deel.size() << "cards. Deck size remaining:" << Cards.size();
    return deel;
}
