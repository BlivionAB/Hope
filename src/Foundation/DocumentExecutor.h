#ifndef ELET_DOCUMENTEXECUTOR_H
#define ELET_DOCUMENTEXECUTOR_H

#include "Utf8String.h"
#include "Path.h"

template<typename TTestSelection>
class DocumentExecutor
{

public:

    void virtual execute(const Path& entryFile, const Path& outputFile, const TTestSelection& selection) const = 0;
};


#endif //ELET_DOCUMENTEXECUTOR_H
