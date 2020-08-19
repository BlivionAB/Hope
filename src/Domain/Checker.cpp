#include "Checker.h"


namespace elet::domain::compiler
{


void
Checker::checkTopLevelDeclaration(const ast::Declaration *declaration)
{
    _sourceFile = declaration->sourceFile;
    switch (declaration->kind)
    {
        case ast::SyntaxKind::FunctionDeclaration:
            checkFunctionDeclaration(reinterpret_cast<const ast::FunctionDeclaration*>(declaration));
            break;
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
        }
    }
}

void
Checker::checkCallExpression(const ast::CallExpression* callExpression)
{
    auto result = _sourceFile->names.find(callExpression->name->name);
    if (result != _sourceFile->names.end())
    {

    }
    result->second;
}


}
