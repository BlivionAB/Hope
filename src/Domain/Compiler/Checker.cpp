#include "Checker.h"
#include "Domain/Compiler/Error/Error_TypeCheck.h"
#include <Foundation/Int128.h>


namespace elet::domain::compiler::ast
{

    Type* Checker::anyType = new Type(TypeKind::Any);
    Type* Checker::booleanType = new Type(TypeKind::Bool);
    Type* Checker::s8Type = new Type(TypeKind::S8);
    Type* Checker::s16Type = new Type(TypeKind::S16);
    Type* Checker::s32Type = new Type(TypeKind::S32);
    Type* Checker::s64Type = new Type(TypeKind::S64);
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
    Checker::checkExpression(Expression* expression, const Type* operatingType)
    {
        switch (expression->kind)
        {
            case SyntaxKind::BooleanLiteral:
                return checkBooleanLiteral(reinterpret_cast<BooleanLiteral*>(expression));
            case SyntaxKind::BinaryExpression:
                return checkBinaryExpression(reinterpret_cast<BinaryExpression*>(expression), operatingType);
            case SyntaxKind::ParenExpression:
                return checkParenExpression(reinterpret_cast<ParenExpression*>(expression), operatingType);
            case SyntaxKind::PropertyExpression:
                return checkPropertyExpression(reinterpret_cast<PropertyExpression*>(expression), operatingType);
            case SyntaxKind::IntegerLiteral:
                return checkIntegerLiteral(reinterpret_cast<IntegerLiteral*>(expression), operatingType);
            default:
                throw std::runtime_error("Unknown expression in checkExpression.");
        }
    }


    Type*
    Checker::checkParenExpression(ParenExpression* parenExpression, const Type* operatingType)
    {
        return checkExpression(parenExpression->expression, operatingType);
    }


    Type*
    Checker::checkPropertyExpression(PropertyExpression* propertyExpression, const Type* operatingType)
    {
        Type* operandType;
        Type* expressionType = propertyExpression->referenceDeclaration->expressionType;
        if (propertyExpression->typeCast)
        {
            operandType = checkTypeCast(propertyExpression->typeCast);
            operandType->setBounds(expressionType);
        }
        else
        {
            operandType = propertyExpression->referenceDeclaration->expressionType;
        }
        if (!checkTypeAssignability(operatingType, operandType, propertyExpression))
        {
            return anyType;
        }
        return operandType;
    }


    Type*
    Checker::resolveTypeFromDeclaration(Declaration* declaration)
    {
        return declaration->declarationType;
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
        Type* operatingType = variable->declarationType = new Type(variable->type->type, variable->type->pointers.size());
        variable->expressionType = checkExpression(variable->expression, operatingType);
        checkTypeAssignability(variable->declarationType, variable->expressionType, variable->expression);
    }


    Type*
    Checker::checkIntegerLiteral(IntegerLiteral* integerLiteral, const Type* operatingType)
    {
        if (integerLiteral->value > getMaxLimitFromType(operatingType))
        {
            addError<error::IntegralLiteralOverflowError>(integerLiteral, getIntegerKind(operatingType));
            return anyType;
        }
        if (integerLiteral->value < getMinLimitFromType(operatingType))
        {
            addError<error::IntegralLiteralUnderflowError>(integerLiteral, getIntegerKind(operatingType));
            return anyType;
        }
        if (integerLiteral->typeCast)
        {
            Type* type = checkTypeCast(integerLiteral->typeCast);
            if (!checkTypeAssignability(operatingType, type, integerLiteral))
            {
                return integerLiteral->resultingType =
                    integerLiteral->operatingType = anyType;
            }
        }
        return integerLiteral->resultingType =
            integerLiteral->operatingType = new Type(
                operatingType->kind,
                integerLiteral->value,
                integerLiteral->value);
    }

