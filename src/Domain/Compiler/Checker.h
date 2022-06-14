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
        type::Type*
        resolveTypeAssignment(const TypeAssignment* type);

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
        checkExpression(Expression* expression, const Type* operatingType);

        bool
        tryGetValueFromBinaryExpression(Int128& value, const BinaryExpression* binaryExpression);

        bool
        expressionHasImmediateValue(elet::foundation::Int128& value, const Expression* expression);

        Type*
        checkPropertyExpression(PropertyExpression* propertyExpression, const Type* operatingType);

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

        Int128
        getMaxLimitFromType(const Type* type);

        Int128
        getMinLimitFromType(const Type* type);

        Type*
        getUnsignedCounterPart(Type* signedType);

        Type*
        getTypeFromIntegerLiteral(IntegerLiteral* integerLiteral);

        Type*
        checkBinaryExpression(BinaryExpression* binaryExpression, const Type* operatingType);

        Type*
        checkIntegerLiteral(IntegerLiteral* integerLiteral, const Type* operatingType);

        template<typename T, typename... Args>
        void
        addError(const Syntax* syntax, Args... args);

        void
        checkReturnStatement(ReturnStatement* returnStatement, FunctionDeclaration* functionDeclaration);

        bool
        checkTypeAssignability(const Type* placeholder, const Type* target, const Syntax* targetSyntax);

        Type*
        checkBooleanLiteral(BooleanLiteral* literal);

        bool
        isBooleanType(const Type* type) const;

        bool
        isLogicalOperation(BinaryExpression* binaryExpression);

        bool
        isAny(const Type* type) const;

        std::string
        getTypeString(const Type* type);

        Type*
        createOperatingType(const Type* resultingType, const Type* left, const Type* right);

//        Type*
//        getMinIntegralTypeFromImmediateValue(const Int128& value, Expression* binaryExpression);

        Type*
        checkBinaryOperation(BinaryExpression* binaryExpression, const Type* left, const Type* right, const Type* operatingType);

        bool
        checkIntegralBounds(const Type* placeholder, const Type* target, const Syntax* targetSyntax);

        TypeKind
        getMaxSignedType(Type* type1, Type* type2);

        TypeKind
        getMaxUnsignedType(Type* type1, Type* type2);

        bool
        checkIntegralTypeAndBoundsAssignability(const Type* placeholder, const Type* target, const Syntax* targetSyntax);

        bool
        checkIntegralAssignability(const Type* placeholder, const Type* target, const Syntax* targetSyntax);

        Type*
        checkParenExpression(ParenExpression* parenExpression, const Type* operatingType);

    bool
    checkTypeCastRelation(const Type* target, const Type* destination, const TypeCast* syntax);

    Type*
    checkTypeCast(const TypeCast* typeCast);

    void
    setMinMaxOfModuloExpression(const Type* left, const Type* right, Int128& minValue, Int128& maxValue) const;
};
}


#endif //ELET_CHECKER_H
