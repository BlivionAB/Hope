#ifndef ELET_TRANSFORMER_H
#define ELET_TRANSFORMER_H


#include "Domain/Compiler/Syntax/Syntax.h"
#include "Instruction.h"
#include <queue>
#include <stack>
#include <cstdint>
#include <Domain/Compiler/Compiler.h>

#define TYPE_SIZE_64 8
#define TYPE_SIZE_32 4
#define TYPE_SIZE_8 1

using namespace elet::domain::compiler::instruction;


namespace elet::domain::compiler
{
    struct Symbol;
    struct CompilerOptions;

    namespace ast
    {
        struct Declaration;
        struct NamedExpression;
        struct StringLiteral;
        struct PropertyExpression;
        struct ArgumentDeclaration;

        namespace type
        {
            enum TypeSize : uint64_t;
        }
    }
}

namespace elet::domain::compiler::instruction
{

namespace output
{
    struct Function;
    struct Parameter;
    struct Operand;
    struct FunctionReference;
    struct  LoadInstruction;
    struct StoreRegisterInstruction;
    struct ParameterDeclaration;
    struct String;
    struct Register;
    struct StoreRegisterInstruction;
    struct ArgumentDeclaration;
    typedef std::variant<std::size_t, String*, ParameterDeclaration*, StoreRegisterInstruction*> ArgumentValue;
    struct InternalRoutine;
    struct FunctionRoutine;
    struct MoveRegisterInstruction;
    typedef std::variant<int8_t, uint8_t, int32_t, uint32_t, int64_t, uint64_t> ImmediateValue;
    enum class OperandRegister;
    typedef std::variant<std::monostate, OperandRegister, ImmediateValue> CanonicalExpression;
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


enum class StackSubtractionLocation
{
    BeforePush,
    AfterPush,
    BeforePop = AfterPush,
    AfterPop = BeforePush,
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


typedef std::variant<std::monostate, output::Register, output::ScratchRegister*> BinaryTemp;


class Transformer
{
public:

    Transformer(std::mutex& dataMutex, CompilerOptions& compilerOptions);

    output::FunctionRoutine*
    transform(ast::Declaration* declaration);

private:

    void
    transformFunctionParameters(const ast::FunctionDeclaration* functionDeclaration, output::FunctionRoutine* routine, uint64_t& stackOffset);

    output::FunctionRoutine*
    transformFunctionDeclaration(const ast::FunctionDeclaration* functionDeclaration);

    void
    transformArgumentStringLiteral(ast::StringLiteral* stringLiteral, uint64_t argumentIndex);

    output::FunctionRoutine*
    createFunctionRoutine(const Utf8StringView& name);

    List<output::Instruction*>*
    transformLocalStatements(List<ast::Syntax*>& statements, uint64_t& stackOffset);

    static
    Utf8StringView
    getSymbolReference(ast::NamedExpression *expression);

    void
    transformCallExpression(const ast::CallExpression* callExpression, uint64_t& stackOffset);

    output::String*
    addStaticConstantString(ast::StringLiteral* stringLiteral);

    static
    uint64_t
    getDefaultFunctionStackOffset(const CompilerOptions& compilerOptions);

    uint64_t
    _defaultFunctionStackOffset;

    CompilerOptions&
    _compilerOptions;

    std::size_t
    _pointerSize = TYPE_SIZE_64;

    List<output::String*>
    _cstrings;

    unsigned int
    _currentArgumentIndex = 0;

    unsigned int
    _currentParameterIndex = 0;

    std::mutex&
    _dataMutex;

    std::stack<output::InternalRoutine*>
    _currentRoutineStack;

    List<output::ParameterDeclaration*>
    segmentParameterDeclaration(ast::ParameterDeclaration* parameter);

    void
    transformArgumentPropertyExpression(ast::PropertyExpression* propertyExpression, uint64_t argumentIndex);

    void
    transformVariableDeclaration(ast::VariableDeclaration* variable, uint64_t& stackOffset);

    void
    addInstruction(output::Instruction* instruction);

    output::CanonicalExpression
    transformExpression(ast::Expression* expression, uint64_t& stackOffset, output::OperandRegister operandRegister);

    bool
    isImmediateValueExpression(ast::Expression* expression);

    output::ImmediateValue
    transformImmediateBinaryExpression(ast::BinaryExpression* binaryExpression);

    output::ScratchRegister*
    transformImmediateToMemoryExpression(output::ScratchRegister* left, output::ImmediateValue right, ast::BinaryOperatorKind _operator);

    output::ImmediateValue
    transformToUint(ast::IntegerLiteral* expression);

    void
    transformReturnStatement(ast::ReturnStatement* returnStatement, uint64_t& stackOffset);

    output::OperandRegister
    transformPropertyExpression(ast::PropertyExpression* propertyExpression, output::OperandRegister operandRegister);

    bool
    isAddressValueExpression(ast::Expression* expression);

    void
    transformMemoryToMemoryBinaryExpression(ast::BinaryOperatorKind binaryOperatorKind);

    output::CanonicalExpression
    transformBinaryExpression(ast::BinaryExpression* binaryExpression, uint64_t& stackOffset);

    output::OperandRegister
    getOperandRegisterFromArgumentIndex(uint64_t argumentIndex);

    uint64_t
    getAlignedStackSizeFromStackOffset(uint64_t offset);
};


}


#endif //ELET_TRANSFORMER_H
