#ifndef FLASHPOINT_FILE_H
#define FLASHPOINT_FILE_H

#include "Utf8String.h"
#include "List.h"
#include "Path.h"

class File
{
public:
    struct FileRemoveException : public std::exception
    {
        explicit FileRemoveException() noexcept;
    };

    static
    Utf8String
    read(const Path& path);

    static
    void
    write(const Path& path, const Utf8String& content);

    static
    void
    remove(const Path& path);

    static
    void
    createDirectory(const Path& path);
};

struct FileReadError : std::exception
{

};
#endif //FLASHPOINT_FILE_H
