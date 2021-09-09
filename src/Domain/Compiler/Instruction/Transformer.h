#ifndef ELET_TRANSFORMER_H
#define ELET_TRANSFORMER_H


#include "Domain/Compiler/Syntax/Syntax.h"
#include "Instruction.h"
#include <queue>
#include <stack>
#include <Domain/Compiler/Compiler.h>

#define TYPE_SIZE_64 8
#define TYPE_SIZE_32 4
#define TYPE_SIZE_8 1

using namespace elet::domain::compiler::instruction;


namespace elet::domain::compiler
{
    struct Symbol;

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
    struct VariableDeclaration;
    struct ParameterDeclaration;
    struct String;
    struct Register;
    struct VariableDeclaration;
    struct ArgumentDeclaration;
    typedef std::variant<std::size_t, String*, ParameterDeclaration*, VariableDeclaration*> ArgumentValue;
    struct InternalRoutine;
    struct FunctionRoutine;
    typedef std::variant<int64_t, uint64_t> ImmediateValue;
    typedef std::variant<ScratchRegister*, ImmediateValue> CanonicalExpression;
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


typedef std::variant<std::monostate, output::Register, output::ScratchRegister*> BinaryTemp;


class Transformer
{
public:

    Transformer(
        std::mutex& dataMutex);

    output::FunctionRoutine*
    transform(ast::Declaration* declaration);

private:

    void
    transformFunctionParameters(const ast::FunctionDeclaration* functionDeclaration, output::FunctionRoutine* routine);

    output::FunctionRoutine*
    transformFunctionDeclaration(const ast::FunctionDeclaration* functionDeclaration);

    output::ArgumentDeclaration*
    transformArgumentStringLiteral(ast::StringLiteral* stringLiteral);

    output::FunctionRoutine*
    createFunctionRoutine(const Utf8StringView& name);

    List<output::Instruction*>*
    transformLocalStatements(List<ast::Syntax*>& statements);

    static
    Utf8StringView
    getSymbolReference(ast::NamedExpression *expression);

    void
    transformCallExpression(const ast::CallExpression* callExpression);

    output::String*
    addStaticConstantString(ast::StringLiteral* stringLiteral);

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

    bool
    isCompositeExpression(ast::Expression* expression);

    List<output::ArgumentDeclaration*>
    segmentArgumentDeclarations(ast::ArgumentDeclaration* parameter);

    output::ArgumentDeclaration*
    transformArgumentPropertyExpression(ast::PropertyExpression* propertyExpression);

    void
    transformVariableDeclaration(ast::VariableDeclaration* variable);

    void
    addInstruction(output::Instruction* instruction);

    output::CanonicalExpression
    transformExpression(ast::Expression* expression);

    bool
    isImmediateExpression(ast::Expression* expression);

    output::ImmediateValue
    transformImmediateBinaryExpression(ast::BinaryExpression* binaryExpression);

    output::ScratchRegister*
    transformImmediateToMemoryExpression(output::ScratchRegister* left, output::ImmediateValue right, ast::BinaryOperatorKind _operator);

    output::ImmediateValue
    transformToUint(ast::IntegerLiteral* expression);

    void
    transformReturnStatement(ast::ReturnStatement* returnStatement);
};


}


#endif //ELET_TRANSFORMER_H
