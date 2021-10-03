#include "Checker.h"


namespace elet::domain::compiler::ast
{


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
        case SyntaxKind::BinaryExpression:
            return checkBinaryExpression(reinterpret_cast<BinaryExpression*>(expression));
        case SyntaxKind::PropertyExpression:
            return checkPropertyExpression(reinterpret_cast<PropertyExpression*>(expression));
        case SyntaxKind::IntegerLiteral:
            return resolveTypeOfIntegerLiteral(reinterpret_cast<IntegerLiteral*>(expression));
        default:;
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
        switch (statement->kind)
        {
            case SyntaxKind::CallExpression:
                checkCallExpression(reinterpret_cast<CallExpression*>(statement));
                break;
            case SyntaxKind::VariableDeclaration:
                checkVariableDeclaration(reinterpret_cast<VariableDeclaration*>(statement));
                break;
            case SyntaxKind::ReturnStatement:
                checkExpression(reinterpret_cast<ReturnStatement*>(statement)->expression);
                break;
            default:;
        }
    }
}


void
Checker::checkVariableDeclaration(VariableDeclaration* variable)
{
    variable->resolvedType = checkExpression(variable->expression);
}


Type*
Checker::resolveTypeOfIntegerLiteral(IntegerLiteral* integerLiteral)
{
    return integerLiteral->resolvedType = getTypeFromIntegerLiteral(integerLiteral);
}


Type*
Checker::checkBinaryExpression(BinaryExpression* binaryExpression)
{
    Type* leftType = checkExpression(binaryExpression->left);
    Type* rightType = checkExpression(binaryExpression->right);
    Type* commonType = getCommonRealType(leftType, rightType);
    binaryExpression->resolvedType = commonType;
    return commonType;
}


Type*
Checker::getTypeFromIntegerLiteral(IntegerLiteral* integerLiteral)
{
    if (integerLiteral->value <= TypeSizeBounds::Int32Max)
    {
        return new Type(TypeKind::S32);
    }
    else if (integerLiteral->value <= TypeSizeBounds::Int64Max)
    {
        return new Type(TypeKind::S64);
    }

    // TODO: Fix this function to correspond to integer literal sepc of cpp
    throw std::runtime_error("Cannot convert digits immediateValue.");
}


Type*
Checker::getCommonRealType(Type* type1, Type* type2)
{
    // See ImplicitIntegerConversion.md
    if (type1->kind == type2->kind)
    {
        return type1;
    }
    Signedness sign1 = type1->sign();
    Signedness sign2 = type2->sign();
    if (sign1 == sign2)
    {
        return getMaxType(type1, type2);
    }
    Type* signedType;
    Type* unsignedType;
    if (sign1 == Signedness::Signed)
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
            return new Type(TypeKind::U8);
        case TypeKind::S16:
            return new Type(TypeKind::U16);
        case TypeKind::S32:
            return new Type(TypeKind::U32);
        case TypeKind::S64:
            return new Type(TypeKind::U64);
        default:
            throw std::runtime_error("Unknown signed type to fetch unsigned counter part.");
    }
}


uint64_t
Checker::getMaxTypeDomain(Type* type)
{
    switch (type->kind)
    {
        case TypeKind::U8:
            return TypeSizeBounds::UInt8Max;
        case TypeKind::U16:
            return TypeSizeBounds::UInt16Max;
        case TypeKind::U32:
            return TypeSizeBounds::UInt32Max;
        case TypeKind::U64:
            return TypeSizeBounds::UInt64Max;
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
        bool missingSignature = false;
        for (const auto& signature : domain->implements->signatures)
        {
            FunctionDeclaration* functionDeclaration = getDeclarationFromSignature(signature, domain);
            if (!functionDeclaration)
            {
                missingSignature = true;
                continue;
            }
            checkFunctionSignature(functionDeclaration->signature, signature);
            if (signature->isStartFunction)
            {
                functionDeclaration->signature->isStartFunction = true;
                startFunctions.add(functionDeclaration);
            }
        }
        if (missingSignature)
        {

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
    return target->kind == source->kind;
}


void
Checker::checkFunctionSignature(const type::Signature* target, const type::Signature* source)
{
    if (target->name != source->name)
    {
        addDiagnostic(new Diagnostic("Mismatching names."));
    }
    if (target->parameters.size() != source->parameters.size())
    {
        addDiagnostic(new Diagnostic("Mismatching index of parameters."));
    }
    for (unsigned int i = 0; target->parameters.size(); i++)
    {
        auto targetParameter = target->parameters[i];
        auto sourceParameter = source->parameters[i];
        if (!isTypeEqualToType(targetParameter->type, sourceParameter->type))
        {
            addDiagnostic(new Diagnostic("Mismatching index of parameters."));
        }
    }
    if (!isTypeEqualToType(target->type, source->type))
    {
        addDiagnostic(new Diagnostic("some"));
    }
}

void
Checker::addDiagnostic(Diagnostic* diagnostic)
{
    _diagnostics.add(diagnostic);
}


}
