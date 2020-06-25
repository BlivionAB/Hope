#ifndef ELET_FILEREADER_H
#define ELET_FILEREADER_H

#include "Path.h"

class FileReader
{
    std::vector<Path>
    find(const Path& path) const;
};

#endif //ELET_FILEREADER_H
