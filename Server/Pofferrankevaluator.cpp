#include "PofferRankEvaluator.h"
#include <QDebug>
#include <algorithm>


bool compareCardsForRanking(const Card& c1, const Card& c2) {
    if (c1.getValue() != c2.getValue()) {
        return c1.getValue() > c2.getValue();
    }

    return PofferRankEvaluator::getSuitHierarchyValue(c1.getSuit()) > PofferRankEvaluator::getSuitHierarchyValue(c2.getSuit());
}


bool PofferRankEvaluator::HandRankResult::operator>(const HandRankResult& other) const {
    if (type != other.type) {
        return type > other.type;
    }

    switch (type) {
    case HandRankType::GoldenHand:
    case HandRankType::OrderHand:
    case HandRankType::Series:
        if (highestCardValue != other.highestCardValue) {
            return highestCardValue > other.highestCardValue;
        }
        return PofferRankEvaluator::getSuitHierarchyValue(dominantSuit) > PofferRankEvaluator::getSuitHierarchyValue(other.dominantSuit);

    case HandRankType::FourPlusOneHand:
    case HandRankType::ThreePlusTwoHand:
        if (PofferRankEvaluator::compareCardsByValue(relevantCards, other.relevantCards) != 0) {
            return PofferRankEvaluator::compareCardsByValue(relevantCards, other.relevantCards) > 0;
        }
        return PofferRankEvaluator::compareCardsByValue(kickerCards, other.kickerCards) > 0;

    case HandRankType::MscHand:
    case HandRankType::MessyHand:
    case HandRankType::DoublePairHand:
    case HandRankType::SinglePairHand:
        return PofferRankEvaluator::compareCardsByValue(relevantCards, other.relevantCards) > 0;
    default:
        return false;
    }
}

bool PofferRankEvaluator::HandRankResult::operator<(const HandRankResult& other) const {
    return other > *this;
}

bool PofferRankEvaluator::HandRankResult::operator==(const HandRankResult& other) const {
    if (type != other.type) return false;
    if (description != other.description) return false;
    return true;
}


QString PofferRankEvaluator::getRankDescription(HandRankType type) {
    switch (type) {
    case HandRankType::GoldenHand: return "Golden Hand";
    case HandRankType::OrderHand: return "Order Hand";
    case HandRankType::FourPlusOneHand: return "4+1 Hand";
    case HandRankType::ThreePlusTwoHand: return "3+2 Hand";
    case HandRankType::MscHand: return "MSC Hand";
    case HandRankType::Series: return "Series Hand";
    case HandRankType::DoublePairHand: return "Double Pair Hand";
    case HandRankType::SinglePairHand: return "Single Pair Hand";
    case HandRankType::MessyHand: return "Messy Hand";
    default: return "Unknown Rank";
    }
}

int PofferRankEvaluator::getSuitHierarchyValue(CardSuit suit) {
    switch (suit) {
    case CardSuit::Diamond: return 4;
    case CardSuit::Gold:    return 3;
    case CardSuit::Dollar:  return 2;
    case CardSuit::Coin:    return 1;
    default: return 0;
    }
}

int PofferRankEvaluator::compareCardsByValue(const QList<Card>& cards1, const QList<Card>& cards2) {
    int minSize = qMin(cards1.size(), cards2.size());
    for (int i = 0; i < minSize; ++i) {
        if (cards1[i].getValue() != cards2[i].getValue()) {
            return cards1[i].getValue() - cards2[i].getValue();
        }
        int suitCompare = getSuitHierarchyValue(cards1[i].getSuit()) - getSuitHierarchyValue(cards2[i].getSuit());
        if (suitCompare != 0) {
            return suitCompare;
        }
    }
    return cards1.size() - cards2.size();
}

int PofferRankEvaluator::compareCardsBySuitHierarchy(const QList<Card>& cards1, const QList<Card>& cards2) {
    int minSize = qMin(cards1.size(), cards2.size());
    for (int i = 0; i < minSize; ++i) {
        int suitCompare = getSuitHierarchyValue(cards1[i].getSuit()) - getSuitHierarchyValue(cards2[i].getSuit());
        if (suitCompare != 0) {
            return suitCompare;
        }
    }
    return 0;
}

