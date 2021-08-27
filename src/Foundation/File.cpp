#include "File.h"
#include <fstream>
#include <filesystem>


namespace fs = std::filesystem;

namespace elet::foundation
{


Utf8String
File::read(const fs::path& path)
{
    std::ifstream inputFileStream;
    inputFileStream.open(path.string().c_str());

    std::string result(
        (std::istreambuf_iterator<char>(inputFileStream)),
        std::istreambuf_iterator<char>());
    const char* text = result.c_str();
    inputFileStream.close();
    return Utf8String(text);
}


void
File::write(const fs::path& path, const Utf8String& content)
{
    std::ofstream outputFileStream;
    outputFileStream.open(path.string().c_str());
    outputFileStream << content.toString();
    outputFileStream.close();
}


}
