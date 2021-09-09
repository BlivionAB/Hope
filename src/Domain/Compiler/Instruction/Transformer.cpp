#include "Transformer.h"
#include "Domain/Compiler/Exceptions.h"
#include "Instruction.h"

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
            case ast::SyntaxKind::VariableDeclaration:
                transformVariableDeclaration(reinterpret_cast<ast::VariableDeclaration*>(statement));
                break;
            case ast::SyntaxKind::CallExpression:
                transformCallExpression(reinterpret_cast<ast::CallExpression*>(statement));
                break;
            case ast::SyntaxKind::ReturnStatement:
                transformReturnStatement(reinterpret_cast<ast::ReturnStatement*>(statement));
                break;
            default:
                throw UnknownLocalStatement();
        }
    }
    return list;
}


void
Transformer::transformReturnStatement(ast::ReturnStatement* returnStatement)
{

}


void
Transformer::transformVariableDeclaration(ast::VariableDeclaration* variable)
{
    output::VariableDeclaration* localVariable = new output::VariableDeclaration(variable->resolvedType->size());
    localVariable->expression = transformExpression(variable->expression);
    addInstruction(localVariable);
}


CanonicalExpression
Transformer::transformExpression(ast::Expression* expression)
{
    switch (expression->kind)
    {
        // Expressions must be canonicalized before transformation
        case ast::SyntaxKind::BinaryExpression:
        {

            // memory * immediate
            //   immediate * memory (it's cummutative)
            // immediate * immediate
            // memory * memory

            ast::BinaryExpression* binaryExpression = reinterpret_cast<ast::BinaryExpression*>(expression);
            auto left = transformExpression(binaryExpression->left);
            if (std::holds_alternative<ImmediateValue>(left))
            {
                auto right = transformExpression(binaryExpression->right);
                if (std::holds_alternative<ImmediateValue>(right))
                {
                    return transformImmediateBinaryExpression(binaryExpression);
                }

                // rhs is memory here
                return transformImmediateToMemoryExpression(std::get<ScratchRegister*>(right), transformToUint(reinterpret_cast<ast::IntegerLiteral*>(binaryExpression->left)), binaryExpression->binaryOperatorKind);
            }
            // lhs is memory from here

//            else if (isMemoryExpression(binaryExpression->right))
//            {
//                return transformMemoryBinaryExpression(binaryExpression);
//            }
//            return transformImmediateToMemoryExpression(transformExpression(binaryExpression->left), transformToUint(reinterpret_cast<ast::IntegerLiteral*>(binaryExpression->right)), binaryExpression->binaryOperatorKind);
        }
        case ast::SyntaxKind::IntegerLiteral:
            return reinterpret_cast<ast::IntegerLiteral*>(expression)->value;
        default:
            throw std::runtime_error("");
    }
}


output::ImmediateValue
Transformer::transformToUint(ast::IntegerLiteral* integer)
{
    return integer->value;
}


output::ScratchRegister*
Transformer::transformImmediateToMemoryExpression(ScratchRegister* left, ImmediateValue right, ast::BinaryOperatorKind _operator)
{
    auto scratchRegister = new ScratchRegister();
    scratchRegister->operation = new ImmediateToMemoryOperation(left, right, _operator);
    return scratchRegister;
}


ImmediateValue
Transformer::transformImmediateBinaryExpression(ast::BinaryExpression* binaryExpression)
{
    switch (binaryExpression->binaryOperatorKind)
    {
        case ast::BinaryOperatorKind::Plus:
        {
            ast::IntegerLiteral* left = reinterpret_cast<ast::IntegerLiteral*>(binaryExpression->left);
            ast::IntegerLiteral* right = reinterpret_cast<ast::IntegerLiteral*>(binaryExpression->right);
            if (left->resolvedType->sign() == ast::type::Signedness::Signed)
            {
                switch (left->resolvedType->size())
                {
                    case ast::TypeSize::_64:
                        return left->value + right->value;
//                    case ast::TypeSize::_32:
//                        return static_cast<int32_t>(left->value) + static_cast<int32_t>(right->value);
                    default:
                        throw std::runtime_error("Should not contain lower than 32bit ints.");
                }
            }
            else
            {
                switch (left->resolvedType->size())
                {
                    case ast::TypeSize::_64:
                        return static_cast<uint64_t>(left->value) + static_cast<uint64_t>(right->value);
//                    case ast::TypeSize::_32:
//                        return static_cast<uint32_t>(left->value) + static_cast<uint32_t>(right->value);
                    default:
                        throw std::runtime_error("Should not contain lower than 32bit ints.");
                }
            }
        }
        default:
            throw std::runtime_error("Not implemented binary expression operator for immediate value.");
    }
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
    addInstruction(callInstruction);
}


void
Transformer::addInstruction(Instruction* instruction)
{
    _currentRoutineStack.top()->instructions.add(instruction);
}


ArgumentDeclaration*
Transformer::transformArgumentPropertyExpression(ast::PropertyExpression* propertyExpression)
{
    auto referenceInstruction = propertyExpression->referenceDeclaration->referenceInstruction;
    switch (referenceInstruction->kind)
    {
        case InstructionKind::ParameterDeclaration:
            return new ArgumentDeclaration(_currentArgumentIndex++, referenceInstruction->size, reinterpret_cast<ParameterDeclaration*>(referenceInstruction));
        case InstructionKind::VariableDeclaration:
            return new ArgumentDeclaration(_currentArgumentIndex++, referenceInstruction->size, reinterpret_cast<VariableDeclaration*>(referenceInstruction));
        default:;
    }
}


ArgumentDeclaration*
Transformer::transformArgumentStringLiteral(ast::StringLiteral* stringLiteral)
{
    output::String* cstring = addStaticConstantString(stringLiteral);
    return new ArgumentDeclaration(_currentArgumentIndex++, ast::type::TypeSize::Pointer, cstring);
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
        output::ParameterDeclaration* parameterDeclaration = new output::ParameterDeclaration(0, ast::type::TypeSize::_64);
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


bool
Transformer::isCompositeExpression(ast::Expression* expression)
{
    return expression->kind == ast::SyntaxKind::BinaryExpression;
}


bool
Transformer::isImmediateExpression(ast::Expression* expression)
{
    return expression->kind == ast::SyntaxKind::IntegerLiteral;
}

}
