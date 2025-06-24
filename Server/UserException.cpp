#include "UserException.h"
UserException::UserException(const QString& message)
    : std::runtime_error(message.toStdString()) {}

LoginException::LoginException(const QString& msg) : UserException(msg) {}


SignupException::SignupException(const QString& msg) : UserException(msg) {}
