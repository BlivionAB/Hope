#ifndef ELET_TRANSFORMER_H
#define ELET_TRANSFORMER_H


#include "Domain/Syntax.h"
#include "Instruction.h"
#include <queue>


using namespace elet::domain::compiler::instruction;


namespace elet::domain::compiler::instruction
{


using namespace compiler;


class Transformer
{
public:

    Transformer(std::queue<output::Routine*>& routines, std::mutex& routineWorkMutex);

    output::Routine*
    transform(Declaration* declaration);

private:

    static
    thread_local
    NameToDeclarationMap*
    _localSymbols;

    std::queue<output::Routine*>&
    _routines;

    std::mutex&
    _routineWorkMutex;

    output::Instruction*
    createInstruction(embedded::InstructionType type);

    output::Routine*
    createRoutine(Utf8StringView& name);

    output::Function*
    createFunction(Utf8StringView& name);

    List<output::Instruction*>*
    writeFunctionBody(FunctionBody* body, std::map<Utf8StringView, output::Parameter*>& parameters);

    List<output::Instruction*>*
    writeLocalStatements(List<Syntax*>& statments, std::map<Utf8StringView, output::Parameter*>& parameters);

    void
    resolveAssemblyReference(output::Operand** operand, std::map<Utf8StringView, output::Parameter*>& parameters);

    std::size_t
    lazilyResolveTypeSize(Type* type) const;

    Utf8String
    getSymbol(NamedExpression *expression) const;

    void
    transformCallExpression(CallExpression* callExpression, List<output::Instruction*>* routine);

    void
    transformVariableDeclaration(VariableDeclaration* variableDeclaration);

    void
    transformAssemblyBlock(AssemblyBlock* assemblyBlock, List<output::Instruction*>* routine, std::map<Utf8StringView, output::Parameter*>& parameters);
};


}


#endif //ELET_TRANSFORMER_H
