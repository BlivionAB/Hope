#ifndef ELET_TRANSFORMER_H
#define ELET_TRANSFORMER_H


#include "Domain/Compiler/Syntax/Syntax.h"
#include "Domain/Compiler/Syntax/Instruction.h"
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
        struct StringLiteral;
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
    struct ArgumentDeclaration;
    typedef std::variant<unsigned int, int, Utf8StringView*> ArgumentValue;
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

    output::FunctionRoutine*
    transformFunctionDeclaration(const ast::FunctionDeclaration* functionDeclaration);

    void
    transformArgumentStringLiteral(ast::StringLiteral* stringLiteral);

    output::FunctionRoutine*
    createFunctionRoutine(const Utf8StringView& name);

    output::ArgumentDeclaration*
    createArgumentDeclaration(output::ArgumentValue value);

    List<output::Instruction*>*
    transformLocalStatements(List<ast::Syntax*>& statements);


    static
    std::size_t
    resolvePrimitiveTypeSize(ast::TypeAssignment* type) ;

    static
    Utf8StringView
    getSymbolReference(ast::NamedExpression *expression);

    void
    transformCallExpression(const ast::CallExpression* callExpression);

    Utf8StringView*
    addStaticConstantString(ast::StringLiteral* stringLiteral);

    List<Utf8StringView*>
    _cstrings;

    unsigned int
    _currentArgumentIndex = 0;

    std::mutex&
    _dataMutex;

    output::InternalRoutine*
    _currentRoutine;
};


}


#endif //ELET_TRANSFORMER_H
