#include "Checker.h"


namespace elet::domain::compiler
{


Checker::Checker(const Binder* _binder):
    _binder(_binder)
{

}


void
Checker::checkTopLevelDeclaration(const ast::Declaration* declaration)
{
    _sourceFile = declaration->sourceFile;
    switch (declaration->kind)
    {
        case ast::SyntaxKind::DomainDeclaration:
            checkDomainDeclaration(reinterpret_cast<const ast::DomainDeclaration*>(declaration));
            break;
        case ast::SyntaxKind::FunctionDeclaration:
            checkFunctionDeclaration(reinterpret_cast<const ast::FunctionDeclaration*>(declaration));
            break;
        default:;
    }
}


void
Checker::checkFunctionDeclaration(const ast::FunctionDeclaration *functionDeclaration)
{
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
    auto result = _sourceFile->symbols.equal_range(callExpression->name->name);
    for (auto it = result.first; it != result.second; ++it)
    {
        auto functionDeclaration = reinterpret_cast<ast::FunctionDeclaration*>(it->second);
        unsigned int index = 0;
        unsigned int maxIndex = callExpression->argumentList->arguments.size();
        for (ast::ParameterDeclaration* parameter : functionDeclaration->parameterList->parameters)
        {
            if (index < maxIndex)
            {
                const ast::Expression* expression = callExpression->argumentList->arguments[index];
                Type assignmentType = getTypeFromExpression(expression);
                Type placeholderType = getTypeFromTypeAssignment(parameter->type);
                if (!isTypeAssignableToPlaceholder(assignmentType, placeholderType))
                {
                    std::cout << "not assignable" << std::endl;
                }
            }
            index++;
        }
    }
}


Type
Checker::getTypeFromExpression(const ast::Expression* expression)
{
    switch (expression->kind)
    {
        case ast::SyntaxKind::StringLiteral:
        {
            return Type(TYPE_CHAR, 1);
        }
        default:;
    }
}

bool
Checker::isTypeAssignableToPlaceholder(Type &assignment, Type& placeholder)
{
    switch (placeholder.kind)
    {
        case TYPE_CHAR:
            if (assignment.kind != TYPE_CHAR)
            {
                return false;
            }
            if (assignment.pointers != placeholder.pointers)
            {
                return false;
            }
        case TYPE_CUSTOM:
            break;
    }
    return true;
}


Type
Checker::getTypeFromTypeAssignment(ast::TypeAssignment* typeAssignment)
{
    Type result;
    result.pointers = typeAssignment->pointers.size();
    switch (typeAssignment->type)
    {
        case ast::TypeKind::Char:
            result.kind = TYPE_CHAR;
            break;
        case ast::TypeKind::Custom:
            result.kind = TYPE_CUSTOM;
            break;
        default:;
    }
    return result;
}

void
Checker::checkUsingStatement(const ast::UsingStatement* usingStatement)
{

}

void
Checker::checkDomainDeclaration(const ast::DomainDeclaration* domain)
{
//    domain->implements->properties
}


}
