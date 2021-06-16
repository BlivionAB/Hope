#include "File.h"
#include <glob.h>
#include <fstream>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>


File::FileRemoveException::FileRemoveException() noexcept
    : std::exception()
{ }

Utf8String
File::read(const FilePath& path)
{
    std::ifstream inputFileStream;
    inputFileStream.open(path.toString().toString());

    std::string result(
        (std::istreambuf_iterator<char>(inputFileStream)),
        std::istreambuf_iterator<char>());
    const char* text = result.c_str();
    inputFileStream.close();
    return Utf8String(text);
}

void
File::write(const FilePath& path, const Utf8String& content)
{
    std::ofstream outputFileStream;
    outputFileStream.open(path.toString().toString());
    outputFileStream << content.toString();
    outputFileStream.close();
}

void
File::remove(const FilePath& path)
{
    if (std::remove(path.toString().toString()) != 0)
    {
        throw FileRemoveException();
    }
}

void
File::createDirectory(const FilePath& path)
{
    create_directories(boost::filesystem::path(path.toString().toString()));
}
