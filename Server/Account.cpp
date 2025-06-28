#include "Account.h"
#include <QDebug>
#include <QJsonArray>
#include "GameHistoryEntry.h"

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
        throw UserException("Somebody used this username before!!!");
    }
}

QJsonObject Account::login(const QJsonObject& data)
{
    if (usersRef->login(data)) {
        return QJsonObject{
            {"type", "login"},
            {"status", "success"},
            {"message", "Login succesfully"}
        };
    }
    else {
        throw UserException("Wrong username or password!!!");
    }
}

QJsonObject Account::forgetpass(const QJsonObject& data) {
    try {
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
    } catch (const UserException& ex) {
        throw ex;
    }
}

QJsonObject Account::logout(const QJsonObject& data) {
    QString username = data["username"].toString();
    if (username.isEmpty()) {
        throw UserException("Username cannot be empty for logout!");
    }
    if (!usersRef->Is_User(username)) {
        throw UserException("User not found for logout!");
    }
    return QJsonObject{
        {"type", "logout"},
        {"status", "success"},
        {"message", "Logged out successfully"}
    };
}

QJsonObject Account::editUsername(const QJsonObject& data) {
    QString oldUsername = data["username"].toString();
    QString newUsername = data["new_username"].toString();

    if (oldUsername.isEmpty() || newUsername.isEmpty()) {
        throw UserException("Username fields cannot be empty!");
    }
    if (oldUsername == newUsername) {
        throw UserException("New username is the same as old username!");
    }

    try {
        if (usersRef->updateUsername(oldUsername, newUsername)) {
            return QJsonObject{
                {"type", "Edit_username"},
                {"status", "success"},
                {"message", "Username updated successfully"}
            };
        } else {
            throw UserException("Failed to update username.");
        }
    } catch (const UserException& ex) {
        throw ex;
    }
}

QJsonObject Account::editPassword(const QJsonObject& data) {
    QString username = data["username"].toString();
    QString oldPassword = data["old_password"].toString();
    QString newPassword = data["new_password"].toString();

    if (username.isEmpty() || oldPassword.isEmpty() || newPassword.isEmpty()) {
        throw UserException("Password fields cannot be empty!");
    }

    if (!usersRef->login(QJsonObject{{"username", username}, {"password", oldPassword}})) {
        throw UserException("Incorrect old password!");
    }

    try {
        if (usersRef->updatePassword(username, newPassword)) {
            return QJsonObject{
                {"type", "Edit_password"},
                {"status", "success"},
                {"message", "Password updated successfully"}
            };
        } else {
            throw UserException("Failed to update password.");
        }
    } catch (const UserException& ex) {
        throw ex;
    }
}

QJsonObject Account::editEmail(const QJsonObject& data) {
    QString username = data["username"].toString();
    QString newEmail = data["new_email"].toString();

    if (username.isEmpty() || newEmail.isEmpty()) {
        throw UserException("Email fields cannot be empty!");
    }

    try {
        if (usersRef->updateEmail(username, newEmail)) {
            return QJsonObject{
                {"type", "Edit_email"},
                {"status", "success"},
                {"message", "Email updated successfully"}
            };
        } else {
            throw UserException("Failed to update email.");
        }
    } catch (const UserException& ex) {
        throw ex;
    }
}

QJsonObject Account::editPhone(const QJsonObject& data) {
    QString username = data["username"].toString();
    QString newPhone = data["new_phone"].toString();

    if (username.isEmpty() || newPhone.isEmpty()) {
        throw UserException("Phone fields cannot be empty!");
    }

    try {
        if (usersRef->updatePhone(username, newPhone)) {
            return QJsonObject{
                {"type", "Edit_Phone"},
                {"status", "success"},
                {"message", "Phone number updated successfully"}
            };
        } else {
            throw UserException("Failed to update phone number.");
        }
    } catch (const UserException& ex) {
        throw ex;
    }
}

QJsonObject Account::editFirstName(const QJsonObject& data) {
    QString username = data["username"].toString();
    QString newFirstName = data["new_first_name"].toString();

    if (username.isEmpty() || newFirstName.isEmpty()) {
        throw UserException("First Name fields cannot be empty!");
    }

    try {
        if (usersRef->updateFirstName(username, newFirstName)) {
            return QJsonObject{
                {"type", "Edit_fName"},
                {"status", "success"},
                {"message", "First name updated successfully"}
            };
        } else {
            throw UserException("Failed to update first name.");
        }
    } catch (const UserException& ex) {
        throw ex;
    }
}

QJsonObject Account::editLastName(const QJsonObject& data) {
    QString username = data["username"].toString();
    QString newLastName = data["new_last_name"].toString();

    if (username.isEmpty() || newLastName.isEmpty()) {
        throw UserException("Last Name fields cannot be empty!");
    }

    try {
        if (usersRef->updateLastName(username, newLastName)) {
            return QJsonObject{
                {"type", "Edit_Lname"},
                {"status", "success"},
                {"message", "Last name updated successfully"}
            };
        } else {
            throw UserException("Failed to update last name.");
        }
    } catch (const UserException& ex) {
        throw ex;
    }
}

QJsonObject Account::getGameHistory(const QJsonObject& data) {
    QString username = data["username"].toString();
    if (username.isEmpty()) {
        throw UserException("Username cannot be empty for getting history!");
    }

    try {
        QJsonArray historyArray = usersRef->getGameHistoryJson(username);
        return QJsonObject{
            {"type", "Get_History"},
            {"status", "success"},
            {"username", username},
            {"history", historyArray}
        };
    } catch (const UserException& ex) {
        throw ex;
    }
}

QJsonObject Account::addGameHistory(const QJsonObject& data) {
    QString username = data["username"].toString();
    if (username.isEmpty()) {
        throw UserException("Username cannot be empty for adding game history!");
    }

    GameHistoryEntry entry;
    entry.opponentUsername = data["opponent_username"].toString();
    entry.dateOfPlay = QDateTime::fromString(data["date_of_play"].toString(), Qt::ISODate);
    if (!entry.dateOfPlay.isValid()) {
        entry.dateOfPlay = QDateTime::currentDateTime();
    }

    if (data.contains("round_results") && data["round_results"].isArray()) {
        QJsonArray roundsArray = data["round_results"].toArray();
        for(const QJsonValue& val : roundsArray) {
            entry.roundResults.append(val.toString());
        }
    } else {
        throw UserException("Missing or invalid 'round_results' for game history!");
    }

    entry.finalResult = data["final_result"].toString();

    if (entry.opponentUsername.isEmpty() || entry.finalResult.isEmpty()) {
        throw UserException("Missing opponent username or final result for game history!");
    }

    try {
        if (usersRef->addGameHistory(username, entry)) {
            return QJsonObject{
                {"type", "Add_History"},
                {"status", "success"},
                {"message", "Game history added successfully"}
            };
        } else {
            throw UserException("Failed to add game history.");
        }
    } catch (const UserException& ex) {
        throw ex;
    }
}
