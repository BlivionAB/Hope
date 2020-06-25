#ifndef ELET_DOCUMENTEXECUTOR_H
#define ELET_DOCUMENTEXECUTOR_H

#include "Utf8String.h"

template<typename TPath, typename TFile>
class DocumentExecutor
{

public:

    DocumentExecutor(TPath& path, TFile& file);

    Utf8String
    execute(const TPath& file) const;

private:

    TFile
    _file;

    TPath
    _path;
};


#endif //ELET_DOCUMENTEXECUTOR_H
