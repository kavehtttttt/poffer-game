#pragma once
#include"User.h"
#include <QObject>
#include<QMap>
#include<QJsonParseError>
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonArray>
#include<QFile>
#include"UserException.h"
#include "GameHistoryEntry.h"

class Users :
              public QObject
{
    Q_OBJECT
private:
    QMap<QString, User>* Players;

public:
    Users(QObject* parent=nullptr);
    ~Users();
    bool signin(const QJsonObject& user);
    bool login(const QJsonObject& user);
    bool forgetpass(const QJsonObject& user);
    bool Is_User(const QString& userN);
    void read_from_file(const QString file = "UsersInfo.txt");
    void addToMap(const User& user);
    void Save_to_file(const QString file = "UsersInfo.txt");

    bool updateUsername(const QString& oldUsername, const QString& newUsername);
    bool updatePassword(const QString& username, const QString& newPlainPassword);
    bool updateEmail(const QString& username, const QString& newEmail);
    bool updatePhone(const QString& username, const QString& newPhone);
    bool updateFirstName(const QString& username, const QString& newFirstName);
    bool updateLastName(const QString& username, const QString& newLastName);

    // Methods for Game History
    bool addGameHistory(const QString& username, const GameHistoryEntry& entry);
    QJsonArray getGameHistoryJson(const QString& username);
};
