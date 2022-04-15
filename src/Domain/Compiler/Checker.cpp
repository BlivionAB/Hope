#include "Checker.h"
#include "Domain/Compiler/Error/Error_TypeCheck.h"
#include <Foundation/Int128.h>


namespace elet::domain::compiler::ast
{

    Type* Checker::anyType = new Type(TypeKind::Any);
    Type* Checker::booleanType = new Type(TypeKind::Bool);
    Type* Checker::s8Type = new Type(TypeKind::S8);
    Type* Checker::us8Type = new Type(TypeKind::US8);
    Type* Checker::s16Type = new Type(TypeKind::S16);
    Type* Checker::us16Type = new Type(TypeKind::US16);
    Type* Checker::s32Type = new Type(TypeKind::S32);
    Type* Checker::us32Type = new Type(TypeKind::US32);
    Type* Checker::s64Type = new Type(TypeKind::S64);
    Type* Checker::us64Type = new Type(TypeKind::US64);
    Type* Checker::u8Type = new Type(TypeKind::U8);
    Type* Checker::u16Type = new Type(TypeKind::U16);
    Type* Checker::u32Type = new Type(TypeKind::U32);
    Type* Checker::u64Type = new Type(TypeKind::U64);


    Checker::Checker(const Binder* _binder):
        _binder(_binder)
    {
    }


    void
    Checker::checkTopLevelDeclaration(Declaration* declaration)
    {
        _sourceFile = declaration->sourceFile;
        switch (declaration->kind)
        {
            case SyntaxKind::DomainDeclaration:
                checkDomainDeclaration(reinterpret_cast<const DomainDeclaration*>(declaration));
                break;
            case SyntaxKind::FunctionDeclaration:
                checkFunctionDeclaration(reinterpret_cast<FunctionDeclaration*>(declaration));
                break;
            default:;
        }
    }


    Type*
    Checker::checkExpression(Expression* expression)
    {
        switch (expression->kind)
        {
            case SyntaxKind::BooleanLiteral:
                return checkBooleanLiteral(reinterpret_cast<BooleanLiteral*>(expression));
            case SyntaxKind::BinaryExpression:
                return checkBinaryExpression(reinterpret_cast<BinaryExpression*>(expression));
            case SyntaxKind::PropertyExpression:
                return checkPropertyExpression(reinterpret_cast<PropertyExpression*>(expression));
            case SyntaxKind::IntegerLiteral:
                return checkIntegerLiteral(reinterpret_cast<IntegerLiteral*>(expression));
            default:
                assert("Unknown expression to check.");
        }
    }


    Type*
    Checker::checkPropertyExpression(PropertyExpression* propertyExpression)
    {
        return propertyExpression->referenceDeclaration->resolvedType;
    }


    Type*
    Checker::resolveTypeFromDeclaration(Declaration* declaration)
    {
        return declaration->resolvedType;
    }


    void
    Checker::checkFunctionDeclaration(FunctionDeclaration* functionDeclaration)
    {
        resolveTypeFromFunctionDeclaration(functionDeclaration);
        for (Syntax* statement : functionDeclaration->body->statements)
        {
            try
            {
                switch (statement->kind)
                {
                    case SyntaxKind::CallExpression:
                        checkCallExpression(reinterpret_cast<CallExpression*>(statement));
                        break;
                    case SyntaxKind::VariableDeclaration:
                        checkVariableDeclaration(reinterpret_cast<VariableDeclaration*>(statement));
                        break;
                    case SyntaxKind::ReturnStatement:
                        checkReturnStatement(reinterpret_cast<ReturnStatement*>(statement), functionDeclaration);
                        break;
                    default:;
                }
            }
            catch (error::TypeCheckError* typeCheckException)
            {

            }
        }
    }


    void
    Checker::checkVariableDeclaration(VariableDeclaration* variable)
    {
        Type* expressionType = checkExpression(variable->expression);
        if (variable->type)
        {
            variable->resolvedType = new Type(variable->type->type, variable->type->pointers.size());

            // If expression only consist of immediate values (including binary expressions), try to get the value
            // and calculate the min type for it.
            Int128 value;
            if (tryGetValueFromExpression(value, variable->expression))
            {
                expressionType = getMinIntegralTypeFromImmediateValue(value, variable->expression);
            }
            checkTypeAssignability(variable->resolvedType, expressionType, variable->expression);
        }
        else
        {
            variable->resolvedType = expressionType;
        }
    }


