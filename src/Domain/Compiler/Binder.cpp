#include "Binder.h"
#include "Parser.Error.h"
#include <variant>

namespace elet::domain::compiler
{

thread_local
std::map<Utf8StringView, ast::Declaration*>*
Binder::_fileDeclaration = nullptr;


Binder::Binder()
{

}


void
Binder::performWork(DeclarationWork& work)
{
    _fileDeclaration = &work.file->declarations;

    switch (work.declaration->kind)
    {
        case ast::SyntaxKind::UsingStatement:
            bindUsingStatement(reinterpret_cast<ast::UsingStatement*>(work.declaration));
            break;
        case ast::SyntaxKind::FunctionDeclaration:
            bindFunction(reinterpret_cast<ast::FunctionDeclaration*>(work.declaration));
            break;
    }
}


void
Binder::bindUsingStatement(ast::UsingStatement* usingStatement)
{
    auto declarationMap = getDomainDeclarations(usingStatement->usage, &_globalDeclarations);
}


std::map<Utf8StringView, ast::Declaration*>*
Binder::getDomainDeclarations(const ast::DomainAccessUsage* domainAccessUsage, const AccessMap* accessMap)
{
    auto result = accessMap->find(domainAccessUsage->name->name);
    if (result != accessMap->end())
    {
        if (auto declarationMap = std::get_if<std::map<Utf8StringView, ast::Declaration*>*>(&result->second))
        {
            return *declarationMap;
        }
        else
        {
            if (domainAccessUsage->usage && domainAccessUsage->usage->kind == ast::SyntaxKind::DomainAccessUsage)
            {
                return getDomainDeclarations(
                    reinterpret_cast<ast::DomainAccessUsage*>(domainAccessUsage->usage),
                    reinterpret_cast<AccessMap*>(std::get<void*>(result->second)));
            }
        }
    }

    throw UndefinedDomainAccessError();
}


void
Binder::bindFunction(ast::FunctionDeclaration* declaration)
{
    _currentFunctionDeclaration = declaration;
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
    for (auto expr : callExpression->argumentList->arguments)
    {
        bindExpression(expr);
    }
    auto result = _fileDeclaration->find(callExpression->name->name);
    if (result == _fileDeclaration->end())
    {
        _forwardedReferences.add(callExpression);
        return;
    }
    callExpression->referenceDeclaration = result->second;
}


void
Binder::bindExpression(ast::Expression* expression)
{
    switch (expression->kind)
    {
        case ast::SyntaxKind::PropertyExpression:
            bindPropertyExpression(reinterpret_cast<ast::PropertyExpression*>(expression));
            break;
    }
}


void
Binder::bindPropertyExpression(ast::PropertyExpression* property)
{
    auto declaration = _currentFunctionDeclaration->body->symbols[property->name->name];
    if (declaration)
    {
        property->referenceDeclaration = declaration;
    }
}


}