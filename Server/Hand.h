#ifndef HAND_H
#define HAND_H

#include <QObject>
#include <QList>
#include <QString>
#include "Card.h"


class PofferRankEvaluator;

class Hand : public QObject
{
    Q_OBJECT
public:
    explicit Hand(QObject *parent = nullptr);

    Hand(const QList<Card>& initialCards, QObject *parent = nullptr);

    void addCard(const Card& card);
    bool removeCard(const Card& card);
    QString getRank() const;
    QList<Card> getCards() const;

signals:

private:
    QList<Card> m_cards;
};

#endif // HAND_H
