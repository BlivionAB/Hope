#ifndef ELET_CHECKER_H
#define ELET_CHECKER_H

#include <map>
#include <fmt/format.h>
#include <Foundation/Utf8StringView.h>
#include "Binder.h"
#include "Syntax/Syntax.h"
#include "Syntax/Syntax.Type.h"

using namespace elet::foundation;

namespace elet::domain::compiler
{


class Binder;


namespace ast
{
    struct Declaration;
    struct FunctionDeclaration;
    struct CallExpression;
    struct PropertyExpression;
}

#define TYPE_CHAR       (std::uint8_t)0x1
#define TYPE_INT8      (std::uint8_t)0x2
#define TYPE_INT16     (std::uint8_t)0x3
#define TYPE_INT32     (std::uint8_t)0x4
#define TYPE_INT64     (std::uint8_t)0x5
#define TYPE_UINT8      (std::uint8_t)0x6
#define TYPE_UINT16     (std::uint8_t)0x7
#define TYPE_UINT32     (std::uint8_t)0x8
#define TYPE_UINT64     (std::uint8_t)0x9
#define TYPE_CUSTOM     (std::uint8_t)0xa


struct Diagnostic
{
    Utf8String
    message;

    template<typename... Args>
    explicit Diagnostic(const char* message):
        message(message)
    { }

    template<typename... Args>
    explicit Diagnostic(const char* message, Args... args):
        message(fmt::format(message, args...).c_str())
    { }
};


class Checker
{
public:

    Checker(const Binder* _binder);

    void
    checkUsingStatement(const ast::UsingStatement* usingStatement);

    void
    checkTopLevelDeclaration(ast::Declaration* declaration);

    void
    checkFunctionDeclaration(ast::FunctionDeclaration* functionDeclaration);

    void
    checkCallExpression(const ast::CallExpression* callExpression);

    void
    checkDomainDeclaration(const ast::DomainDeclaration* domain);

    List<ast::FunctionDeclaration*>
    startFunctions;

private:

    static
    void
    resolveTypeFromFunctionDeclaration(ast::FunctionDeclaration* functionDeclaration);

    static
    bool
    isTypeEqualToType(const ast::type::Type* target, const ast::type::Type* source);

    void
    addDiagnostic(Diagnostic* diagnostic);

    List<Diagnostic*>
    _diagnostics;

    const ast::SourceFile*
    _sourceFile;

    const Binder*
    _binder;

    ast::FunctionDeclaration*
    getDeclarationFromSignature(ast::type::Signature* const& signature, const ast::DomainDeclaration* domain) const;

    void
    checkFunctionSignature(const ast::type::Signature* target, const ast::type::Signature* source);

    void
    checkExpression(ast::Expression* expression);

    void
    checkPropertyExpression(ast::PropertyExpression* propertyExpression);

    ast::type::Type*
    resolveTypeFromDeclaration(ast::Declaration* declaration);

    ast::type::Type*
    resolveTypeFromExpression(ast::Expression* expression);

    void
    checkVariableDeclaration(ast::VariableDeclaration* variable);

    ast::Type*
    inferTypeFromExpression(ast::Expression* expression);

    ast::Type*
    getCommonRealType(ast::Type* type1, ast::Type* type2);

    unsigned int
    getConversionRanking(ast::Type* pType);

    ast::Type*
    getMaxType(ast::Type* type1, ast::Type* type2);

    uint64_t
    getMaxTypeDomain(ast::Type* type);

    ast::Type*
    getUnsignedCounterPart(ast::Type* signedType);

    ast::Type*
    getTypeFromIntegerLiteral(ast::IntegerLiteral* integerLiteral);
};


}


#endif //ELET_CHECKER_H
