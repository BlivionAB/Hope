#include "Transformer.h"
#include "Domain/Compiler/Exceptions.h"

namespace elet::domain::compiler::instruction
{

using namespace compiler;


Transformer::Transformer(
    std::mutex& dataMutex):

    _dataMutex(dataMutex)
{

}


output::FunctionRoutine*
Transformer::transform(ast::Declaration* declaration)
{
    if (declaration->kind == ast::SyntaxKind::FunctionDeclaration)
    {
        ast::FunctionDeclaration* functionDeclaration = reinterpret_cast<ast::FunctionDeclaration*>(declaration);
        output::FunctionRoutine* routine = transformFunctionDeclaration(functionDeclaration);
        functionDeclaration->routine = routine;
        return routine;
    }
    else
    {
        throw UnknownDeclaration();
    }
}


output::FunctionRoutine*
Transformer::createFunctionRoutine(const Utf8StringView& name)
{
    return new output::FunctionRoutine(name);
}


output::FunctionRoutine*
Transformer::transformFunctionDeclaration(const ast::FunctionDeclaration* functionDeclaration)
{
    if (functionDeclaration->routine)
    {
        return functionDeclaration->routine;
    }
    output::FunctionRoutine* routine = createFunctionRoutine(functionDeclaration->name->name);
    _currentRoutine = routine;
    transformLocalStatements(functionDeclaration->body->statements);
    return routine;
}


List<output::Instruction*>*
Transformer::transformLocalStatements(List<ast::Syntax*>& statements)
{
    List<output::Instruction*>* list = new List<output::Instruction*>();
    for (const auto& statement : statements)
    {
        switch (statement->kind)
        {
            case ast::SyntaxKind::CallExpression:
                transformCallExpression(reinterpret_cast<ast::CallExpression*>(statement));
                break;
            default:
                throw UnknownLocalStatement();
        }
    }
    return list;
}


void
Transformer::transformCallExpression(const ast::CallExpression* callExpression)
{
    for (const auto& argument : callExpression->argumentList->arguments)
    {
        switch (argument->kind)
        {
            case ast::SyntaxKind::StringLiteral:
                transformArgumentStringLiteral(reinterpret_cast<ast::StringLiteral*>(argument));
                break;
            default:;
        }
    }
    const ast::FunctionDeclaration* functionDeclaration = reinterpret_cast<ast::FunctionDeclaration*>(callExpression->referenceDeclaration);
    if (functionDeclaration->body)
    {
        output::FunctionRoutine* routine = transformFunctionDeclaration(functionDeclaration);
        _currentRoutine->instructions.add(new CallInstruction(routine));
    }
    else
    {
        output::ExternalRoutine* routine = new output::ExternalRoutine(callExpression->name->name);
        _currentRoutine->instructions.add(new CallInstruction(routine));
    }
}


ArgumentDeclaration*
Transformer::createArgumentDeclaration(ArgumentValue value)
{
    return new ArgumentDeclaration(_currentArgumentIndex++, value);
}


void
Transformer::transformArgumentStringLiteral(ast::StringLiteral* stringLiteral)
{
    ArgumentDeclaration* arg = createArgumentDeclaration(addStaticConstantString(stringLiteral));
    _currentRoutine->instructions.add(arg);
}


std::size_t
Transformer::resolvePrimitiveTypeSize(ast::TypeAssignment* type)
{
    if (type->size == 0)
    {
        if (!type->pointers.isEmpty())
        {
            return TYPE_SIZE_64;
        }
        switch (type->type)
        {
            case ast::TypeKind::Int:
            case ast::TypeKind::UInt:
            case ast::TypeKind::UInt64:
                return TYPE_SIZE_64;
            case ast::TypeKind::UInt8:
            case ast::TypeKind::Char:
                return TYPE_SIZE_8;
            default:
                throw UnknownPrimitiveType();
        }
    }
    return type->size;
}


Utf8StringView
Transformer::getSymbolReference(ast::NamedExpression *expression)
{
    return expression->name->name;
}


Utf8StringView*
Transformer::addStaticConstantString(ast::StringLiteral* stringLiteral)
{
    auto string = new Utf8StringView(stringLiteral->stringStart, stringLiteral->stringEnd);
    _cstrings.add(string);
    return string;
}


}
