#include "Binder.h"
#include "Exceptions.h"
#include <variant>

namespace elet::domain::compiler::ast
{


thread_local
FunctionDeclaration*
Binder::_currentFunctionDeclaration = nullptr;



DomainDeclaration*
Binder::startDomainDeclaration = nullptr;


Binder::Binder():
    _domainDeclarationMap(nullptr)
{

}


void
Binder::performWork(BindingWork& work, const DomainDeclarationMap* domainDeclarationMap)
{
    _domainDeclarationMap = domainDeclarationMap;

    switch (work.declaration->kind)
    {
        case SyntaxKind::DomainDeclaration:
            bindDomainDeclaration(reinterpret_cast<DomainDeclaration*>(work.declaration));
            break;
        case SyntaxKind::UsingStatement:
            bindUsingStatement(reinterpret_cast<UsingStatement*>(work.declaration));
            break;
        case SyntaxKind::FunctionDeclaration:
            bindFunctionDeclaration(reinterpret_cast<FunctionDeclaration*>(work.declaration));
            break;
        default:
            throw UnknownDeclaration();
    }
}


void
Binder::bindDomainDeclaration(DomainDeclaration* domain)
{
    if (domain->implementsClause)
    {
        if (domain->implementsClause->name == "IConsoleApplication")
        {
            startDomainDeclaration = domain;
            static auto consoleAppInterface = new type::Interface();
            static auto onApplicationStart = new type::Signature();
            onApplicationStart->isStartFunction = true;
            onApplicationStart->name = "OnApplicationStart";
            onApplicationStart->type = new type::Type(type::TypeKind::Void);
            consoleAppInterface->signatures.add(onApplicationStart);
            domain->implements = consoleAppInterface;
        }
    }
    for (const auto& decl : domain->block->declarations)
    {
        switch (decl->kind)
        {
            case SyntaxKind::UsingStatement:
                bindUsingStatement(reinterpret_cast<UsingStatement*>(decl));
                break;
            case SyntaxKind::FunctionDeclaration:
                bindFunctionDeclaration(reinterpret_cast<FunctionDeclaration*>(decl));
                break;
            default:;
        }
    }
}


void
Binder::bindUsingStatement(UsingStatement* usingStatement)
{
    auto declarationMap = getDomainDeclarations(usingStatement->domains, 0, _domainDeclarationMap);
    for (const auto& name : usingStatement->usageClause->names)
    {
        auto it = declarationMap->find(name->name);
        if (it != declarationMap->end())
        {
            usingStatement->domain->usages.insert({ name->name, reinterpret_cast<Declaration*>(it->second) });
        }
    }
}


std::map<Utf8StringView, Declaration*>*
Binder::getDomainDeclarations(const List<Name*>& domains, unsigned int domainIndex, const DomainDeclarationMap* domainDeclarationMap)
{
    auto domain = domains[domainIndex];
    auto result = domainDeclarationMap->find(domain->name);
    if (result != domainDeclarationMap->end())
    {
        if (auto declarationMap = std::get_if<std::map<Utf8StringView, Declaration*>*>(&result->second))
        {
            return *declarationMap;
        }
        else
        {
            return getDomainDeclarations(domains, domainIndex + 1, reinterpret_cast<DomainDeclarationMap*>(std::get<void*>(result->second)));
        }
    }

    throw UndefinedDomainAccessError();
}


void
Binder::bindFunctionDeclaration(FunctionDeclaration* declaration)
{
    _currentFunctionDeclaration = declaration;
    for (ParameterDeclaration* parameterDeclaration : declaration->parameterList->parameters)
    {
        declaration->body->symbols[parameterDeclaration->name->name] = parameterDeclaration;
    }
    bindStatementBlock(declaration->body);
}


void
Binder::bindStatementBlock(StatementBlock* block)
{
    for (Syntax* statement : block->statements)
    {
        switch (statement->kind)
        {
            case SyntaxKind::VariableDeclaration:
                bindVariableDeclaration(reinterpret_cast<VariableDeclaration*>(statement));
                break;
            case SyntaxKind::CallExpression:
                bindCallExpression(reinterpret_cast<CallExpression*>(statement));
                break;
            case SyntaxKind::IfStatement:
                bindIfStatement(reinterpret_cast<IfStatement*>(statement));
                break;
            case SyntaxKind::ReturnStatement:
            {
                ReturnStatement* returnStatement = reinterpret_cast<ReturnStatement*>(statement);
                bindExpression(returnStatement->expression);
                break;
            }
            default:
                throw UnknownBindingStatement();
        }
    }
}

void
Binder::bindIfStatement(const IfStatement* ifStatement)
{
    bindExpression(ifStatement->condition);
    bindStatementBlock(ifStatement->body);
}


void
Binder::bindVariableDeclaration(VariableDeclaration* variableDeclaration)
{
    _currentFunctionDeclaration->body->symbols[variableDeclaration->name->name] = variableDeclaration;
    bindExpression(variableDeclaration->expression);
}


void
Binder::bindCallExpression(CallExpression* callExpression)
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
Binder::bindExpression(Expression* expression)
{
    switch (expression->kind)
    {
        case SyntaxKind::IntegerLiteral:
            break;
        case SyntaxKind::BinaryExpression:
            bindBinaryExpression(reinterpret_cast<BinaryExpression*>(expression));
            break;
        case SyntaxKind::PropertyExpression:
            bindPropertyExpression(reinterpret_cast<PropertyExpression*>(expression));
            break;
        default:;
    }
}


void
Binder::bindBinaryExpression(BinaryExpression* binaryExpression)
{
    bindExpression(binaryExpression->left);
    bindExpression(binaryExpression->right);
}


void
Binder::bindPropertyExpression(PropertyExpression* property)
{
    auto declaration = _currentFunctionDeclaration->body->symbols[property->name->name];
    if (declaration)
    {
        property->referenceDeclaration = declaration;
    }
}


}