PofferRankEvaluator::HandRankResult PofferRankEvaluator::evaluateHand(QList<Card> cards) {
    HandRankResult result;
    std::sort(cards.begin(), cards.end(), compareCardsForRanking);

    if (isGoldenHand(cards, result)) {
        result.type = HandRankType::GoldenHand;
        result.description = getRankDescription(result.type);
        return result;
    }
    if (isOrderHand(cards, result)) {
        result.type = HandRankType::OrderHand;
        result.description = getRankDescription(result.type);
        return result;
    }
    if (isFourPlusOneHand(cards, result)) {
        result.type = HandRankType::FourPlusOneHand;
        result.description = getRankDescription(result.type);
        return result;
    }
    if (isThreePlusTwoHand(cards, result)) {
        result.type = HandRankType::ThreePlusTwoHand;
        result.description = getRankDescription(result.type);
        return result;
    }
    if (isMscHand(cards, result)) {
        result.type = HandRankType::MscHand;
        result.description = getRankDescription(result.type);
        return result;
    }
    if (isSeries(cards, result)) {
        result.type = HandRankType::Series;
        result.description = getRankDescription(result.type);
        return result;
    }
    if (isDoublePairHand(cards, result)) {
        result.type = HandRankType::DoublePairHand;
        result.description = getRankDescription(result.type);
        return result;
    }
    if (isSinglePairHand(cards, result)) {
        result.type = HandRankType::SinglePairHand;
        result.description = getRankDescription(result.type);
        return result;
    }
    if (isMessyHand(cards, result)) {
        result.type = HandRankType::MessyHand;
        result.description = getRankDescription(result.type);
        return result;
    }

    result.type = HandRankType::MessyHand;
    result.description = "Undetermined Rank - Fallback";
    return result;
}

bool PofferRankEvaluator::isGoldenHand(const QList<Card>& sortedCards, HandRankResult& result) {
    if (sortedCards.size() != 5) return false;

    CardSuit firstSuit = sortedCards[0].getSuit();
    for (int i = 1; i < 5; ++i) {
        if (sortedCards[i].getSuit() != firstSuit) return false;
    }

    if (sortedCards[0].getRank() == CardRank::Bitcoin &&
        sortedCards[1].getRank() == CardRank::King &&
        sortedCards[2].getRank() == CardRank::Queen &&
        sortedCards[3].getRank() == CardRank::Soldier &&
        sortedCards[4].getRank() == CardRank::Ten)
    {
        result.relevantCards = sortedCards;
        result.highestCardValue = sortedCards[0].getValue();
        result.dominantSuit = firstSuit;
        return true;
    }
    return false;
}

bool PofferRankEvaluator::isOrderHand(const QList<Card>& sortedCards, HandRankResult& result) {
    if (sortedCards.size() != 5) return false;

    CardSuit firstSuit = sortedCards[0].getSuit();
    for (int i = 1; i < 5; ++i) {
        if (sortedCards[i].getSuit() != firstSuit) return false;
    }

    for (int i = 0; i < 4; ++i) {
        if (sortedCards[i].getValue() != sortedCards[i+1].getValue() + 1) return false;
    }
    result.relevantCards = sortedCards;
    result.highestCardValue = sortedCards[0].getValue();
    result.dominantSuit = firstSuit;
    return true;
}

bool PofferRankEvaluator::isFourPlusOneHand(const QList<Card>& sortedCards, HandRankResult& result) {
    if (sortedCards.size() != 5) return false;

    if (sortedCards[0].getValue() == sortedCards[1].getValue() &&
        sortedCards[1].getValue() == sortedCards[2].getValue() &&
        sortedCards[2].getValue() == sortedCards[3].getValue())
    {
        result.relevantCards.append(sortedCards[0]);
        result.relevantCards.append(sortedCards[1]);
        result.relevantCards.append(sortedCards[2]);
        result.relevantCards.append(sortedCards[3]);
        result.kickerCards.append(sortedCards[4]);
        result.highestCardValue = sortedCards[0].getValue();
        return true;
    }
    if (sortedCards[1].getValue() == sortedCards[2].getValue() &&
        sortedCards[2].getValue() == sortedCards[3].getValue() &&
        sortedCards[3].getValue() == sortedCards[4].getValue())
    {
        result.relevantCards.append(sortedCards[1]);
        result.relevantCards.append(sortedCards[2]);
        result.relevantCards.append(sortedCards[3]);
        result.relevantCards.append(sortedCards[4]);
        result.kickerCards.append(sortedCards[0]);
        result.highestCardValue = sortedCards[1].getValue();
        return true;
    }
    return false;
}

bool PofferRankEvaluator::isPenthouseHand(const QList<Card>& sortedCards, HandRankResult& result) {
    if (sortedCards.size() != 5) return false;

    bool threeOfAKindFound = false;
    bool pairFound = false;
    CardRank threeRank = CardRank::Two;
    CardRank pairRank = CardRank::Two;

    if (sortedCards[0].getValue() == sortedCards[1].getValue() &&
        sortedCards[1].getValue() == sortedCards[2].getValue()) {
        threeOfAKindFound = true;
        threeRank = sortedCards[0].getRank();
        if (sortedCards[3].getValue() == sortedCards[4].getValue()) {
            pairFound = true;
            pairRank = sortedCards[3].getRank();
        }
    }
    else if (sortedCards[2].getValue() == sortedCards[3].getValue() &&
             sortedCards[3].getValue() == sortedCards[4].getValue()) {
        threeOfAKindFound = true;
        threeRank = sortedCards[2].getRank();
        if (sortedCards[0].getValue() == sortedCards[1].getValue()) {
            pairFound = true;
            pairRank = sortedCards[0].getRank();
        }
    }

    if (threeOfAKindFound && pairFound) {
        for(const Card& card : sortedCards) {
            if (card.getRank() == threeRank) result.relevantCards.append(card);
            else if (card.getRank() == pairRank) result.kickerCards.append(card);
        }
        result.highestCardValue = static_cast<int>(threeRank);
        return true;
    }
    return false;
}

