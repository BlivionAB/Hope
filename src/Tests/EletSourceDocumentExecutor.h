#ifndef ELET_ELETSOURCEDOCUMENTEXECUTOR_H
#define ELET_ELETSOURCEDOCUMENTEXECUTOR_H

#include <Foundation/DocumentExecutor.h>
#include <Foundation/Utf8String.h>
#include <Foundation/Path.h>

class EletSourceDocumentExecutor: public DocumentExecutor
{
    void
    execute(const Path &file) const;
};


#endif //ELET_ELETSOURCEDOCUMENTEXECUTOR_H
