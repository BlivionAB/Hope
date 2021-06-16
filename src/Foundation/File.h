#ifndef FLASHPOINT_FILE_H
#define FLASHPOINT_FILE_H

#include "Utf8String.h"
#include "List.h"
#include "FilePath.h"

namespace elet::foundation
{


class File
{
public:
    struct FileRemoveException : public std::exception
    {
        explicit FileRemoveException() noexcept;
    };

    static
    Utf8String
    read(const FilePath& path);

    static
    void
    write(const FilePath& path, const Utf8String& content);

    static
    void
    remove(const FilePath& path);

    static
    void
    createDirectory(const FilePath& path);
};

struct FileReadError : std::exception
{

};


}
#endif //FLASHPOINT_FILE_H