    IntegerKind
    Checker::getIntegerKind(const Type* operatingType) const
    {
        switch (operatingType->kind)
        {
            case TypeKind::U64:
                return IntegerKind::U64;
            case TypeKind::S64:
                return IntegerKind::S64;
            case TypeKind::U32:
                return IntegerKind::U32;
            case TypeKind::S32:
                return IntegerKind::S32;
            case TypeKind::U16:
                return IntegerKind::U16;
            case TypeKind::S16:
                return IntegerKind::S16;
            case TypeKind::U8:
                return IntegerKind::U8;
            case TypeKind::S8:
                return IntegerKind::S8;
            default:
                throw std::runtime_error("Not supported type.");
        }
    }


    Type*
    Checker::checkTypeCast(const TypeCast* typeCast)
    {
        Type* previousType = nullptr;
        while (typeCast)
        {
            Type* type = resolveTypeAssignment(typeCast->type);
            if (previousType)
            {
                if (!checkTypeCastRelation(previousType, type, typeCast))
                {
                    return anyType;
                }
            }
            typeCast = typeCast->typeCast;
            previousType = type;
        }
        return previousType;
    }


    bool
    Checker::checkTypeCastRelation(const Type* target, const Type* destination, const TypeCast* syntax)
    {
        // Integers are up and down castable and across signs. Meaning all integers can be cast
        // to any integers. Though, we later have to add bound checking.
        if (isIntegralType(target) && isIntegralType(destination))
        {
            // TODO: Add bound checking.
            return true;
        }
        throw std::runtime_error("No supported type cast relation in checkTypeCastRelation.");
    }


    Type*
    Checker::checkBinaryExpression(BinaryExpression* binaryExpression, const Type* operatingType)
    {
        Type* left = checkExpression(binaryExpression->left, operatingType);
        Type* right = checkExpression(binaryExpression->right, operatingType);
        if (isIntegralType(left) && isIntegralType(right))
        {
            Type* resultingType = binaryExpression->resultingType = checkBinaryOperation(binaryExpression, left, right, operatingType);
            binaryExpression->operatingType = new Type(*operatingType);
            return resultingType;
        }
        if (isBooleanType(left) && isBooleanType(right))
        {
            if (!isLogicalOperation(binaryExpression))
            {
                addError<error::UndefinedBinaryOperatorError>(binaryExpression->binaryOperator, error::UndefinedBinaryOperatorError::Type::Bool);
                return anyType;
            }
            binaryExpression->operatingType = booleanType;
            binaryExpression->resultingType = booleanType;
            return booleanType;
        }
        if (isAny(left) || isAny(right))
        {
            binaryExpression->operatingType = anyType;
            binaryExpression->resultingType = anyType;
            return anyType;
        }
        addError<error::TypeMismatchBinaryOperationError>(binaryExpression, left, right);
        return anyType;
    }


