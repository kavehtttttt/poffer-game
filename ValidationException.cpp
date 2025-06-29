#include "ValidationException.h"

ValidationException::ValidationException(const QString &msg)
    : message(msg)
{
}

const char* ValidationException::what() const noexcept
{
    return message.toUtf8().constData();
}
