#ifndef ELET_ELETSOURCEDOCUMENTEXECUTOR_H
#define ELET_ELETSOURCEDOCUMENTEXECUTOR_H

#include <Foundation/DocumentExecutor.h>
#include <Foundation/Utf8String.h>
#include <Foundation/FilePath.h>
#include <Domain/Compiler/Compiler.h>
#include <Domain/CommandLine/BaselineTestCommand.h>

using namespace elet::domain::compiler;

class EletSourceDocumentExecutor: public DocumentExecutor<BaselineTestSelection>
{
    void
    execute(const FilePath &entryFile, const FilePath& outputFile, const BaselineTestSelection& selection) const;

    void
    compileFileWithTarget(const FilePath& entryFile, const FilePath& outputFile, AssemblyTarget target, ObjectFileTarget objectFileTarget) const;
};


#endif //ELET_ELETSOURCEDOCUMENTEXECUTOR_H
