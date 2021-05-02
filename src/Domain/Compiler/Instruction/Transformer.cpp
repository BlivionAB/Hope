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


output::Routine*
Transformer::transform(ast::Declaration* declaration)
{
    if (declaration->kind == ast::SyntaxKind::FunctionDeclaration)
    {
        ast::FunctionDeclaration* functionDeclaration = reinterpret_cast<ast::FunctionDeclaration*>(declaration);
        output::Routine* routine = transformFunctionDeclaration(functionDeclaration);
        functionDeclaration->routine = routine;
    }
    else
    {
        throw UnknownDeclaration();
    }
}


output::Routine*
Transformer::createRoutine(const Utf8StringView& name)
{
    output::Routine* routine = new output::Routine();
    routine->name = name;
    return routine;
}


output::Routine*
Transformer::transformFunctionDeclaration(ast::FunctionDeclaration* functionDeclaration)
{
    output::Routine* routine = _currentRoutine = createRoutine(functionDeclaration->name->name);
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
    _currentRoutine->instructions.add(new CallInstruction());
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
