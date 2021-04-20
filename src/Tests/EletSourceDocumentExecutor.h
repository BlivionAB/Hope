#ifndef ELET_ELETSOURCEDOCUMENTEXECUTOR_H
#define ELET_ELETSOURCEDOCUMENTEXECUTOR_H

#include <Foundation/DocumentExecutor.h>
#include <Foundation/Utf8String.h>
#include <Foundation/Path.h>
#include <Domain/Compiler/Compiler.h>
#include <Domain/CommandLine/BaselineTestCommand.h>

using namespace elet::domain::compiler;

class EletSourceDocumentExecutor: public DocumentExecutor<BaselineTestSelection>
{
    void
    execute(const Path &entryFile, const Path& outputFile, const BaselineTestSelection& selection) const;

    void
    compileFileWithTarget(const Path& entryFile, const Path& outputFile, AssemblyTarget target, ObjectFileTarget objectFileTarget) const;
};


#endif //ELET_ELETSOURCEDOCUMENTEXECUTOR_H
