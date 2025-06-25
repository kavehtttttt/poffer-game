#include "GameHistoryEntry.h"


QJsonObject GameHistoryEntry::toJson() const {
    QJsonObject obj;
    obj["opponent_username"] = opponentUsername;
    obj["date_of_play"] = dateOfPlay.toString(Qt::ISODate);

    QJsonArray roundsArray;
    for (const QString& result : roundResults) {
        roundsArray.append(result);
    }
    obj["round_results"] = roundsArray;
    obj["final_result"] = finalResult;
    return obj;
}


GameHistoryEntry GameHistoryEntry::fromJson(const QJsonObject& obj) {
    GameHistoryEntry entry;
    entry.opponentUsername = obj["opponent_username"].toString();
    entry.dateOfPlay = QDateTime::fromString(obj["date_of_play"].toString(), Qt::ISODate);

    QJsonArray roundsArray = obj["round_results"].toArray();
    for (const QJsonValue& val : roundsArray) {
        entry.roundResults.append(val.toString());
    }
    entry.finalResult = obj["final_result"].toString();
    return entry;
}
