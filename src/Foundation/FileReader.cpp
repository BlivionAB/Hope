#include "FileReader.h"
#include <fstream>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

namespace elet::foundation
{


int
FileReader::openFile(const FilePath& string)
{
    return open(string.toString().asString(), O_RDONLY);
}


size_t
FileReader::readChunk(int fd, char* readCursor, size_t size)
{
    return read(fd, readCursor, size);
}


size_t
FileReader::getFileSize(int fd)
{
    struct stat sb;
    fstat(fd, &sb);
    return sb.st_size;
}


}