#include "EletSourceDocumentExecutor.h"

void
EletSourceDocumentExecutor::execute(const Path &entryFile, const Path& outputFile, const BaselineTestSelection& selection) const
{
    static const List<AssemblyTarget> targets = { AssemblyTarget::Baseline, AssemblyTarget::x86_64 };
    for (const auto& target : targets)
    {
        if (selection.assembly == AssemblyTarget::Unknown || selection.assembly == target)
        {
            compileFileWithTarget(entryFile, outputFile, target, ObjectFileTarget::MachO);
        }
    }
}


void
EletSourceDocumentExecutor::compileFileWithTarget(const Path& entryFile, const Path& outputFile, AssemblyTarget assemblyTarget, ObjectFileTarget objectFileTarget) const
{
    Compiler compiler(assemblyTarget, objectFileTarget);
    compiler.startWorkers();
    compiler.compileFile(entryFile, outputFile);
    compiler.endWorkers();
}
