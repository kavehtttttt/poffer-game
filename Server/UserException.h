#ifndef USEREXCEPTION_H
#define USEREXCEPTION_H

#include <stdexcept>
#include <QString>

class UserException : public std::runtime_error {
public:
    explicit UserException(const QString& message);
};

class LoginException : public UserException {
public:
    explicit LoginException(const QString& msg);
};

class SignupException : public UserException {
public:
    explicit SignupException(const QString& msg);
};


#endif // USEREXCEPTION_H