    Type*
    Checker::checkBinaryOperation(BinaryExpression* binaryExpression, const Type* left, const Type* right, const Type* operatingType)
    {
        Int128 minValue;
        Int128 maxValue;
        switch (binaryExpression->binaryOperatorKind)
        {
            case BinaryOperatorKind::Add:
                maxValue = left->maxValue + right->maxValue;
                minValue = left->minValue + right->minValue;
                break;
            case BinaryOperatorKind::Minus:
                maxValue = left->maxValue - right->minValue;
                minValue = left->minValue - right->maxValue;
                break;
            case BinaryOperatorKind::Multiply:
                if ((left->maxValue.isPositive() && right->maxValue.isPositive()))
                {
                    maxValue = left->maxValue * right->maxValue;
                }
                else if (left->maxValue.isNegative() && right->maxValue.isNegative())
                {
                    maxValue = left->minValue * right->minValue;
                }
                else if (left->maxValue.isNegative())
                {
                    maxValue = left->maxValue * right->minValue;
                }
                else
                {
                    maxValue = left->minValue * right->maxValue;
                }

                if ((left->minValue.isPositive() && right->minValue.isPositive()))
                {
                    minValue = left->minValue * right->minValue;
                }
                else if (left->minValue.isNegative() && right->minValue.isNegative())
                {
                    minValue = Int128::min(left->minValue * right->minValue, left->minValue * right->maxValue, left->maxValue * right->minValue);
                }
                else if (left->maxValue.isNegative())
                {
                    minValue = left->minValue * right->maxValue;
                }
                else
                {
                    minValue = left->maxValue * right->minValue;
                }
                break;
            case BinaryOperatorKind::Divide:
                if (left->minValue.isPositive() && right->minValue.isPositive())
                {
                    maxValue = left->maxValue / right->minValue;
                    minValue = left->minValue / right->maxValue;
                }
                else if (left->maxValue.isNegative() && right->maxValue.isNegative())
                {
                    maxValue = left->maxValue / right->minValue;
                    minValue = left->minValue / right->maxValue;
                }
                else
                {
                    maxValue = Int128::min(left->minValue / right->minValue, left->minValue / right->maxValue, left->maxValue / right->minValue, left->maxValue / right->maxValue);
                    minValue = Int128::min(left->minValue / right->minValue, left->minValue / right->maxValue, left->maxValue / right->minValue, left->maxValue / right->maxValue);
                }
                break;
            case BinaryOperatorKind::Modulo:
                setMinMaxOfModuloExpression(left, right, minValue, maxValue);
                break;
            case BinaryOperatorKind::BitwiseAnd:
                if (left->minValue == left->maxValue && right->minValue == right->maxValue)
                {
                    maxValue = minValue = left->minValue & right->minValue;
                }
                else
                {
                    if (Int128::max(left->maxValue, right->maxValue) <= static_cast<uint64_t>(IntegerLimit::U32Max))
                    {
                        minValue = static_cast<uint64_t>(IntegerLimit::U32Min);
                        maxValue = static_cast<uint64_t>(IntegerLimit::U32Max);
                    }
                    else
                    {
                        minValue = static_cast<uint64_t>(IntegerLimit::U64Min);
                        maxValue = static_cast<uint64_t>(IntegerLimit::U64Max);
                    }
                }
                break;
            case BinaryOperatorKind::BitwiseOr:
                if (left->minValue == left->maxValue && right->minValue == right->maxValue)
                {
                    maxValue = minValue = left->minValue | right->minValue;
                }
                else
                {
                    if (Int128::max(left->maxValue, right->maxValue) <= static_cast<uint64_t>(IntegerLimit::U32Max))
                    {
                        minValue = static_cast<uint64_t>(IntegerLimit::U32Min);
                        maxValue = static_cast<uint64_t>(IntegerLimit::U32Max);
                    }
                    else
                    {
                        minValue = static_cast<uint64_t>(IntegerLimit::U64Min);
                        maxValue = static_cast<uint64_t>(IntegerLimit::U64Max);
                    }
                }
                break;
            case BinaryOperatorKind::BitwiseXor:
                if (left->minValue == left->maxValue && right->minValue == right->maxValue)
                {
                    maxValue = minValue = left->minValue ^ right->minValue;
                }
                else
                {
                    if (Int128::max(left->maxValue, right->maxValue) <= static_cast<uint64_t>(IntegerLimit::U32Max))
                    {
                        minValue = static_cast<uint64_t>(IntegerLimit::U32Min);
                        maxValue = static_cast<uint64_t>(IntegerLimit::U32Max);
                    }
                    else
                    {
                        minValue = static_cast<uint64_t>(IntegerLimit::U64Min);
                        maxValue = static_cast<uint64_t>(IntegerLimit::U64Max);
                    }
                }
                break;
            default:
                throw std::runtime_error("Unsupported binary operator in checkBinaryOperation.");
        }
        if (maxValue > static_cast<uint64_t>(IntegerLimit::U64Max))
        {
            addError<error::IntegralExpressionGlobalOverflowError>(binaryExpression);
            return anyType;
        }
        if (minValue < static_cast<int64_t>(SignedIntegerLimit::S64Min))
        {
            addError<error::IntegralExpressionGlobalUnderflowError>(binaryExpression);
            return anyType;
        }
        if (left->sign() == Sign::Signed && right->sign() == Sign::Signed)
        {
            return new Type(operatingType->kind, minValue, maxValue);
        }
        else if (left->sign() == Sign::Unsigned && right->sign() == Sign::Unsigned)
        {
            return new Type(operatingType->kind, minValue, maxValue);
        }
        addError<error::TypeMismatchBinaryOperationError>(binaryExpression, left, right);
        return anyType;
    }


