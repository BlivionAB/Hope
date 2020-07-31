#include "Transformer.h"

namespace elet::domain::compiler::instruction
{

using namespace compiler;


thread_local
NameToDeclarationMap*
Transformer::_localSymbols = nullptr;


Transformer::Transformer(std::queue<output::Routine*>& routines, std::mutex& routineWorkMutex):
    _routines(routines),
    _routineWorkMutex(routineWorkMutex)
{

}

output::Routine*
Transformer::transform(Declaration* declaration)
{
    if (declaration->kind == SyntaxKind::FunctionDeclaration)
    {
        FunctionDeclaration* functionDeclaration = reinterpret_cast<FunctionDeclaration*>(declaration);
        output::Function* _function = createFunction(functionDeclaration->name->name);
        std::size_t offset = 0;
        unsigned int index = 0;
        for (const ParameterDeclaration* parameter : functionDeclaration->parameterList->parameters)
        {
            Utf8String symbol = parameter->name->name.toString();
            std::size_t size = lazilyResolveTypeSize(parameter->type);
            auto outputParam = new output::Parameter(size, offset, index, symbol, parameter->display);
            _function->parameters[symbol] = outputParam;
            _function->parameterOrder.add(symbol);
            offset += size;
            index++;
        }
        _function->instructions = writeFunctionBody(functionDeclaration->body, _function->parameters);
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
    return _function;
}


List<output::Instruction*>*
Transformer::writeFunctionBody(FunctionBody* body, std::map<Utf8StringView, output::Parameter*>& parameters)
{
    auto list = new List<output::Instruction*>();
    list->add(*writeLocalStatements(body->statements, parameters));
    return list;
}


List<output::Instruction*>*
Transformer::writeLocalStatements(List<Syntax*>& statements, std::map<Utf8StringView, output::Parameter*>& parameters)
{
    List<output::Instruction*>* list = new List<output::Instruction*>();
    for (const auto& statement : statements)
    {
        switch (statement->kind)
        {
            case SyntaxKind::CallExpression:
                transformCallExpression(reinterpret_cast<CallExpression*>(statement), list);
                break;
            case SyntaxKind::VariableDeclaration:
//                transformVariableDeclaration();
                break;
            case SyntaxKind::AssemblyBlock:
                transformAssemblyBlock(reinterpret_cast<AssemblyBlock*>(statement), list, parameters);
                break;
        }
    }
    return list;
}


void
Transformer::resolveAssemblyReference(output::Operand** operand, std::map<Utf8StringView, output::Parameter*>& parameters)
{
    if ((*operand)->kind == output::OperandKind::AssemblyReference)
    {
        output::AssemblyReference* assemblyReference = reinterpret_cast<output::AssemblyReference*>(*operand);
        auto result = parameters.find(assemblyReference->name);
        if (result != parameters.end())
        {
            output::Parameter* parameter = result->second;
            *operand = new output::VariableReference(parameter->index, parameter->size, parameter->offset, true);
        }
    }
}


std::size_t
Transformer::lazilyResolveTypeSize(Type* type) const
{
    if (type->size == 0)
    {
        if (!type->pointers.isEmpty())
        {
            return 64;
        }
        switch (type->type)
        {
            case TypeKind::Int:
            case TypeKind::UInt:
            case TypeKind::UInt64:
                return 64;
            case TypeKind::UInt8:
            case TypeKind::Char:
                return 8;
        }
    }
    return type->size;
}

void
Transformer::transformAssemblyBlock(AssemblyBlock *assemblyBlock, List<output::Instruction*>* routine, std::map<Utf8StringView, output::Parameter*>& parameters)
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
Transformer::transformVariableDeclaration(VariableDeclaration* variableDeclaration)
{

}

void
Transformer::transformCallExpression(CallExpression* callExpression, List<output::Instruction*>* routine)
{
    output::Instruction* instruction = createInstruction(embedded::InstructionType::Call);
    Utf8StringView symbol = getSymbol(callExpression);
    instruction->operand1 = new output::Label(symbol);
    routine->add(instruction);
}


Utf8String
Transformer::getSymbol(NamedExpression *expression) const
{
    return expression->referenceDeclaration->symbol;
}


}
