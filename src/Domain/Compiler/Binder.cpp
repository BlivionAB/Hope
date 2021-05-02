#include "Binder.h"
#include "Exceptions.h"
#include <variant>

namespace elet::domain::compiler
{


thread_local
ast::FunctionDeclaration*
Binder::_currentFunctionDeclaration = nullptr;


Binder::Binder():
    _domainDeclarationMap(nullptr)
{

}


void
Binder::performWork(BindingWork& work, const ast::DomainDeclarationMap* domainDeclarationMap)
{
    _domainDeclarationMap = domainDeclarationMap;

    switch (work.declaration->kind)
    {
        case ast::SyntaxKind::UsingStatement:
            bindUsingStatement(reinterpret_cast<ast::UsingStatement*>(work.declaration));
            break;
        case ast::SyntaxKind::FunctionDeclaration:
            bindFunction(reinterpret_cast<ast::FunctionDeclaration*>(work.declaration));
            break;
        default:
            throw UnknownDeclaration();
    }
}


void
Binder::bindUsingStatement(ast::UsingStatement* usingStatement)
{
    auto declarationMap = getDomainDeclarations(usingStatement->domains, 0, _domainDeclarationMap);
    for (const auto& name : usingStatement->usageClause->names)
    {
        auto it = declarationMap->find(name->name);
        if (it != declarationMap->end())
        {
            usingStatement->domain->usages.insert({ name->name, reinterpret_cast<ast::Declaration*>(it->second) });
        }
    }
}


std::map<Utf8StringView, ast::Declaration*>*
Binder::getDomainDeclarations(const List<ast::Name*>& domains, unsigned int domainIndex, const ast::DomainDeclarationMap* domainDeclarationMap)
{
    auto domain = domains[domainIndex];
    auto result = domainDeclarationMap->find(domain->name);
    if (result != domainDeclarationMap->end())
    {
        if (auto declarationMap = std::get_if<std::map<Utf8StringView, ast::Declaration*>*>(&result->second))
        {
            return *declarationMap;
        }
        else
        {
            return getDomainDeclarations(domains, domainIndex + 1, reinterpret_cast<ast::DomainDeclarationMap*>(std::get<void*>(result->second)));
        }
    }

    throw UndefinedDomainAccessError();
}


void
Binder::bindFunction(ast::FunctionDeclaration* declaration)
{
    _currentFunctionDeclaration = declaration;
    for (ast::ParameterDeclaration* parameterDeclaration : declaration->parameterList->parameters)
    {
        declaration->body->symbols[parameterDeclaration->name->name] = parameterDeclaration;
    }
    for (ast::Syntax* statement : declaration->body->statements)
    {
        switch (statement->kind)
        {
            case ast::SyntaxKind::CallExpression:
                bindCallExpression(reinterpret_cast<ast::CallExpression*>(statement));
                break;
            default:
                throw UnknownBindingStatement();
        }
    }
}


void
Binder::bindCallExpression(ast::CallExpression* callExpression)
{
    for (auto expr : callExpression->argumentList->arguments)
    {
        bindExpression(expr);
    }
    const auto& name = callExpression->name->name;
    const auto& domain = _currentFunctionDeclaration->domain;
    const auto& usages = domain->usages;
    const auto& usagesIt = usages.find(name);
    if (usagesIt != usages.end())
    {
        callExpression->referenceDeclaration = usagesIt->second;
        return;
    }
    const auto& symbols = domain->block->symbols;
    const auto& domainSymbolsIt = symbols.find(name);
    if (domainSymbolsIt != symbols.end())
    {
        callExpression->referenceDeclaration = domainSymbolsIt->second;
        return;
    }
}


void
Binder::bindExpression(ast::Expression* expression)
{
    switch (expression->kind)
    {
        case ast::SyntaxKind::PropertyExpression:
            bindPropertyExpression(reinterpret_cast<ast::PropertyExpression*>(expression));
            break;
        default:;
    }
}


void
Binder::bindPropertyExpression(ast::PropertyExpression* property)
{
    auto declaration = _currentFunctionDeclaration->body->symbols[property->name->name];
    if (declaration)
    {
        property->referenceDeclaration = declaration;
    }
}


}