    void
    Checker::setMinMaxOfModuloExpression(const Type* left, const Type* right, Int128& minValue, Int128& maxValue) const
    {
        Int128 absRightMin = Int128::abs(right->minValue);
        Int128 absRightMax = Int128::abs(right->maxValue);
        Int128 absMaxOfAbsRightMinMax = Int128::max(absRightMin, absRightMax);

        if (left->minValue.isNegative())
        {
            Int128 absMin = Int128::abs(left->minValue);
            if (absMin > absMaxOfAbsRightMinMax)
            {
                minValue = -(absMaxOfAbsRightMinMax - Int128(1));
            }
            else
            {
                minValue = -absMin;
            }
            if (left->maxValue.isNegative())
            {
                Int128 absMax = Int128::abs(left->maxValue);
                if (absMax >= absMaxOfAbsRightMinMax)
                {
                    maxValue = 0;
                }
                else
                {
                    maxValue = -absMax;
                }
            }
            else
            {
                Int128 absMax = left->maxValue;
                if (absMax > absMaxOfAbsRightMinMax)
                {
                    maxValue = absMaxOfAbsRightMinMax - Int128(1);
                }
                else if (absMax == absMaxOfAbsRightMinMax)
                {
                    maxValue = 0;
                }
                else
                {
                    maxValue = absMax;
                }
            }
        }
        else
        {
            Int128 absMin = left->minValue;
            if (absMin > absMaxOfAbsRightMinMax)
            {
                minValue = absMaxOfAbsRightMinMax - Int128(1);
            }
            else if (absMin == absMaxOfAbsRightMinMax)
            {
                minValue = 0;
            }
            else
            {
                minValue = absMin;
            }
            Int128 absMax = left->maxValue;
            if (absMax > absMaxOfAbsRightMinMax)
            {
                maxValue = absMaxOfAbsRightMinMax - Int128(1);
            }
            else if (absMax == absMaxOfAbsRightMinMax)
            {
                minValue = 0;
            }
            else
            {
                maxValue = absMax;
            }
        }
    }


