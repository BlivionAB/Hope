#include "Checker.h"


namespace elet::domain::compiler
{


Checker::Checker(const Binder* _binder):
    _binder(_binder)
{

}


void
Checker::checkTopLevelDeclaration(ast::Declaration* declaration)
{
    _sourceFile = declaration->sourceFile;
    switch (declaration->kind)
    {
        case ast::SyntaxKind::DomainDeclaration:
            checkDomainDeclaration(reinterpret_cast<const ast::DomainDeclaration*>(declaration));
            break;
        case ast::SyntaxKind::FunctionDeclaration:
            checkFunctionDeclaration(reinterpret_cast<ast::FunctionDeclaration*>(declaration));
            break;
        default:;
    }
}


void
Checker::checkExpression(ast::Expression* expression)
{
    switch (expression->kind)
    {
        case ast::SyntaxKind::BinaryExpression:
            checkExpression(reinterpret_cast<ast::BinaryExpression*>(expression)->left);
            checkExpression(reinterpret_cast<ast::BinaryExpression*>(expression)->right);
            break;
        case ast::SyntaxKind::PropertyExpression:
            checkPropertyExpression(reinterpret_cast<ast::PropertyExpression*>(expression));
            break;
        case ast::SyntaxKind::IntegerLiteral:
            resolveTypeFromExpression(reinterpret_cast<ast::IntegerLiteral*>(expression));
            break;
        default:;
    }
}


void
Checker::checkPropertyExpression(ast::PropertyExpression* propertyExpression)
{
    ast::Type* type = resolveTypeFromDeclaration(propertyExpression->referenceDeclaration);

}


ast::Type*
Checker::resolveTypeFromDeclaration(ast::Declaration* declaration)
{
    if (declaration->resolvedType)
    {
        return declaration->resolvedType;
    }
//    switch (declaration->kind)
//    {
//        case ast::SyntaxKind::ParameterDeclaration:
//
//    }
}


void
Checker::checkFunctionDeclaration(ast::FunctionDeclaration* functionDeclaration)
{
    resolveTypeFromFunctionDeclaration(functionDeclaration);
    for (ast::Syntax* statement : functionDeclaration->body->statements)
    {
        switch (statement->kind)
        {
            case ast::SyntaxKind::CallExpression:
                checkCallExpression(reinterpret_cast<ast::CallExpression*>(statement));
                break;
            case ast::SyntaxKind::VariableDeclaration:
                checkVariableDeclaration(reinterpret_cast<ast::VariableDeclaration*>(statement));
                break;
            case ast::SyntaxKind::ReturnStatement:
                checkExpression(reinterpret_cast<ast::ReturnStatement*>(statement)->expression);
                break;
            default:;
        }
    }
}


void
Checker::checkVariableDeclaration(ast::VariableDeclaration* variable)
{
    if (!variable->type)
    {
        variable->resolvedType = inferTypeFromExpression(variable->expression);
    }
}


ast::Type*
Checker::inferTypeFromExpression(ast::Expression* expression)
{
    switch (expression->kind)
    {
        case ast::SyntaxKind::BinaryExpression:
        {
            ast::BinaryExpression* binaryExpression = reinterpret_cast<ast::BinaryExpression*>(expression);
            ast::Type* leftType = inferTypeFromExpression(binaryExpression->left);
            ast::Type* rightType = inferTypeFromExpression(binaryExpression->right);
            ast::Type* commonType = getCommonRealType(leftType, rightType);
            binaryExpression->resolvedType = commonType;
            return commonType;
        }
        case ast::SyntaxKind::IntegerLiteral:
        {
            ast::IntegerLiteral* integerLiteral = reinterpret_cast<ast::IntegerLiteral*>(expression);
            ast::Type* type = getTypeFromIntegerLiteral(integerLiteral);
            integerLiteral->resolvedType = type;
            return type;
        }
    }
}


ast::Type*
Checker::getTypeFromIntegerLiteral(ast::IntegerLiteral* integerLiteral)
{
    if (integerLiteral->value <= ast::TypeSizeBounds::Int32Max)
    {
        return new ast::Type(ast::TypeKind::Int32);
    }
    throw std::runtime_error("Cannot convert integer value.");
}


ast::Type*
Checker::getCommonRealType(ast::Type* type1, ast::Type* type2)
{
    // See ImplicitIntegerConversion.md
    if (type1->kind == type2->kind)
    {
        return type1;
    }
    ast::Signedness sign1 = type1->sign();
    ast::Signedness sign2 = type2->sign();
    if (sign1 == sign2)
    {
        return getMaxType(type1, type2);
    }
    ast::Type* signedType;
    ast::Type* unsignedType;
    if (sign1 == ast::Signedness::Signed)
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


ast::Type*
Checker::getUnsignedCounterPart(ast::Type* signedType)
{
    switch (signedType->kind)
    {
        case ast::TypeKind::Int8:
            return new ast::Type(ast::TypeKind::UInt8);
        case ast::TypeKind::Int16:
            return new ast::Type(ast::TypeKind::UInt16);
        case ast::TypeKind::Int32:
            return new ast::Type(ast::TypeKind::UInt32);
        case ast::TypeKind::Int64:
            return new ast::Type(ast::TypeKind::UInt64);
        default:
            throw std::runtime_error("Unknown signed type to fetch unsigned counter part.");
    }
}


uint64_t
Checker::getMaxTypeDomain(ast::Type* type)
{
    switch (type->kind)
    {
        case ast::TypeKind::UInt8:
            return ast::TypeSizeBounds::UInt8Max;
        case ast::TypeKind::UInt16:
            return ast::TypeSizeBounds::UInt16Max;
        case ast::TypeKind::UInt32:
            return ast::TypeSizeBounds::UInt32Max;
        case ast::TypeKind::UInt64:
            return ast::TypeSizeBounds::UInt64Max;
        default:
            throw std::runtime_error("Cannot get max domain of type");
    }
}


ast::Type*
Checker::getMaxType(ast::Type* type1, ast::Type* type2)
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
Checker::getConversionRanking(ast::Type* type)
{
    return static_cast<unsigned int>(type->kind);
}


void
Checker::checkCallExpression(const ast::CallExpression* callExpression)
{
    for (const auto& argument : callExpression->argumentList->arguments)
    {
        ast::Type* type = resolveTypeFromExpression(argument);
    }
}



ast::Type*
Checker::resolveTypeFromExpression(ast::Expression* expression)
{
    if (expression->labels & LABEL__NAMED_EXPRESSION)
    {
        return resolveTypeFromDeclaration(reinterpret_cast<ast::NamedExpression*>(expression)->referenceDeclaration);
    }
}


void
Checker::checkUsingStatement(const ast::UsingStatement* usingStatement)
{

}


void
Checker::checkDomainDeclaration(const ast::DomainDeclaration* domain)
{
    for (const auto& decl : domain->block->declarations)
    {
        if (decl->kind == ast::SyntaxKind::FunctionDeclaration)
        {
            checkFunctionDeclaration(reinterpret_cast<ast::FunctionDeclaration*>(decl));
        }
    }
    if (domain->implements)
    {
        bool missingSignature = false;
        for (const auto& signature : domain->implements->signatures)
        {
            ast::FunctionDeclaration* functionDeclaration = getDeclarationFromSignature(signature, domain);
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

ast::FunctionDeclaration*
Checker::getDeclarationFromSignature(ast::type::Signature* const& signature, const ast::DomainDeclaration* domain) const
{
    for (const auto& decl : domain->block->declarations)
    {
        if (decl->kind == ast::SyntaxKind::FunctionDeclaration)
        {
            auto functionDeclaration = reinterpret_cast<ast::FunctionDeclaration*>(decl);
            if (functionDeclaration->signature->name == signature->name)
            {
                return functionDeclaration;
            }
        }
    }
    return nullptr;
}


void
Checker::resolveTypeFromFunctionDeclaration(ast::FunctionDeclaration* functionDeclaration)
{
    if (functionDeclaration->signature)
    {
        return;
    }
    auto signature = new ast::type::Signature();
    signature->name = functionDeclaration->name->name;
    signature->type = new ast::type::Type(functionDeclaration->type->type, functionDeclaration->type->pointers.size());
    for (const auto& p : functionDeclaration->parameterList->parameters)
    {
        const auto param = new ast::type::Parameter();
        param->name = p->name->name;
        p->resolvedType = param->type = new ast::type::Type(p->type->type, p->type->pointers.size());
        signature->parameters.add(param);
    }
    functionDeclaration->signature = signature;
}


bool
Checker::isTypeEqualToType(const ast::Type* target, const ast::Type* source)
{
    return target->kind == source->kind;
}


void
Checker::checkFunctionSignature(const ast::type::Signature* target, const ast::type::Signature* source)
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