    Type*
    Checker::checkIntegerLiteral(IntegerLiteral* integerLiteral)
    {
        integerLiteral->resolvedType = getTypeFromIntegerLiteral(integerLiteral);
//        if (integerLiteral->isNegative)
//        {
//            if (integerLiteral->resolvedType->kind == TypeKind::S32)
//            {
//                if (integerLiteral->value > static_cast<uint64_t>(IntegerLimit::S32Max) + 1)
//                {
//                    addError<error::IntegralLiteralUnderflowError>(integerLiteral, IntegerKind::S32);
//                }
//                else if (integerLiteral->value > static_cast<uint64_t>(IntegerLimit::S64Max) + 1)
//                {
//                    addError<error::IntegralLiteralUnderflowError>(integerLiteral, IntegerKind::S64);
//                }
//            }
//        }
//        else
//        {
//            if (integerLiteral->resolvedType->kind == TypeKind::S32)
//            {
//                if (integerLiteral->value > static_cast<uint64_t>(IntegerLimit::S32Max))
//                {
//                    addError<error::IntegralLiteralOverflowError>(integerLiteral, IntegerKind::S32);
//                }
//            }
//            if (integerLiteral->resolvedType->kind == TypeKind::S64)
//            {
//                if (integerLiteral->value > static_cast<uint64_t>(IntegerLimit::S64Max))
//                {
//                    addError<error::IntegralLiteralOverflowError>(integerLiteral, IntegerKind::S64);
//                }
//            }
//        }
        return integerLiteral->resolvedType;
    }


    Type*
    Checker::checkBinaryExpression(BinaryExpression* binaryExpression)
    {
        Type* left = checkExpression(binaryExpression->left);
        Type* right = checkExpression(binaryExpression->right);
        if (isIntegralType(left) && isIntegralType(right))
        {
            Type* commonType = getCommonRealType(left, right);
            binaryExpression->resolvedType = commonType;
            return commonType;
        }
        if (isBooleanType(left) && isBooleanType(right))
        {
            if (!isLogicalOperation(binaryExpression))
            {
                addError<error::UndefinedBinaryOperatorError>(binaryExpression->binaryOperator, error::UndefinedBinaryOperatorError::Type::Bool);
                return anyType;
            }
            binaryExpression->resolvedType = booleanType;
            return booleanType;
        }
        if (isAny(left) || isAny(right))
        {
            binaryExpression->resolvedType = anyType;
            return anyType;
        }
        addError<error::TypeMismatchBinaryOperationError>(binaryExpression, left, right);
        return anyType;
    }


    Type*
    Checker::getMinIntegralTypeFromImmediateValue(const Int128& value, Expression* binaryExpression)
    {
        if (value < 0)
        {
            if (value >= IntegerLimit::S8Min)
            {
                return s8Type;
            }
            else if (value >= IntegerLimit::S16Min)
            {
                return s16Type;
            }
            else if (value >= IntegerLimit::S32Min)
            {
                return s32Type;
            }
            else if (value >= IntegerLimit::S64Min)
            {
                return s64Type;
            }
            addError<error::IntegralLiteralUnderflowError>(binaryExpression, IntegerKind::S64);
        }
        else
        {
            if (value <= IntegerLimit::S8Max)
            {
                return us8Type;
            }
            if (value <= IntegerLimit::U8Max)
            {
                return u8Type;
            }
            if (value <= IntegerLimit::S16Max)
            {
                return us16Type;
            }
            if (value <= IntegerLimit::U16Max)
            {
                return u16Type;
            }
            if (value <= IntegerLimit::S32Max)
            {
                return us32Type;
            }
            if (value <= IntegerLimit::U32Max)
            {
                return u32Type;
            }
            if (value <= IntegerLimit::S64Max)
            {
                return us64Type;
            }
            if (value <= IntegerLimit::U64Max)
            {
                return u64Type;
            }
            addError<error::IntegralLiteralOverflowError>(binaryExpression, IntegerKind::U64);
        }
    }


    bool
    Checker::tryGetValueFromBinaryExpression(Int128& value, const BinaryExpression* binaryExpression)
    {
        Int128 left;
        if (!tryGetValueFromExpression(left, binaryExpression->left))
        {
            return false;
        }
        Int128 right;
        if (!tryGetValueFromExpression(right, binaryExpression->right))
        {
            return false;
        }
        switch (binaryExpression->binaryOperatorKind)
        {
            case BinaryOperatorKind::Plus:
                value = left + right;
                break;
            case BinaryOperatorKind::Minus:
                value = left - right;
                break;
            case BinaryOperatorKind::Multiply:
                value = left * right;
                break;
            case BinaryOperatorKind::Divide:
                value = left / right;
                break;
            default:
                throw std::runtime_error("Not implemented binary operator kind in tryGetValueFromBinaryExpression.");
        }
        return true;
    }


