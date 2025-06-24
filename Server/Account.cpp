#include "Account.h"

Account::Account(Users* users, QObject* parent)
    : QObject(parent), usersRef(users) {
}

QJsonObject Account::signup(const QJsonObject& data)
{
    if (usersRef->signin(data)) {
        return QJsonObject{
            {"type", "signup"},
            {"status", "success"},
            {"message", "Signup succesfully"}
        };
    }
    else {
        throw UserException("Sombody used this username befor!!!");
    }
}

QJsonObject Account::login(const QJsonObject& data)
{
    if (usersRef->login(data)) {
        return QJsonObject{
            {"type", "login"},
            {"status", "success"},
            {"message", "login succesfully"}
        };
    }
    else {
        throw UserException("Wrong username or password!!!");
    }
}

QJsonObject Account::forgetpass(const QJsonObject& data) {
    if (usersRef->forgetpass(data)) {
        return QJsonObject{
            {"type", "forgetPassword"},
            {"status", "success"},
            {"message", "Password changed successfully"}
        };
    }
    else {
        throw UserException("Wrong username or phoneNumber!!!");
    }
}
