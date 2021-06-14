#include <fstream>
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
    auto output = compiler.getOutput();

    std::ofstream file;
    const char* path = Path::resolve(Path::cwd(), "cmake-build-debug/test.o").toString().toString();
    file.open(path, std::ios_base::binary);
    for (int i = 0; i < output.size(); ++i)
    {
        file.write(reinterpret_cast<char*>(&output[i]), 1);
    }
    std::cout << path << std::endl;
    file.close();
}
