#include "Transformer.h"

namespace elet::domain::compiler::instruction
{

using namespace compiler;


Transformer::Transformer(
    const CallingConvention* callingConvention,
    std::queue<output::Routine*>& routines,
    std::mutex& routineWorkMutex,
    List<Utf8StringView*>* cstrings,
    std::uint64_t& cstringOffset,
    std::mutex& dataMutex):

    _callingConvention(callingConvention),
    _routines(routines),
    _routineWorkMutex(routineWorkMutex),
    _cstrings(cstrings),
    _cstringOffset(cstringOffset),
    _dataMutex(dataMutex)
{

}


output::Routine*
Transformer::transform(ast::Declaration* declaration)
{
    if (declaration->kind == ast::SyntaxKind::FunctionDeclaration)
    {
        ast::FunctionDeclaration* functionDeclaration = reinterpret_cast<ast::FunctionDeclaration*>(declaration);
        output::Function* _function = createFunction(functionDeclaration->name->name);

        std::size_t offset = 0;
        unsigned int index = 0;
        for (const ast::ParameterDeclaration* parameter : functionDeclaration->parameterList->parameters)
        {
            Utf8String symbol = parameter->name->name.toString();
            std::size_t size = resolvePrimitiveTypeSize(parameter->type);
            auto param = new output::Parameter(size, offset, index, symbol, parameter->display);
            _function->parameters.add(param);
            offset += size;
            index++;

            functionDeclaration->instructionParameters.add(param);
        }
        _function->instructions = transformFunctionBody(functionDeclaration->body, _function->parameters);
        return _function;
    }
}


output::Instruction*
Transformer::createInstruction(embedded::InstructionType type)
{
    output::Instruction* instruction = new output::Instruction();
    instruction->type = type;
    return instruction;
}


output::Routine*
Transformer::createRoutine(Utf8StringView& name)
{
    output::Routine* routine = new output::Routine();
    routine->name = name;
    return routine;
}


output::Function*
Transformer::createFunction(Utf8StringView& name)
{
    output::Function* _function = new output::Function();
    _function->kind = output::RoutineKind::Function;
    _function->name = name;
    _function->symbol = new Symbol(SymbolSectionIndex::Text, 0, name);
    return _function;
}


List<output::Instruction*>*
Transformer::transformFunctionBody(ast::StatementBlock* body, List<output::Parameter*>& parameters)
{
    auto list = new List<output::Instruction*>();
    list->add(*transformLocalStatements(body->statements, parameters));
    return list;
}


List<output::Instruction*>*
Transformer::transformLocalStatements(List<ast::Syntax*>& statements, List<output::Parameter*>& parameters)
{
    List<output::Instruction*>* list = new List<output::Instruction*>();
    for (const auto& statement : statements)
    {
        switch (statement->kind)
        {
            case ast::SyntaxKind::CallExpression:
                transformCallExpression(reinterpret_cast<ast::CallExpression*>(statement), list);
                break;
            case ast::SyntaxKind::VariableDeclaration:
//                transformVariableDeclaration();
                break;
            case ast::SyntaxKind::AssemblyBlock:
                transformAssemblyBlock(reinterpret_cast<ast::AssemblyBlock*>(statement), list, parameters);
                break;
        }
    }
    return list;
}


void
Transformer::resolveAssemblyReference(output::Operand** operand, List<output::Parameter*>& parameters)
{
    if ((*operand)->kind == output::OperandKind::AssemblyReference)
    {
        output::AssemblyReference* assemblyReference = reinterpret_cast<output::AssemblyReference*>(*operand);
//        auto result = parameters.find(assemblyReference->name);
//        if (result != parameters.end())
//        {
//            output::Parameter* parameter = result->second;
//            *operand = new output::VariableReference(parameter->index, parameter->size, parameter->offset, true);
//        }
    }
}


std::size_t
Transformer::resolvePrimitiveTypeSize(ast::TypeAssignment* type) const
{
    if (type->size == 0)
    {
        if (!type->pointers.isEmpty())
        {
            return 64;
        }
        switch (type->type)
        {
            case ast::TypeKind::Int:
            case ast::TypeKind::UInt:
            case ast::TypeKind::UInt64:
                return 64;
            case ast::TypeKind::UInt8:
            case ast::TypeKind::Char:
                return 8;
        }
    }
    return type->size;
}


void
Transformer::transformAssemblyBlock(ast::AssemblyBlock *assemblyBlock, List<output::Instruction*>* routine, List<output::Parameter*>& parameters)
{
    for (output::Instruction* instruction : *assemblyBlock->body->instructions)
    {
        if (instruction->operand1)
        {
            resolveAssemblyReference(&instruction->operand1, parameters);
        }
        if (instruction->operand2)
        {
            resolveAssemblyReference(&instruction->operand2, parameters);
        }
        routine->add(instruction);
    }
}


void
Transformer::transformVariableDeclaration(ast::VariableDeclaration* variableDeclaration)
{

}


void
Transformer::transformCallExpression(ast::CallExpression* callExpression, List<output::Instruction*>* routine)
{
    unsigned int assignedParameter = 0;
    std::size_t numberOfParameterRegisters = _callingConvention->parameterRegisters.size();
    for (const auto& argument : callExpression->argumentList->arguments)
    {
        transformArgument(argument, numberOfParameterRegisters, assignedParameter, routine);
        assignedParameter++;
    }
    output::Instruction* instruction = createInstruction(embedded::InstructionType::Call);
    Utf8StringView reference = getSymbolReference(callExpression);
    instruction->operand1 = new output::FunctionReference(reference);
    routine->add(instruction);
}


void
Transformer::transformArgument(ast::Expression* expression, std::size_t numberOfParameterRegisters, unsigned int assignedParameter, List<output::Instruction*>* routine)
{
    switch (expression->kind)
    {
        case ast::SyntaxKind::StringLiteral:
        {
            auto stringLiteral = reinterpret_cast<ast::StringLiteral*>(expression);
            addStaticConstantString(stringLiteral->stringStart, stringLiteral->stringEnd);
            transformStringParameter(numberOfParameterRegisters, assignedParameter, routine);
            _cstringOffset += stringLiteral->stringEnd - stringLiteral->stringStart + 1;
            break;
        }
        case ast::SyntaxKind::PropertyExpression:
        {
            auto expr = reinterpret_cast<ast::PropertyExpression*>(expression);
            auto ref = expr->referenceDeclaration;
            switch (ref->kind)
            {
                case ast::SyntaxKind::ParameterDeclaration:
                    auto storeInstruction = createInstruction(embedded::InstructionType::StoreAddress64);
//                    storeInstruction->operand1 = new output::Register();
                    break;
            }
            break;
        }
    }
}


void
Transformer::transformStringParameter(std::size_t& numberOfParameterRegisters, unsigned int& parameterIndex, List<output::Instruction*>* routine)
{
    if (numberOfParameterRegisters > 0)
    {
        std::uint8_t registerIndex = _callingConvention->parameterRegisters[parameterIndex];
        auto instruction = createInstruction(embedded::InstructionType::StoreAddress64);
        instruction->operand1 = new output::Register(registerIndex);
        instruction->operand2 = new output::StringReference(_cstringOffset);
        routine->add(instruction);
    }
}


Utf8StringView
Transformer::getSymbolReference(ast::NamedExpression *expression) const
{
    return expression->name->name;
}


void
Transformer::addStaticConstantString(const char* start, const char* end)
{
    _cstrings->add(new Utf8StringView(start, end));
}


}
