#include "FileReader.h"
#include <fstream>
#include <fcntl.h>
#include <sys/stat.h>

namespace elet::foundation
{


void
FileStreamReader::openFile(const fs::path path)
{
    _ifstream.open(path.string().c_str(), std::ifstream::in);
}


size_t
FileStreamReader::readChunk(char* readCursor, size_t size)
{
    return _ifstream.readsome(readCursor, size);
}


size_t
FileStreamReader::getFileSize()
{
    return _ifstream.tellg();
}


void
FileStreamReader::close()
{
    _ifstream.close();
}


}