    bool
    Checker::tryGetValueFromExpression(Int128& value, const Expression* expression)
    {
        if (expression->kind == SyntaxKind::IntegerLiteral)
        {
            const IntegerLiteral* integerLiteral = reinterpret_cast<const IntegerLiteral*>(expression);
            value = integerLiteral->value;
            return true;
        }
        else if (expression->kind == SyntaxKind::BinaryExpression)
        {
            return tryGetValueFromBinaryExpression(value, reinterpret_cast<const BinaryExpression*>(expression));
        }
        return false;
    }


    Type*
    Checker::getTypeFromIntegerLiteral(IntegerLiteral* integerLiteral)
    {
        if (std::holds_alternative<DecimalLiteral*>(integerLiteral->digits))
        {
            if (integerLiteral->isNegative)
            {
                if (integerLiteral->value >= static_cast<int64_t>(IntegerLimit::S32Min))
                {
                    return s32Type;
                }
                if (integerLiteral->value >= static_cast<int64_t>(IntegerLimit::S64Min))
                {
                    return s64Type;
                }
                addError<error::IntegralLiteralUnderflowError>(integerLiteral, IntegerKind::S64);
                return anyType;
            }
            else
            {
                if (integerLiteral->value <= static_cast<int64_t>(IntegerLimit::S32Max))
                {
                    return s32Type;
                }
                if (integerLiteral->value <= static_cast<int64_t>(IntegerLimit::S64Max))
                {
                    return s64Type;
                }
                addError<error::IntegralLiteralOverflowError>(integerLiteral, IntegerKind::S64);
                return anyType;
            }
        }
        else // Hexadecimal or BinaryLiteral
        {
            if (integerLiteral->value <= static_cast<uint64_t>(IntegerLimit::U32Max))
            {
                return u32Type;
            }
            if (integerLiteral->value <= static_cast<uint64_t>(IntegerLimit::U64Max))
            {
                return u64Type;
            }
            addError<error::IntegralLiteralOverflowError>(integerLiteral, IntegerKind::U64);
            return anyType;
        }
    }


    Type*
    Checker::getCommonRealType(Type* type1, Type* type2)
    {
        // See ImplicitIntegerConversion.md
        if (type1->kind == type2->kind)
        {
            return type1;
        }
        Sign sign1 = type1->sign();
        Sign sign2 = type2->sign();
        if (sign1 == sign2)
        {
            return getMaxType(type1, type2);
        }
        Type* signedType;
        Type* unsignedType;
        if (sign1 == Sign::Signed)
        {
            signedType = type1;
            unsignedType = type2;
        }
        else
        {
            signedType = type2;
            unsignedType = type1;
        }
        if (getMaxTypeDomain(signedType) >= getMaxTypeDomain(unsignedType)/* always true: && getMinTypeDomain(signedType) <= 0 */)
        {
            return signedType;
        }
        return getUnsignedCounterPart(signedType);
    }


    Type*
    Checker::getUnsignedCounterPart(Type* signedType)
    {
        switch (signedType->kind)
        {
            case TypeKind::S8:
                return u8Type;
            case TypeKind::S16:
                return u16Type;
            case TypeKind::S32:
                return u32Type;
            case TypeKind::S64:
                return u64Type;
            default:
                throw std::runtime_error("Unknown signed type to fetch unsigned counter part.");
        }
    }


    uint64_t
    Checker::getMaxTypeDomain(Type* type)
    {
        switch (type->kind)
        {
            case TypeKind::Char:
            case TypeKind::U8:
                return static_cast<uint64_t>(IntegerLimit::U8Max);
            case TypeKind::U16:
                return static_cast<uint64_t>(IntegerLimit::U16Max);
            case TypeKind::U32:
                return static_cast<uint64_t>(IntegerLimit::U32Max);
            case TypeKind::U64:
                return static_cast<uint64_t>(IntegerLimit::U64Max);
            default:
                throw std::runtime_error("Cannot get max domain of type");
        }
    }


