#ifndef ELET_FILEREADER_H
#define ELET_FILEREADER_H


#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;


namespace elet::foundation
{


class FileStreamReader
{

public:

    virtual
    void
    openFile(const fs::path string);

    virtual
    size_t
    readChunk(char* readCursor, size_t size);

    virtual
    size_t
    getFileSize();

    void
    close();

private:

    std::ifstream
    _ifstream;
};


}


#endif //ELET_FILEREADER_H
