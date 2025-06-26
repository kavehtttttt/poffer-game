#include "Deck.h"

Deck::Deck() {}
void Deck::initializeDeck(){
    for(int i=0;i<4;++i)
        for(int j=2;j<15;++j)
            Cards.append(Card(i,j));
}
void Deck::shuffle(){
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine rng(seed);
    std::shuffle(Cards.begin(), Cards.end(), rng);
}
QList<Card> Deck::dealCards(int count){
    QList<Card> deel;
    int cardsToDeal = qMin(count, Cards.size());
    for(int n=0;n<cardsToDeal;++n){
        deel.append(Cards.takeFirst());
    }
    return deel;
}
