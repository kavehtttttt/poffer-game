#ifndef GAMEEXCEPTION_H
#define GAMEEXCEPTION_H

#include <exception>
#include <QString>

class GameException : public std::exception
{
private:
    QString message;

public:
    explicit GameException(const QString &msg);
    const char* what() const noexcept override;
};

#endif // GAMEEXCEPTION_H
