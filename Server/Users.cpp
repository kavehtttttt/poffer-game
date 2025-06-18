#include "Users.h"
#include<QPair>
Users::Users(QObject* parent):QObject(parent) {

}
Users::~Users() {

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
bool Users::login(const QJsonObject& user) {
	/*if (!Is_User(user["username"].toString()))
		return false;
	const QString u = user["username"].toString();*/
}
bool Users::Is_User(const QString& userN) {
	return Players->contains(userN);
}
void Users::addToMap(User* user) {
	Players->insert(user->getUsername(), user);
	Save_to_file();
}
void Users::read_from_file(const QString file = "UsersInfo.txt") {

}
void Users::Save_to_file(const QString file = "UsersInfo.txt") {

}