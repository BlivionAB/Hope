#include "Binder.h"


namespace elet::domain::compiler
{

thread_local
std::map<Utf8StringView, Declaration*>*
Binder::fileDeclaration = nullptr;


Binder::Binder()
{

}


void
Binder::performWork(BindingWork& work)
{
    fileDeclaration = &work.file->declarations;
    if (work.declaration->kind == SyntaxKind::FunctionDeclaration)
    {
        bindFunction(reinterpret_cast<FunctionDeclaration*>(work.declaration));
    }
}


void
Binder::bindFunction(FunctionDeclaration* declaration)
{
    for (ParameterDeclaration* parameterDeclaration : declaration->parameterList->parameters)
    {
        declaration->body->symbols[parameterDeclaration->name->name] = parameterDeclaration;
    }
    for (Syntax* statement : declaration->body->statements)
    {
        switch (statement->kind)
        {
            case SyntaxKind::AssemblyBlock:
                bindAssemblyBlock(reinterpret_cast<AssemblyBlock*>(statement), declaration->body->symbols);
                break;
            case SyntaxKind::CallExpression:
                bindCallExpression(reinterpret_cast<CallExpression*>(statement));
                break;
        }
    }
}


void
Binder::bindAssemblyBlock(AssemblyBlock* assemblyBlock, SymbolMap& symbols)
{
    for (output::Instruction* instruction : *assemblyBlock->body->instructions)
    {
        tryBindOperand(instruction->operand1, symbols);
        tryBindOperand(instruction->operand2, symbols);
    }
}


void
Binder::tryBindOperand(output::Operand* operand, SymbolMap& symbols)
{
    if (operand && operand->kind == output::OperandKind::AssemblyReference)
    {
        output::AssemblyReference* assemblyReference = reinterpret_cast<output::AssemblyReference*>(operand);
        auto result = symbols.find(assemblyReference->name);
        if (result != symbols.end())
        {
            assemblyReference->reference = result->second;
        }
    }
}

void
Binder::bindCallExpression(CallExpression* callExpression)
{
    auto result = fileDeclaration->find(callExpression->name->name);
    if (result == fileDeclaration->end())
    {
        _forwardedReferences.add(callExpression);
        return;
    }
    callExpression->referenceDeclaration = result->second;
}


}