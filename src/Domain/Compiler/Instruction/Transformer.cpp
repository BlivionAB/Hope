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
        routine->isStartFunction = functionDeclaration->signature->isStartFunction;
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
    _currentRoutineStack.push(routine);
    routine->name = functionDeclaration->signature->isStartFunction ? "_main" : functionDeclaration->symbol->externalSymbol;
    transformFunctionParameters(functionDeclaration, routine);
    transformLocalStatements(functionDeclaration->body->statements);
    _currentRoutineStack.pop();
    return routine;
}

void
Transformer::transformFunctionParameters(const ast::FunctionDeclaration* functionDeclaration, output::FunctionRoutine* routine)
{
    unsigned int i = 0;
    for (ast::ParameterDeclaration* parameterDeclaration : functionDeclaration->parameterList->parameters)
    {
        List<output::ParameterDeclaration*> parameterList = segmentParameterDeclaration(parameterDeclaration);
        for (const auto& param : parameterList)
        {
            param->index = i;
            routine->parameters.add(param);
            ++i;
        }
    }
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
    auto callInstruction = new CallInstruction();
    for (const auto& argument : callExpression->argumentList->arguments)
    {
        switch (argument->kind)
        {
            case ast::SyntaxKind::StringLiteral:
                callInstruction->arguments.add(transformArgumentStringLiteral(reinterpret_cast<ast::StringLiteral*>(argument)));
                break;
            case ast::SyntaxKind::PropertyExpression:
                callInstruction->arguments.add(transformArgumentPropertyExpression(reinterpret_cast<ast::PropertyExpression*>(argument)));
                break;
            default:;
        }
    }
    const ast::FunctionDeclaration* functionDeclaration = reinterpret_cast<ast::FunctionDeclaration*>(callExpression->referenceDeclaration);
    if (functionDeclaration->body)
    {
        callInstruction->routine = transformFunctionDeclaration(functionDeclaration);
    }
    else
    {
        callInstruction->routine = new output::ExternalRoutine(callExpression->referenceDeclaration->symbol->externalSymbol);
    }
    _currentRoutineStack.top()->instructions.add(callInstruction);
}


ArgumentDeclaration*
Transformer::transformArgumentPropertyExpression(ast::PropertyExpression* propertyExpression)
{
    auto referenceInstruction = propertyExpression->referenceDeclaration->referenceInstruction;
    switch (referenceInstruction->kind)
    {
        case InstructionKind::ParameterDeclaration:
            return new ArgumentDeclaration(_currentArgumentIndex++, referenceInstruction->size, reinterpret_cast<ParameterDeclaration*>(referenceInstruction));
        case InstructionKind::LocalVariableDeclaration:
            return new ArgumentDeclaration(_currentArgumentIndex++, referenceInstruction->size, reinterpret_cast<LocalVariableDeclaration*>(referenceInstruction));
        default:;
    }
}


ArgumentDeclaration*
Transformer::transformArgumentStringLiteral(ast::StringLiteral* stringLiteral)
{
    output::String* cstring = addStaticConstantString(stringLiteral);
    return new ArgumentDeclaration(_currentArgumentIndex++, _pointerSize, cstring);
}


std::size_t
Transformer::resolvePrimitiveTypeSize(ast::TypeAssignment* type)
{
    if (type->size == 0)
    {
        if (!type->pointers.isEmpty())
        {
            return _pointerSize;
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


output::String*
Transformer::addStaticConstantString(ast::StringLiteral* stringLiteral)
{
    auto cstring = new output::String(Utf8StringView(stringLiteral->stringStart, stringLiteral->stringEnd));
    _cstrings.add(cstring);
    return cstring;
}


List<output::ParameterDeclaration*>
Transformer::segmentParameterDeclaration(ast::ParameterDeclaration* parameter)
{
    List<output::ParameterDeclaration*> parameterList;
    if (parameter->resolvedType->pointers > 0)
    {
        output::ParameterDeclaration* parameterDeclaration = new output::ParameterDeclaration(0, TYPE_SIZE_64);
        parameter->referenceInstruction = parameterDeclaration;
        parameterList.add(parameterDeclaration);
    }
    return parameterList;
}


List<output::ArgumentDeclaration*>
Transformer::segmentArgumentDeclarations(ast::ArgumentDeclaration* argumentDeclaration)
{
    List<output::ArgumentDeclaration*> argumentList;
    return argumentList;
}


}
