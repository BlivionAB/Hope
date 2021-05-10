#ifndef ELET_TRANSFORMER_H
#define ELET_TRANSFORMER_H


#include "Domain/Compiler/Syntax/Syntax.h"
#include "Instruction.h"
#include <queue>
#include <stack>
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
        struct StringLiteral;
        struct PropertyExpression;
        struct ArgumentDeclaration;
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
    struct ParameterDeclaration;
    struct CString;
    struct LocalVariableDeclaration;
    struct ArgumentDeclaration;
    typedef std::variant<std::size_t, CString*, ParameterDeclaration*, LocalVariableDeclaration*> ArgumentValue;
    struct InternalRoutine;
    struct FunctionRoutine;
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

    std::size_t
    resolvePrimitiveTypeSize(ast::TypeAssignment* type) ;

    static
    Utf8StringView
    getSymbolReference(ast::NamedExpression *expression);

    void
    transformCallExpression(const ast::CallExpression* callExpression);

    output::CString*
    addStaticConstantString(ast::StringLiteral* stringLiteral);

    std::size_t
    _pointerSize = TYPE_SIZE_64;

    List<output::CString*>
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

    List<output::ArgumentDeclaration*>
    segmentArgumentDeclarations(ast::ArgumentDeclaration* parameter);

    output::ArgumentDeclaration*
    transformArgumentPropertyExpression(ast::PropertyExpression* propertyExpression);
};


}


#endif //ELET_TRANSFORMER_H
