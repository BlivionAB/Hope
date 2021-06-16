#ifndef ELET_FILEREADER_H
#define ELET_FILEREADER_H


#include "./FilePath.h"


namespace elet::foundation
{


class FileReader
{

public:

    virtual
    int
    openFile(const FilePath& filepath);

    virtual
    size_t
    readChunk(int fd, char* readCursor, size_t size);

    virtual
    size_t
    getFileSize(int fd);
};


}


#endif //ELET_FILEREADER_H