    bool
    Checker::tryGetValueFromBinaryExpression(Int128& value, const BinaryExpression* binaryExpression)
    {
        Int128 left;
        if (!expressionHasImmediateValue(left, binaryExpression->left))
        {
            return false;
        }
        Int128 right;
        if (!expressionHasImmediateValue(right, binaryExpression->right))
        {
            return false;
        }
        switch (binaryExpression->binaryOperatorKind)
        {
            case BinaryOperatorKind::Add:
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
    Checker::expressionHasImmediateValue(Int128& value, const Expression* expression)
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
                if (integerLiteral->value >= SignedIntegerLimit::S32Min)
                {
                    return s32Type;
                }
                if (integerLiteral->value >= SignedIntegerLimit::S64Min)
                {
                    return s64Type;
                }
                addError<error::IntegralLiteralUnderflowError>(integerLiteral, IntegerKind::S64);
                return anyType;
            }
            else
            {
                if (integerLiteral->value <= SignedIntegerLimit::S32Max)
                {
                    return s32Type;
                }
                if (integerLiteral->value <= SignedIntegerLimit::S64Max)
                {
                    return s64Type;
                }
                if (integerLiteral->value <= IntegerLimit::U64Max)
                {
                    return u64Type;
                }
                addError<error::IntegralLiteralOverflowError>(integerLiteral, IntegerKind::U64);
                return anyType;
            }
        }
        else // Hexadecimal or BinaryLiteral
        {
            if (integerLiteral->value <= IntegerLimit::U32Max)
            {
                return u32Type;
            }
            if (integerLiteral->value <= IntegerLimit::U64Max)
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
        if (getMaxLimitFromType(signedType) >= getMaxLimitFromType(unsignedType)/* always true: && getMinBound(signedType) <= 0 */)
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


    Int128
    Checker::getMaxLimitFromType(const Type* type)
    {
        switch (type->kind)
        {
            case TypeKind::Char:
            case TypeKind::U8:
                return static_cast<uint64_t>(IntegerLimit::U8Max);
            case TypeKind::S8:
                return static_cast<uint64_t>(SignedIntegerLimit::S8Max);
            case TypeKind::U16:
                return static_cast<uint64_t>(IntegerLimit::U16Max);
            case TypeKind::S16:
                return static_cast<uint64_t>(SignedIntegerLimit::S16Max);
            case TypeKind::U32:
                return static_cast<uint64_t>(IntegerLimit::U32Max);
            case TypeKind::S32:
                return static_cast<uint64_t>(SignedIntegerLimit::S32Max);
            case TypeKind::U64:
                return static_cast<uint64_t>(IntegerLimit::U64Max);
            case TypeKind::S64:
                return static_cast<uint64_t>(SignedIntegerLimit::S64Max);
            default:
                throw std::runtime_error("Cannot get max domain of type");
        }
    }

    Int128
    Checker::getMinLimitFromType(const Type* type)
    {
        switch (type->kind)
        {
            case TypeKind::Char:
            case TypeKind::U8:
                return static_cast<uint64_t>(IntegerLimit::U8Min);
            case TypeKind::S8:
                return static_cast<int64_t>(SignedIntegerLimit::S8Min);
            case TypeKind::U16:
                return static_cast<uint64_t>(IntegerLimit::U16Min);
            case TypeKind::S16:
                return static_cast<int64_t>(SignedIntegerLimit::S16Min);
            case TypeKind::U32:
                return static_cast<uint64_t>(IntegerLimit::U32Min);
            case TypeKind::S32:
                return static_cast<int64_t>(SignedIntegerLimit::S32Min);
            case TypeKind::U64:
                return static_cast<uint64_t>(IntegerLimit::U64Min);
            case TypeKind::S64:
                return static_cast<int64_t>(SignedIntegerLimit::S64Min);
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
        signature->type = resolveTypeAssignment(functionDeclaration->type);
        for (const auto& p : functionDeclaration->parameterList->parameters)
        {
            const auto param = new type::Parameter();
            param->name = p->name->name;
            p->declarationType = p->expressionType = param->type = new type::Type(p->type->type, p->type->pointers.size());
            signature->parameters.add(param);
        }
        functionDeclaration->signature = signature;
    }


    type::Type*
    Checker::resolveTypeAssignment(const TypeAssignment* type)
    {
        return new Type(type->type, type->pointers.size());
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
        ast::Type* expressionType = checkExpression(returnStatement->expression, functionDeclaration->signature->type);
        checkTypeAssignability(functionDeclaration->signature->type, expressionType, returnStatement->expression);
        returnStatement->expectedType = functionDeclaration->signature->type;
    }


    bool
    Checker::checkTypeAssignability(const Type* placeholder, const Type* target, const Syntax* targetSyntax)
    {
        if (placeholder->kind == TypeKind::Any || target->kind == TypeKind::Any)
        {
            return true;
        }
        if (isBooleanType(placeholder) && isBooleanType(target))
        {
            return true;
        }
        if (isIntegralType(placeholder) && isIntegralType(target))
        {
            return checkIntegralTypeAndBoundsAssignability(placeholder, target, targetSyntax);
        }
        addError<error::TypeAssignabilityError>(targetSyntax, placeholder, target);
        return false;
    }


    bool
    Checker::checkIntegralTypeAndBoundsAssignability(const Type* placeholder, const Type* target, const Syntax* targetSyntax)
    {
        if (checkIntegralAssignability(placeholder, target, targetSyntax))
        {
            return checkIntegralBounds(placeholder, target, targetSyntax);
        }
        return false;
    }


    bool
    Checker::checkIntegralAssignability(const Type* placeholder, const Type* target, const Syntax* targetSyntax)
    {
        Sign placeholderSign = placeholder->sign();
        Sign targetSign = target->sign();
        if (placeholderSign != targetSign)
        {
            addError<error::SignMismatchAssignabilityError>(targetSyntax, placeholder, target);
            return false;
        }
        if (placeholderSign == Sign::Unsigned)
        {
            switch (placeholder->kind) {
                case TypeKind::U32:
                    switch (target->kind)
                    {
                        case TypeKind::U64:
                            addError<error::IntegralTypeMismatchAssignabilityError>(targetSyntax, placeholder, target);
                            return false;
                    }
                    break;
                case TypeKind::U16:
                    switch (target->kind)
                    {
                        case TypeKind::U64:
                        case TypeKind::U32:
                            addError<error::IntegralTypeMismatchAssignabilityError>(targetSyntax, placeholder, target);
                            return false;
                    }
                    break;
                case TypeKind::U8:
                    switch (target->kind)
                    {
                        case TypeKind::U64:
                        case TypeKind::U32:
                        case TypeKind::U16:
                            addError<error::IntegralTypeMismatchAssignabilityError>(targetSyntax, placeholder, target);
                            return false;
                    }
                    break;
            }
        }
        else {
            switch (placeholder->kind) {
                case TypeKind::S32:
                    switch (target->kind)
                    {
                        case TypeKind::S64:
                            addError<error::IntegralTypeMismatchAssignabilityError>(targetSyntax, placeholder, target);
                            return false;
                    }
                    break;
                case TypeKind::S16:
                    switch (target->kind)
                    {
                        case TypeKind::S64:
                        case TypeKind::S32:
                            addError<error::IntegralTypeMismatchAssignabilityError>(targetSyntax, placeholder, target);
                            return false;
                    }
                    break;
                case TypeKind::S8:
                    switch (target->kind)
                    {
                        case TypeKind::S64:
                        case TypeKind::S32:
                        case TypeKind::S16:
                            addError<error::IntegralTypeMismatchAssignabilityError>(targetSyntax, placeholder, target);
                            return false;
                    }
                    break;
            }
        }
        return true;
    }


    bool
    Checker::checkIntegralBounds(const Type* placeholder, const Type* target, const Syntax* targetSyntax)
    {
        if (target->maxValue > placeholder->maxValue)
        {
            addError<error::IntegralExpressionOverflowError>(targetSyntax, placeholder, target);
            return false;
        }
        if (target->minValue < placeholder->minValue)
        {
            addError<error::IntegralExpressionUnderflowError>(targetSyntax, placeholder, target);
            return false;
        }
        return true;
    }


    Type*
    Checker::checkBooleanLiteral(BooleanLiteral* literal)
    {
        return new Type(TypeKind::Bool);
    }


    bool
    Checker::isBooleanType(const Type* type) const
    {
        return type->kind == TypeKind::Bool;
    }


    bool
    Checker::isAny(const Type* type) const
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


    Type*
    Checker::createOperatingType(const Type* resultingType, const Type* left, const Type* right)
    {
        Int128 maxValue = Int128::max(resultingType->maxValue, left->maxValue, right->maxValue);
        Int128 minValue = Int128::min(resultingType->minValue, left->minValue, right->minValue);
        if (left->sign() == right->sign())
        {
            if (left->sign() == Sign::Signed && right->sign() == Sign::Signed)
            {
                return new Type(getDefaultSignedTypeFromBounds(minValue, maxValue), minValue, maxValue);
            }
            else
            {
                return new Type(getDefaultUnsignedTypeFromBounds(minValue, maxValue), minValue, maxValue);
            }
        }
        return new Type(getDefaultTypeFromBounds(minValue, maxValue), minValue, maxValue);
    }


    TypeKind
    Checker::getMaxSignedType(Type* type1, Type* type2)
    {
        if (type1->kind >= type2->kind)
        {
            return type1->kind;
        }
        return type2->kind;
    }

    TypeKind
    Checker::getMaxUnsignedType(Type* type1, Type* type2)
    {
        if (type1->kind >= type2->kind)
        {
            return type1->kind;
        }
        return type2->kind;
    }


}
