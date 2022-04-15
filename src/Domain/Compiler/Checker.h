#ifndef ELET_CHECKER_H
#define ELET_CHECKER_H


#include <map>
#include <format>
#include <Foundation/Utf8StringView.h>
#include <Foundation/Int128.h>
#include "Binder.h"
#include "Syntax/Syntax.h"
#include "Syntax/Syntax.Type.h"
#include "Domain/Compiler/Error/Error.h"


using namespace elet::foundation;


namespace elet::domain::compiler::ast
{
    class Binder;
    struct Declaration;
    struct FunctionDeclaration;
    struct CallExpression;
    struct PropertyExpression;

    struct MinTypeValue
    {
        Type*
        type;

        Int128
        value;
    };

    class Checker
    {
    public:

        static Type* anyType;
        static Type* booleanType;
        static Type* s8Type;
        static Type* us8Type;
        static Type* s16Type;
        static Type* us16Type;
        static Type* s32Type;
        static Type* us32Type;
        static Type* s64Type;
        static Type* us64Type;
        static Type* u8Type;
        static Type* u16Type;
        static Type* u32Type;
        static Type* u64Type;

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

        const SourceFile*
        _sourceFile;

        const Binder*
        _binder;

        FunctionDeclaration*
        getDeclarationFromSignature(type::Signature* const& signature, const DomainDeclaration* domain) const;

        bool
        isMatchingFunctionSignature(const type::Signature* target, const type::Signature* source);

        Type*
        checkExpression(Expression* expression);

        bool
        tryGetValueFromBinaryExpression(Int128& value, const BinaryExpression* binaryExpression);

        bool
        tryGetValueFromExpression(elet::foundation::Int128& value, const Expression* expression);

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
        checkIntegerLiteral(IntegerLiteral* integerLiteral);

        template<typename T, typename... Args>
        void
        addError(const Syntax* syntax, Args... args);

        void
        checkReturnStatement(ReturnStatement* returnStatement, FunctionDeclaration* functionDeclaration);

        void
        checkTypeAssignability(Type* placeholder, Type* target, Syntax* targetSyntax);

        Type*
        checkBooleanLiteral(BooleanLiteral* literal);

        bool
        isIntegralType(Type* type);

        bool
        isBooleanType(Type* type);

        bool
        isLogicalOperation(BinaryExpression* binaryExpression);

        bool
        isAny(Type* type);

        bool
        isIntegralSubsetOrEqualType(Type* reference, Type* target);

        std::string
        getTypeString(const Type* type);

        Type*
        getMinIntegralTypeFromImmediateValue(const Int128& value, Expression* binaryExpression);
    };
}


#endif //ELET_CHECKER_H