    Type*
    Checker::getMaxType(Type* type1, Type* type2)
    {
        int rank1 = getConversionRanking(type1);
        int rank2 = getConversionRanking(type2);
        if (rank1 >= rank2)
        {
            return type1;
        }
        return type2;
    }


    unsigned int
    Checker::getConversionRanking(Type* type)
    {
        return static_cast<unsigned int>(type->kind);
    }


    void
    Checker::checkCallExpression(const CallExpression* callExpression)
    {
        for (const auto& argument : callExpression->argumentList->arguments)
        {
            Type* type = resolveTypeFromExpression(argument);
        }
    }



    Type*
    Checker::resolveTypeFromExpression(Expression* expression)
    {
        if (expression->labels & LABEL__NAMED_EXPRESSION)
        {
            return resolveTypeFromDeclaration(reinterpret_cast<NamedExpression*>(expression)->referenceDeclaration);
        }
        else if (expression->kind == SyntaxKind::StringLiteral)
        {
            return new Type(TypeKind::Char, 1);
        }
        throw std::runtime_error("Unimplemented resolve type from expression.");
    }


    void
    Checker::checkUsingStatement(const UsingStatement* usingStatement)
    {

    }


    void
    Checker::checkDomainDeclaration(const DomainDeclaration* domain)
    {
        for (const auto& decl : domain->block->declarations)
        {
            if (decl->kind == SyntaxKind::FunctionDeclaration)
            {
                checkFunctionDeclaration(reinterpret_cast<FunctionDeclaration*>(decl));
            }
        }
        if (domain->implements)
        {
            int implementationsCount = 0;
            List<Signature*> signatures = domain->implements->signatures;
            for (const auto& signature : domain->implements->signatures)
            {
                FunctionDeclaration* functionDeclaration = getDeclarationFromSignature(signature, domain);
                if (!functionDeclaration)
                {
                    continue;
                }
                if (isMatchingFunctionSignature(functionDeclaration->signature, signature))
                {
                    implementationsCount++;
                }
                if (signature->isStartFunction)
                {
                    functionDeclaration->signature->isStartFunction = true;
                    startFunctions.add(functionDeclaration);
                }
            }
            if (implementationsCount != signatures.size())
            {
                addError<error::MissingImplementationError>(domain->names, domain);
            }
        }
    }

    FunctionDeclaration*
    Checker::getDeclarationFromSignature(type::Signature* const& signature, const DomainDeclaration* domain) const
    {
        for (const auto& decl : domain->block->declarations)
        {
            if (decl->kind == SyntaxKind::FunctionDeclaration)
            {
                auto functionDeclaration = reinterpret_cast<FunctionDeclaration*>(decl);
                if (functionDeclaration->signature->name == signature->name)
                {
                    return functionDeclaration;
                }
            }
        }
        return nullptr;
    }


    void
    Checker::resolveTypeFromFunctionDeclaration(FunctionDeclaration* functionDeclaration)
    {
        if (functionDeclaration->signature)
        {
            return;
        }
        auto signature = new type::Signature();
        signature->name = functionDeclaration->name->name;
        signature->type = new type::Type(functionDeclaration->type->type, functionDeclaration->type->pointers.size());
        for (const auto& p : functionDeclaration->parameterList->parameters)
        {
            const auto param = new type::Parameter();
            param->name = p->name->name;
            p->resolvedType = param->type = new type::Type(p->type->type, p->type->pointers.size());
            signature->parameters.add(param);
        }
        functionDeclaration->signature = signature;
    }


    bool
    Checker::isTypeEqualToType(const Type* target, const Type* source)
    {
        if (target->kind == TypeKind::Any || source->kind == TypeKind::Any)
        {
            return true;
        }
        return target->kind == source->kind;
    }


    bool
    Checker::isMatchingFunctionSignature(const type::Signature* target, const type::Signature* source)
    {
        if (target->name != source->name)
        {
            return false;
        }
        if (target->parameters.size() != source->parameters.size())
        {
            return false;
        }
        for (unsigned int i = 0; target->parameters.size(); i++)
        {
            auto targetParameter = target->parameters[i];
            auto sourceParameter = source->parameters[i];
            if (!isTypeEqualToType(targetParameter->type, sourceParameter->type))
            {
                return false;
            }
        }
        if (!isTypeEqualToType(target->type, source->type))
        {
            return false;
        }
        return true;
    }


    template<typename T, typename... Args>
    void
    Checker::addError(const Syntax* syntax, Args... args)
    {
        errors.add(new T(_sourceFile, syntax, args...));
    }


