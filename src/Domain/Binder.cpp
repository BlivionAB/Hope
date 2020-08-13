#include "Binder.h"


namespace elet::domain::compiler
{

thread_local
std::map<Utf8StringView, ast::Declaration*>*
Binder::fileDeclaration = nullptr;


Binder::Binder()
{

}


void
Binder::performWork(BindingWork& work)
{
    fileDeclaration = &work.file->declarations;
    if (work.declaration->kind == ast::SyntaxKind::FunctionDeclaration)
    {
        bindFunction(reinterpret_cast<ast::FunctionDeclaration*>(work.declaration));
    }
}


void
Binder::bindFunction(ast::FunctionDeclaration* declaration)
{
    for (ast::ParameterDeclaration* parameterDeclaration : declaration->parameterList->parameters)
    {
        declaration->body->symbols[parameterDeclaration->name->name] = parameterDeclaration;
    }
    for (ast::Syntax* statement : declaration->body->statements)
    {
        switch (statement->kind)
        {
            case ast::SyntaxKind::AssemblyBlock:
                bindAssemblyBlock(reinterpret_cast<ast::AssemblyBlock*>(statement), declaration->body->symbols);
                break;
            case ast::SyntaxKind::CallExpression:
                bindCallExpression(reinterpret_cast<ast::CallExpression*>(statement));
                break;
        }
    }
}


void
Binder::bindAssemblyBlock(ast::AssemblyBlock* assemblyBlock, SymbolMap& symbols)
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
        auto assemblyReference = reinterpret_cast<output::AssemblyReference*>(operand);
        auto result = symbols.find(assemblyReference->name);
        if (result != symbols.end())
        {
            assemblyReference->reference = result->second;
        }
    }
}

void
Binder::bindCallExpression(ast::CallExpression* callExpression)
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