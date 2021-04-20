#ifndef ELET_EXCEPTIONS_H
#define ELET_EXCEPTIONS_H

#include "Scanner.h"

using Token = Scanner::Token;

struct UnexpectedToken
{
    Utf8String
    message;

    Token
    token;

    UnexpectedToken(Utf8String _message, Token _token):
        message(_message),
        token(_token)
    { }
};

#endif //ELET_EXCEPTIONS_H
