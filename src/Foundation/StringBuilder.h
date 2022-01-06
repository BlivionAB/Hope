#ifndef ELET_STRINGBUILDER_H
#define ELET_STRINGBUILDER_H


#include "Utf8String.h"


namespace elet::foundation
{
    class StringBuilder
    {
    public:

        StringBuilder();

        StringBuilder(std::string string);

        void
        append(std::string string);

        std::string
        toString();

    private:

        std::string
        _string;
    };

    void
    replaceAll(std::string& str, const std::string& from, const std::string& to);
}


#endif //ELET_STRINGBUILDER_H
