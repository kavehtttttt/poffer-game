#include "User.h"
#include <QDebug> // برای qDbug

// --- Constructor ---
User::User() = default;

User::User(const QString& username,
           const QString& password,
           const QString& firstName,
           const QString& lastName,
           const QString& phoneNumber,
           const QString& email)
    : username(username),
    firstName(firstName),
    lastName(lastName),
    phoneNumber(phoneNumber),
    email(email)
{
    hashedPassword = hashPassword(password);
}

// --- Getters ---
QString User::getUsername() const { return username; }
QString User::getFirstName() const { return firstName; }
QString User::getLastName() const { return lastName; }
QString User::getPhoneNumber() const { return phoneNumber; }
QString User::getEmail() const { return email; }
QString User::getHashedPassword() const { return hashedPassword; }

// --- Setters ---
void User::setFirstName(const QString& value) { firstName = value; }
void User::setLastName(const QString& value) { lastName = value; }
void User::setPhoneNumber(const QString& value) { phoneNumber = value; }
void User::setEmail(const QString& value) { email = value; }

void User::setPassword(const QString& plainPassword) {
    hashedPassword = hashPassword(plainPassword);
}

// --- Hashing ---
QString User::hashPassword(const QString& password) {
    return QString(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
}

bool User::checkPassword(const QString& inputPassword) const {
    return hashedPassword == hashPassword(inputPassword);
}

// --- JSON Conversion ---
QJsonObject User::toJson() const {
    QJsonObject obj;
    obj["username"] = username;
    obj["hashed_password"] = hashedPassword;
    obj["first_name"] = firstName;
    obj["last_name"] = lastName;
    obj["phone_number"] = phoneNumber;
    obj["email"] = email;
    return obj;
}


User User::fromJson(const QJsonObject& obj, bool hashIfNeeded) {
    User u;
    qDebug() << "User::fromJson - Input JSON object for username:" << obj["username"];
    qDebug() << "User::fromJson - Extracted username string:" << obj["username"].toString();

    u.username = obj["username"].toString();
    u.firstName = obj["first_name"].toString();
    u.lastName = obj["last_name"].toString();
    u.phoneNumber = obj["phone_number"].toString();
    u.email = obj["email"].toString();

    if (hashIfNeeded) {
        u.hashedPassword = hashPassword(obj["password"].toString());
    }
    else {
        u.hashedPassword = obj["hashed_password"].toString();
    }
    qDebug() << "User::fromJson - Before returning, username in 'u':" << u.getUsername();
    return u;
}
