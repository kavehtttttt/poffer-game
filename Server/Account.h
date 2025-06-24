#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QObject>
#include <QJsonObject>
#include "Users.h"
#include "UserException.h"

class Account : public QObject
{
    Q_OBJECT
public:
    explicit Account(Users* users, QObject* parent = nullptr);
    QJsonObject login(const QJsonObject& data);
    QJsonObject signup(const QJsonObject& data);
    QJsonObject forgetpass(const QJsonObject& data);

private:
    Users* usersRef;
};

#endif // ACCOUNT_H
