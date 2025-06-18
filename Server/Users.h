#pragma once
#include"User.h"
#include<iostream>
#include <QObject>
#include<QMap>
#include<QJsonParseError>
#include<QJsonDocument>
#include<QJsonObject>

class Users :
    public QObject
{
    Q_OBJECT
private:
    QMap<QString, User*>* Players;

public:
    Users(QObject* parent=nullptr);
    ~Users();
    bool signin(const QJsonObject& user);
    bool login(const QJsonObject& user);
    bool Is_User(const QString& userN);
    void read_from_file(const QString file = "UsersInfo.txt");
    void addToMap(User* user);
    void Save_to_file(const QString file = "UsersInfo.txt");
};

