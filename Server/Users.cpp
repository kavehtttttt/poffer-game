#include "Users.h"
#include <QPair>
#include <QDebug>

Users::Users(QObject* parent) : QObject(parent) {
    Players = new QMap<QString, User>();
    read_from_file();
    qDebug() << "Users loaded from file. Current users count:" << Players->size();
    for (const QString& key : Players->keys()) {
        qDebug() << "Loaded user:" << key;
    }
}

Users::~Users() {
    Save_to_file();
    delete Players;
}

bool Users::signin(const QJsonObject& userJson) {
    QString username = userJson["username"].toString();
    qDebug() << "Attempting to sign in user:" << username;
    if (!Is_User(username)) {
        User u = User::fromJson(userJson);
        qDebug() << "Users::signin - After fromJson, username in 'u':" << u.getUsername();
        addToMap(u);
        return true;
    }
    else {
        qDebug() << "User already exists:" << username;
        return false;
    }
}

bool Users::login(const QJsonObject& userJson) {
    QString username = userJson["username"].toString();
    QString password = userJson["password"].toString();

    if (!Is_User(username))
        return false;

    User user = Players->value(username);
    return user.checkPassword(password);
}

bool Users::forgetpass(const QJsonObject& userJson) {
    QString username = userJson["username"].toString();
    QString phone = userJson["phoneNumber"].toString();
    QString npass = userJson["newPassword"].toString();
    User user = Players->value(username);
    if (!Is_User(username)|| user.getPhoneNumber()!=phone)
        return false;

    user.setPassword(npass);
    (*Players)[username] = user;
    return true;
}

bool Users::Is_User(const QString& userN) {
    return Players->contains(userN);
}

void Users::addToMap(const User& user) {
    Players->insert(user.getUsername(), user);
    qDebug() << "User added to map:" << user.getUsername() << ". Total users:" << Players->size();
    Save_to_file();
}

void Users::read_from_file(const QString file) {
    QFile f(file);
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open user file:" << file;
        Players->clear();
        return;
    }

    QByteArray data = f.readAll();
    f.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonArray array = doc.array();

    Players->clear();

    for (const QJsonValue& val : array) {
        QJsonObject obj = val.toObject();
        User u = User::fromJson(obj, false);
        if (!u.getUsername().isEmpty()) {
            qDebug() << "Read from file - User object username:" << u.getUsername();
            Players->insert(u.getUsername(), u);
        } else {
            qWarning() << "Failed to deserialize User from JSON object (username empty).";
        }
    }
}

void Users::Save_to_file(const QString file) {
    QFile f(file);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "Cannot write user file:" << file;
        return;
    }

    QJsonArray array;
    for (const User& user : Players->values()) {
        array.append(user.toJson());
    }

    QJsonDocument doc(array);
    f.write(doc.toJson(QJsonDocument::Indented));
    f.close();
}
