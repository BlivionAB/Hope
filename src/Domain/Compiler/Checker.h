#ifndef ELET_CHECKER_H
#define ELET_CHECKER_H


#include <map>
#include <format>
#include <Foundation/Utf8StringView.h>
#include "Binder.h"
#include "Syntax/Syntax.h"
#include "Syntax/Syntax.Type.h"
#include "Exceptions.h"


using namespace elet::foundation;


namespace elet::domain::compiler::ast
{
    class Binder;
    struct Declaration;
    struct FunctionDeclaration;
    struct CallExpression;
    struct PropertyExpression;


    class Checker
    {
    public:

        Checker(const Binder* _binder);

        void
        checkUsingStatement(const UsingStatement* usingStatement);

        void
        checkTopLevelDeclaration(Declaration* declaration);

        void
        checkFunctionDeclaration(FunctionDeclaration* functionDeclaration);

        void
        checkCallExpression(const CallExpression* callExpression);

        void
        checkDomainDeclaration(const DomainDeclaration* domain);

        List<FunctionDeclaration*>
        startFunctions;

        List<error::SyntaxError*>
        errors;

    private:

        static
        void
        resolveTypeFromFunctionDeclaration(FunctionDeclaration* functionDeclaration);

        static
        bool
        isTypeEqualToType(const type::Type* target, const type::Type* source);

        void
        addDiagnostic(Diagnostic* diagnostic);

        List<Diagnostic*>
        _diagnostics;

        const SourceFile*
        _sourceFile;

        const Binder*
        _binder;

        FunctionDeclaration*
        getDeclarationFromSignature(type::Signature* const& signature, const DomainDeclaration* domain) const;

        void
        checkFunctionSignature(const type::Signature* target, const type::Signature* source);

        Type*
        checkExpression(Expression* expression);

        Type*
        checkPropertyExpression(PropertyExpression* propertyExpression);

        type::Type*
        resolveTypeFromDeclaration(Declaration* declaration);

        type::Type*
        resolveTypeFromExpression(Expression* expression);

        void
        checkVariableDeclaration(VariableDeclaration* variable);

        Type*
        getCommonRealType(Type* type1, Type* type2);

        unsigned int
        getConversionRanking(Type* pType);

        Type*
        getMaxType(Type* type1, Type* type2);

        uint64_t
        getMaxTypeDomain(Type* type);

        Type*
        getUnsignedCounterPart(Type* signedType);

        Type*
        getTypeFromIntegerLiteral(IntegerLiteral* integerLiteral);

        Type*
        checkBinaryExpression(BinaryExpression* binaryExpression);

        Type*
        resolveTypeOfIntegerLiteral(IntegerLiteral* integerLiteral);

        template<typename... Args>
        void
        addError(const Syntax* syntax, const char* message, Args... args);

        void
        checkReturnStatement(ReturnStatement* returnStatement, FunctionDeclaration* functionDeclaration);

        void
        checkTypeAssignability(Type* target, Type* reference);
    };
}


#endif //ELET_CHECKER_H
