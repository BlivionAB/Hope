#ifndef FLASHPOINT_FILE_H
#define FLASHPOINT_FILE_H

#include "Utf8String.h"
#include "List.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace elet::foundation
{


class File
{
public:

    static
    Utf8String
    read(const fs::path& path);

    static
    void
    write(const fs::path& path, const Utf8String& content);

};

struct FileReadError : std::exception
{

};


}
#endif //FLASHPOINT_FILE_H
