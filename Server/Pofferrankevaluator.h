#ifndef POFFERRANKEVALUATOR_H
#define POFFERRANKEVALUATOR_H

#include <QList>
#include <QString>
#include <QJsonObject>
#include "Card.h"

class PofferRankEvaluator
{
private:
    PofferRankEvaluator() = delete;

public:
    enum class HandRankType {
        MessyHand = 0,
        SinglePairHand,
        DoublePairHand,
        ThreePlusTwoHand,
        Series,
        MscHand,
        FourPlusOneHand,
        OrderHand,
        GoldenHand
    };

    struct HandRankResult {
        HandRankType type;
        QString description;
        QList<Card> relevantCards;
        QList<Card> kickerCards;
        int highestCardValue;
        CardSuit dominantSuit;

        bool operator>(const HandRankResult& other) const;
        bool operator<(const HandRankResult& other) const;
        bool operator==(const HandRankResult& other) const;
    };

    static HandRankResult evaluateHand(QList<Card> cards);

    // Changed to public
    static int getSuitHierarchyValue(CardSuit suit);

private:
    static bool isGoldenHand(const QList<Card>& sortedCards, HandRankResult& result);
    static bool isOrderHand(const QList<Card>& sortedCards, HandRankResult& result);
    static bool isFourPlusOneHand(const QList<Card>& sortedCards, HandRankResult& result);
    static bool isPenthouseHand(const QList<Card>& sortedCards, HandRankResult& result);
    static bool isMscHand(const QList<Card>& sortedCards, HandRankResult& result);
    static bool isSeries(const QList<Card>& sortedCards, HandRankResult& result);
    static bool isThreePlusTwoHand(const QList<Card>& sortedCards, HandRankResult& result);
    static bool isDoublePairHand(const QList<Card>& sortedCards, HandRankResult& result);
    static bool isSinglePairHand(const QList<Card>& sortedCards, HandRankResult& result);
    static bool isMessyHand(const QList<Card>& sortedCards, HandRankResult& result);

    static int compareCardsByValue(const QList<Card>& cards1, const QList<Card>& cards2);
    static int compareCardsBySuitHierarchy(const QList<Card>& cards1, const QList<Card>& cards2);

    static QString getRankDescription(HandRankType type);
};

#endif // POFFERRANKEVALUATOR_H