bool PofferRankEvaluator::isThreePlusTwoHand(const QList<Card>& sortedCards, HandRankResult& result) {
    return isPenthouseHand(sortedCards, result);
}


bool PofferRankEvaluator::isMscHand(const QList<Card>& sortedCards, HandRankResult& result) {
    if (sortedCards.size() != 5) return false;

    CardSuit firstSuit = sortedCards[0].getSuit();
    bool allSameSuit = true;
    for (int i = 1; i < 5; ++i) {
        if (sortedCards[i].getSuit() != firstSuit) {
            allSameSuit = false;
            break;
        }
    }
    if (!allSameSuit) return false;

    if (isOrderHand(sortedCards, result)) return false;

    result.relevantCards = sortedCards;
    result.highestCardValue = sortedCards[0].getValue();
    result.dominantSuit = firstSuit;
    return true;
}

bool PofferRankEvaluator::isSeries(const QList<Card>& sortedCards, HandRankResult& result) {
    if (sortedCards.size() != 5) return false;

    for (int i = 0; i < 4; ++i) {
        if (sortedCards[i].getValue() != sortedCards[i+1].getValue() + 1) return false;
    }
    CardSuit firstSuit = sortedCards[0].getSuit();
    bool allSameSuit = true;
    for (int i = 1; i < 5; ++i) {
        if (sortedCards[i].getSuit() != firstSuit) {
            allSameSuit = false;
            break;
        }
    }
    if (allSameSuit) return false;

    result.relevantCards = sortedCards;
    result.highestCardValue = sortedCards[0].getValue();
    return true;
}


bool PofferRankEvaluator::isDoublePairHand(const QList<Card>& sortedCards, HandRankResult& result) {
    if (sortedCards.size() != 5) return false;

    QList<Card> pairs;
    QList<Card> kickers;
    QMap<int, int> rankCounts;

    for (const Card& card : sortedCards) {
        rankCounts[card.getValue()]++;
    }

    int pairCount = 0;
    for (int rankValue = static_cast<int>(CardRank::Bitcoin); rankValue >= static_cast<int>(CardRank::Two); --rankValue) {
        if (rankCounts.contains(rankValue) && rankCounts[rankValue] == 2) {
            for (const Card& card : sortedCards) {
                if (card.getValue() == rankValue) {
                    pairs.append(card);
                }
            }
            pairCount++;
        } else if (rankCounts.contains(rankValue) && rankCounts[rankValue] == 1) {
            for (const Card& card : sortedCards) {
                if (card.getValue() == rankValue) {
                    kickers.append(card);
                }
            }
        }
    }

    if (pairCount == 2 && kickers.size() == 1) {
        result.relevantCards = pairs;
        result.kickerCards = kickers;
        result.highestCardValue = pairs[0].getValue();
        return true;
    }
    return false;
}

bool PofferRankEvaluator::isSinglePairHand(const QList<Card>& sortedCards, HandRankResult& result) {
    if (sortedCards.size() != 5) return false;

    QList<Card> pairCards;
    QList<Card> kickerCards;
    QMap<int, int> rankCounts;

    for (const Card& card : sortedCards) {
        rankCounts[card.getValue()]++;
    }

    int pairRankValue = -1;
    for (int rankValue = static_cast<int>(CardRank::Bitcoin); rankValue >= static_cast<int>(CardRank::Two); --rankValue) {
        if (rankCounts.contains(rankValue) && rankCounts[rankValue] == 2) {
            pairRankValue = rankValue;
            break;
        }
    }

    if (pairRankValue != -1) {
        for (const Card& card : sortedCards) {
            if (card.getValue() == pairRankValue) {
                pairCards.append(card);
            } else {
                kickerCards.append(card);
            }
        }
        if (pairCards.size() == 2 && kickerCards.size() == 3) {
            result.relevantCards = pairCards;
            std::sort(kickerCards.begin(), kickerCards.end(), compareCardsForRanking);
            result.kickerCards = kickerCards;
            result.highestCardValue = pairCards[0].getValue();
            return true;
        }
    }
    return false;
}

bool PofferRankEvaluator::isMessyHand(const QList<Card>& sortedCards, HandRankResult& result) {
    if (sortedCards.size() != 5) return false;

    result.relevantCards = sortedCards;
    result.highestCardValue = sortedCards[0].getValue();
    return true;
}
