#include "Users.h"
#include<QPair>
Users::Users(QObject* parent) : QObject(parent) {
	Players = new QMap<QString, User*>();
	read_from_file(); 
}

Users::~Users() {
	qDeleteAll(*Players);
	delete Players;
}
bool Users::signin(const QJsonObject& user) {
	if (!Is_User(user["username"].toString())) {
		User* u = new User;
		u->fromJson(user);
		addToMap(u);
		return true;
	}
	else
		return false;
}
bool Users::login(const QJsonObject& userJson) {
	QString username = userJson["username"].toString();
	QString password = userJson["password"].toString();

	if (!Is_User(username))
		return false;

	User* user = Players->value(username);
	return user->checkPassword(password);
}

bool Users::forgetpass(const QJsonObject& userJson) {
	QString username = userJson["username"].toString();
	QString phone = userJson["phoneNumber"].toString();
	QString npass = userJson["newPassword"].toString();
	User* user = Players->value(username);
	if (!Is_User(username)|| user->getPhoneNumber()!=phone)
		return false;

	user->setPassword(npass);
	return true;
}

bool Users::Is_User(const QString& userN) {
	return Players->contains(userN);
}
void Users::addToMap(User* user) {
	Players->insert(user->getUsername(), user);
	Save_to_file();
}
void Users::read_from_file(const QString file) {
    QFile f(file);
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open user file:" << file;
        return;
    }

    QByteArray data = f.readAll();
    f.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonArray array = doc.array();

    for (const QJsonValue& val : array) {
        QJsonObject obj = val.toObject();
        User* u = User::fromJson(obj, false);
 
        Players->insert(u->getUsername(), u);
    }
}

void Users::Save_to_file(const QString file) {
    QFile f(file);
    if (!f.open(QIODevice::WriteOnly)) {
        qWarning() << "Cannot write user file:" << file;
        return;
    }

    QJsonArray array;
    for (User* user : Players->values()) {
        array.append(user->toJson());
    }

    QJsonDocument doc(array);
    f.write(doc.toJson(QJsonDocument::Indented));
    f.close();
}