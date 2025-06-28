#include "GameException.h"

GameException::GameException(const QString &msg)
    : message(msg)
{
}

const char* GameException::what() const noexcept
{
    return message.toUtf8().constData();
}
