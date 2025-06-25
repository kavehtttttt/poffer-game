#include "Users.h"
#include <QPair>
#include <QStandardPaths>
#include <QDebug>
#include <QJsonArray>
#include "GameHistoryEntry.h" // <-- Include the new header

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
    if (!Is_User(username))
        throw UserException("Username not found!");
    User user = Players->value(username);
    if (user.getPhoneNumber()!=phone)
        return false;

    user.setPassword(npass);
    (*Players)[username] = user;
    Save_to_file();
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

bool Users::updateUsername(const QString& oldUsername, const QString& newUsername) {
    if (!Is_User(oldUsername))
        throw UserException("Old username not found!");
    if (Is_User(newUsername))
        throw UserException("New username already exists!");

    User user = Players->value(oldUsername);
    Players->remove(oldUsername);
    user.setUsername(newUsername);
    Players->insert(newUsername, user);
    Save_to_file();
    return true;
}

bool Users::updatePassword(const QString& username, const QString& newPlainPassword) {
    if (!Is_User(username))
        throw UserException("Username not found!");
    User user = Players->value(username);
    user.setPassword(newPlainPassword);
    (*Players)[username] = user;
    Save_to_file();
    return true;
}

bool Users::updateEmail(const QString& username, const QString& newEmail) {
    if (!Is_User(username))
        throw UserException("Username not found!");
    User user = Players->value(username);
    user.setEmail(newEmail);
    (*Players)[username] = user;
    Save_to_file();
    return true;
}

bool Users::updatePhone(const QString& username, const QString& newPhone) {
    if (!Is_User(username))
        throw UserException("Username not found!");
    User user = Players->value(username);
    user.setPhoneNumber(newPhone);
    (*Players)[username] = user;
    Save_to_file();
    return true;
}

bool Users::updateFirstName(const QString& username, const QString& newFirstName) {
    if (!Is_User(username))
        throw UserException("Username not found!");
    User user = Players->value(username);
    user.setFirstName(newFirstName);
    (*Players)[username] = user;
    Save_to_file();
    return true;
}

bool Users::updateLastName(const QString& username, const QString& newLastName) {
    if (!Is_User(username))
        throw UserException("Username not found!");
    User user = Players->value(username);
    user.setLastName(newLastName);
    (*Players)[username] = user;
    Save_to_file();
    return true;
}


bool Users::addGameHistory(const QString& username, const GameHistoryEntry& entry) {
    if (!Is_User(username)) {
        throw UserException("User not found to add game history!");
    }
    User user = Players->value(username);
    user.addGameHistory(entry);
    (*Players)[username] = user;
    Save_to_file();
    return true;
}

QJsonArray Users::getGameHistoryJson(const QString& username) {
    if (!Is_User(username)) {
        throw UserException("User not found to retrieve game history!");
    }
    User user = Players->value(username);
    QJsonArray historyArray;
    for (const GameHistoryEntry& entry : user.getGameHistory()) {
        historyArray.append(entry.toJson());
    }
    return historyArray;
}
