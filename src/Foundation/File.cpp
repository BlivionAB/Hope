#include "File.h"
#include <glob.h>
#include <fstream>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>


File::FileRemoveException::FileRemoveException() noexcept
    : std::exception()
{ }

Utf8String
File::read(const Path& path)
{
    std::ifstream inputFileStream;
    inputFileStream.open(path.toString().toString());

    std::string result(
        (std::istreambuf_iterator<char>(inputFileStream)),
        std::istreambuf_iterator<char>());
    const char* text = result.c_str();
    return Utf8String(text);
}

void
File::write(const Path& path, const Utf8String& content)
{
    std::ofstream outputFileStream;
    outputFileStream.open(path.toString().toString());
    outputFileStream << content.toString();
    outputFileStream.close();
}

void
File::remove(const Path& path)
{
    if (std::remove(path.toString().toString()) != 0)
    {
        throw FileRemoveException();
    }
}

void
File::createDirectory(const Path& path)
{
    create_directories(boost::filesystem::path(path.toString().toString()));
}