    void
    Checker::checkReturnStatement(ReturnStatement* returnStatement, FunctionDeclaration* functionDeclaration)
    {
        ast::Type* expressionType = checkExpression(returnStatement->expression);
        checkTypeAssignability(functionDeclaration->signature->type, expressionType, returnStatement->expression);
        returnStatement->expectedType = functionDeclaration->signature->type;
    }


    void
    Checker::checkTypeAssignability(Type* placeholder, Type* target, Syntax* targetSyntax)
    {
        if (placeholder->kind == TypeKind::Any || target->kind == TypeKind::Any)
        {
            return;
        }
        if (placeholder->kind == target->kind)
        {
            return;
        }
        if (isIntegralType(placeholder) && isIntegralType(target))
        {
            if (!isIntegralSubsetOrEqualType(placeholder, target))
            {
                addError<error::IntegralExpressionMisfitError>(
                    targetSyntax,
                    placeholder,
                    Utf8StringView(targetSyntax->start, targetSyntax->end));
            }
            return;
        }
        addError<error::TypeAssignabilityError>(targetSyntax, placeholder, target);
    }


    Type*
    Checker::checkBooleanLiteral(BooleanLiteral* literal)
    {
        return new Type(TypeKind::Bool);
    }


    bool
    Checker::isIntegralType(Type* type)
    {
        return type->kind >= TypeKind::IntegralStart && type->kind <= TypeKind::IntegralEnd;
    }


    bool
    Checker::isBooleanType(Type* type)
    {
        return type->kind == TypeKind::Bool;
    }


    bool
    Checker::isAny(Type* type)
    {
        return type->kind == TypeKind::Any;
    }


    bool
    Checker::isLogicalOperation(BinaryExpression* binaryExpression)
    {
        switch (binaryExpression->binaryOperatorKind)
        {
            case BinaryOperatorKind::And:
            case BinaryOperatorKind::Or:
                return true;
        }
        return false;
    }


    bool
    Checker::isIntegralSubsetOrEqualType(Type* reference, Type* target)
    {
        assert(isIntegralType(reference) && isIntegralType(target) && "target and source must be integral type.");

        switch (reference->kind)
        {
            case TypeKind::S64:
                switch (target->kind)
                {
                    case TypeKind::S64:
                    case TypeKind::US64:
                    case TypeKind::S32:
                    case TypeKind::S16:
                    case TypeKind::S8:
                    case TypeKind::U32:
                    case TypeKind::US32:
                    case TypeKind::U16:
                    case TypeKind::US16:
                    case TypeKind::U8:
                    case TypeKind::US8:
                        return true;
                }
                break;
            case TypeKind::S32:
                switch (target->kind)
                {
                    case TypeKind::S32:
                    case TypeKind::US32:
                    case TypeKind::S16:
                    case TypeKind::S8:
                    case TypeKind::U16:
                    case TypeKind::US16:
                    case TypeKind::U8:
                    case TypeKind::US8:
                        return true;
                }
                break;
            case TypeKind::S16:
                switch (target->kind)
                {
                    case TypeKind::S16:
                    case TypeKind::US16:
                    case TypeKind::S8:
                    case TypeKind::U8:
                    case TypeKind::US8:
                        return true;
                }
                break;
            case TypeKind::S8:
                switch (target->kind)
                {
                    case TypeKind::S8:
                    case TypeKind::US8:
                        return true;
                }
                break;
            case TypeKind::U64:
                switch (target->kind)
                {
                    case TypeKind::U64:
                    case TypeKind::US64:
                    case TypeKind::U32:
                    case TypeKind::US32:
                    case TypeKind::U16:
                    case TypeKind::US16:
                    case TypeKind::U8:
                    case TypeKind::US8:
                        return true;
                }
                break;
            case TypeKind::U32:
                switch (target->kind)
                {
                    case TypeKind::U32:
                    case TypeKind::US32:
                    case TypeKind::U16:
                    case TypeKind::US16:
                    case TypeKind::U8:
                    case TypeKind::US8:
                        return true;
                }
                break;
            case TypeKind::U16:
                switch (target->kind)
                {
                    case TypeKind::U16:
                    case TypeKind::US16:
                    case TypeKind::U8:
                    case TypeKind::US8:
                        return true;
                }
                break;
            case TypeKind::U8:
                switch (target->kind)
                {
                    case TypeKind::U8:
                    case TypeKind::US8:
                        return true;
                }
                break;
        }
        return false;
    }
}
