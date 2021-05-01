#ifndef ELET_TRANSFORMER_H
#define ELET_TRANSFORMER_H


#include "Domain/Compiler/Syntax/Syntax.h"
#include "Instruction.h"
#include <queue>
#include <Domain/Compiler/Compiler.h>

#define TYPE_SIZE_64 64
#define TYPE_SIZE_32 32
#define TYPE_SIZE_8 8

using namespace elet::domain::compiler::instruction;


namespace elet::domain::compiler
{
    struct Symbol;

    namespace ast
    {
        struct Declaration;
        struct NamedExpression;
    }
}

namespace elet::domain::compiler::instruction
{

namespace output
{
    struct Routine;
    struct Function;
    struct Parameter;
    struct Operand;
    struct FunctionReference;
}
using namespace compiler;

#define R0      1 << 1
#define R1      1 << 2
#define R2      1 << 3
#define R3      1 << 4
#define R4      1 << 5
#define R5      1 << 6
#define R6      1 << 7
#define R7      1 << 8
#define R8      1 << 9
#define R9      1 << 10
#define R10     1 << 11
#define R11     1 << 12

enum class ParameterStackOrder
{
    RTL,
    LTR,
};


struct CallingConvention
{
//    std::uint8_t
//    stackAlignment;

    List<std::uint8_t>
    parameterRegisters;

    std::uint8_t
    numberOfParameterRegisters;

    List<std::uint32_t>
    returnRegisters;

    std::uint8_t
    numberReturnRegisters;
//
//    std::uint32_t
//    calleeSavedRegisters;
//
//    ParameterStackOrder
//    stackOrder;
};


class Transformer
{
public:

    Transformer(
        const CallingConvention* callingConvention,
        std::queue<output::Routine*>& routines,
        std::mutex& routineWorkMutex,
        List<Utf8StringView*>* data,
        std::uint64_t& cstringOffset,
        std::mutex& dataMutex);

    output::Routine*
    transform(ast::Declaration* declaration);

private:

    static output::Instruction*
    createInstruction(embedded::InstructionType type);

    static output::Routine*
    createRoutine(Utf8StringView& name);

    static output::Function*
    createFunction(Utf8StringView& name);

    List<output::Instruction*>*
    transformFunctionBody(ast::StatementBlock* body, List<output::Parameter*>& parameters);

    List<output::Instruction*>*
    transformLocalStatements(List<ast::Syntax*>& statements, List<output::Parameter*>& parameters);

    static
    void
    resolveAssemblyReference(output::Operand** operand, List<output::Parameter*>& parameters);

    static
    std::size_t
    resolvePrimitiveTypeSize(ast::TypeAssignment* type) ;

    static
    Utf8StringView
    getSymbolReference(ast::NamedExpression *expression);

    void
    transformCallExpression(ast::CallExpression* callExpression, List<output::Instruction*>* routine);

    void
    transformArgument(ast::Expression* expression, std::size_t numberOfParameterRegisters, unsigned int assignedParameter, List<output::Instruction*>* routine);

    void
    transformVariableDeclaration(ast::VariableDeclaration* variableDeclaration);

    static
    void
    transformAssemblyBlock(ast::AssemblyBlock* assemblyBlock, List<output::Instruction*>* routine, List<output::Parameter*>& parameters);

    void
    addStaticConstantString(const char* start, const char* end);

    void
    transformStringParameter(std::size_t& numberOfParameterRegisters, unsigned int& parameterIndex, List<output::Instruction*>* routine);

    const CallingConvention*
    _callingConvention;

    std::queue<output::Routine*>&
    _routines;

    std::mutex&
    _routineWorkMutex;

    List<Utf8StringView*>*
    _cstrings;

    /**
     * The data offset is calculated during the transform phase and kept immutable after. It's offset based,
     * hence position independent data. The object file just need to relocate the data in order for usage.
     */
    std::uint64_t&
    _cstringOffset;

    std::mutex&
    _dataMutex;
};


}


#endif //ELET_TRANSFORMER_H
