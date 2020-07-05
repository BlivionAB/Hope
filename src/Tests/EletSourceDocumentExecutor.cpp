#include "EletSourceDocumentExecutor.h"
#include <Domain/Compiler.h>

void
EletSourceDocumentExecutor::execute(const Path &file) const
{
    Compiler compiler;
    compiler.startWorkers();
    compiler.parseStandardLibrary();
    compiler.parseFile(file.toString().asString());
    compiler.endWorkers();
}
