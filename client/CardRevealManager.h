#ifndef CARDREVEALMANAGER_H
#define CARDREVEALMANAGER_H

#include <QObject>
#include <QTimer>
#include <QJsonArray>
#include <QMap>
#include "CardWidget.h"

class CardRevealManager : public QObject
{
    Q_OBJECT

public:
    explicit CardRevealManager(QMap<QString, CardWidget*>& cards, QObject* parent = nullptr);

    void revealComparisonCards(const QJsonArray& cards);

private:
    QMap<QString, CardWidget*>& cardMap;
    QJsonArray comparisonCards;
    int currentIndex;
    QTimer timer;

    QString cardIdFromUsername(const QString& username);
    QString formatCardText(int rank, int suit);

private slots:
    void revealNextCard();
};

#endif // CARDREVEALMANAGER_H
