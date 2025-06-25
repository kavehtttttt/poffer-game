#ifndef GAMEHISTORYENTRY_H
#define GAMEHISTORYENTRY_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <QList>
#include <QJsonValue>

class GameHistoryEntry
{
public:
    QString opponentUsername;
    QDateTime dateOfPlay;
    QList<QString> roundResults;
    QString finalResult;


    QJsonObject toJson() const;


    static GameHistoryEntry fromJson(const QJsonObject& obj);
};

#endif // GAMEHISTORYENTRY_H
