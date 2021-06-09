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
        case ast::SyntaxKind::PropertyExpression:
            checkPropertyExpression(reinterpret_cast<ast::PropertyExpression*>(expression));
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
    for (const ast::Syntax* statement : functionDeclaration->body->statements)
    {
        switch (statement->kind)
        {
            case ast::SyntaxKind::CallExpression:
                checkCallExpression(reinterpret_cast<const ast::CallExpression*>(statement));
                break;
            default:;
        }
    }
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
    signature->type = new ast::type::Type();
    signature->type->kind = functionDeclaration->type->type;
    signature->type->pointers = functionDeclaration->type->pointers.size();
    for (const auto& p : functionDeclaration->parameterList->parameters)
    {
        const auto param = new ast::type::Parameter();
        param->name = p->name->name;
        p->resolvedType = param->type = new ast::type::Type();
        param->type->kind = p->type->type;
        param->type->pointers = p->type->pointers.size();
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
        addDiagnostic(new Diagnostic("Mismatching number of parameters."));
    }
    for (unsigned int i = 0; target->parameters.size(); i++)
    {
        auto targetParameter = target->parameters[i];
        auto sourceParameter = source->parameters[i];
        if (!isTypeEqualToType(targetParameter->type, sourceParameter->type))
        {
            addDiagnostic(new Diagnostic("Mismatching number of parameters."));
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
