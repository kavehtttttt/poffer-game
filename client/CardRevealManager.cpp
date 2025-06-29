#include "CardRevealManager.h"
#include "qjsonobject.h"
#include <QDebug>

CardRevealManager::CardRevealManager(QMap<QString, CardWidget*>& cards, QObject* parent)
    : QObject(parent), cardMap(cards), currentIndex(0)
{
    connect(&timer, &QTimer::timeout, this, &CardRevealManager::revealNextCard);
}

void CardRevealManager::revealComparisonCards(const QJsonArray& cards)
{
    comparisonCards = cards;
    currentIndex = 0;
    timer.start(1000);  // هر 1 ثانیه یک کارت نمایش داده شود
}

QString CardRevealManager::cardIdFromUsername(const QString& username)
{
    if (username == "b") return "B3";
    if (username == "c") return "T3";
    if (username == "jj") return "L3";
    if (username == "d") return "R3";
    return QString();
}

QString CardRevealManager::formatCardText(int rank, int suit)
{
    QString suitStr;
    switch (suit) {
    case 0: suitStr = "♦"; break;
    case 1: suitStr = "$"; break;
    case 2: suitStr = "€"; break;
    case 3: suitStr = "¤"; break;
    default: suitStr = "?"; break;
    }
    return QString::number(rank) + suitStr;
}

void CardRevealManager::revealNextCard()
{
    if (currentIndex >= comparisonCards.size()) {
        timer.stop();
        return;
    }

    QJsonObject obj = comparisonCards[currentIndex].toObject();
    QString username = obj["username"].toString();
    QJsonObject cardObj = obj["card"].toObject();
    int rank = cardObj["rank"].toInt();
    int suit = cardObj["suit"].toInt();

    QString cardId = cardIdFromUsername(username);
    QString cardText = formatCardText(rank, suit);

    if (cardMap.contains(cardId)) {
        cardMap[cardId]->setCardText(cardText);
        cardMap[cardId]->setCardState(0);  // Normal = سفید
        qDebug() << "Revealed card for" << username << ":" << cardId << "->" << cardText;
    } else {
        qDebug() << "Card ID not found for" << username << "expected:" << cardId;
    }

    currentIndex++;
}
