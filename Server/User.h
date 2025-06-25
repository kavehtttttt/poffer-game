#pragma once
#include <QString>
#include <QJsonObject>
#include <QCryptographicHash>
#include <QList>
#include <QDateTime>
#include <QJsonArray>

#include "GameHistoryEntry.h"

class User
{
public:
    User();
    User(const QString& username,
         const QString& password,
         const QString& firstName,
         const QString& lastName,
         const QString& phoneNumber,
         const QString& email);

    QString getUsername() const;
    QString getFirstName() const;
    QString getLastName() const;
    QString getPhoneNumber() const;
    QString getEmail() const;
    QString getHashedPassword() const;

    void setFirstName(const QString& value);
    void setLastName(const QString& value);
    void setPhoneNumber(const QString& value);
    void setEmail(const QString& value);
    void setPassword(const QString& plainPassword);
    void setUsername(const QString& value);

    bool checkPassword(const QString& inputPassword) const;

    QJsonObject toJson() const;
    static User fromJson(const QJsonObject& obj, bool hashIfNeeded = true);

    static QString hashPassword(const QString& password);

    // Methods for Game History
    void addGameHistory(const GameHistoryEntry& entry);
    QList<GameHistoryEntry> getGameHistory() const;

private:
    QString username;
    QString hashedPassword;
    QString firstName;
    QString lastName;
    QString phoneNumber;
    QString email;
    QList<GameHistoryEntry> gameHistory;
};
