#ifndef VALIDATIONEXCEPTION_H
#define VALIDATIONEXCEPTION_H

#include <exception>
#include <QString>

class ValidationException : public std::exception
{
private:
    QString message;

public:
    explicit ValidationException(const QString &msg);
    const char* what() const noexcept override;
};

#endif // VALIDATIONEXCEPTION_H
