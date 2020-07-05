#ifndef ELET_DOCUMENTEXECUTOR_H
#define ELET_DOCUMENTEXECUTOR_H

#include "Utf8String.h"
#include "Path.h"

class DocumentExecutor
{

public:

    void virtual execute(const Path& file) const = 0;
};


#endif //ELET_DOCUMENTEXECUTOR_H
