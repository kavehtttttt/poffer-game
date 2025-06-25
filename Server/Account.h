#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QObject>
#include <QJsonObject>
#include "Users.h"
#include "UserException.h"
#include "GameHistoryEntry.h"
class Account : public QObject
{
    Q_OBJECT
public:
    explicit Account(Users* users, QObject* parent = nullptr);
    QJsonObject login(const QJsonObject& data);
    QJsonObject signup(const QJsonObject& data);
    QJsonObject forgetpass(const QJsonObject& data);

    QJsonObject logout(const QJsonObject& data);
    QJsonObject editUsername(const QJsonObject& data);
    QJsonObject editPassword(const QJsonObject& data);
    QJsonObject editEmail(const QJsonObject& data);
    QJsonObject editPhone(const QJsonObject& data);
    QJsonObject editFirstName(const QJsonObject& data);
    QJsonObject editLastName(const QJsonObject& data);

    // New methods for Game History
    QJsonObject getGameHistory(const QJsonObject& data);
    QJsonObject addGameHistory(const QJsonObject& data);

private:
    Users* usersRef;
};

#endif // ACCOUNT_H
