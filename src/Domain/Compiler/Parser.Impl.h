#ifndef ELET_PARSER_IMPL_H
#define ELET_PARSER_IMPL_H


#include "Parser.h"


namespace elet::domain::compiler::ast
{
    template<typename T>
    T*
    Parser::createSyntax(SyntaxKind kind)
    {
        T* syntax = new T();
        syntax->kind = kind;
        syntax->start = _scanner->getStartPosition();
        return syntax;
    }


    template<typename T>
    void
    Parser::finishSyntax(T* syntax)
    {
        syntax->end = _scanner->getPositionAddress();
    }


    template<typename T>
    void
    Parser::finishDeclaration(T* declaration)
    {
        addSymbol(declaration);
        finishSyntax(declaration);
    }


    template<typename T>
    void
    Parser::finishTypeDeclaration(T* declaration)
    {
        addTypeSymbol(declaration);
        finishSyntax(declaration);
    }




    template<typename T>
    T*
    Parser::createDeclaration(const SyntaxKind kind)
    {
        auto syntax = createSyntax<T>(kind);
        syntax->labels = LABEL__DECLARATION;
        syntax->offset = 0;
        syntax->domain = _currentDomain;
        return syntax;
    }


    template<typename T>
    T*
    Parser::createTypeDeclaration(SyntaxKind kind)
    {
        T* syntax = createSyntax<T>(kind);
        syntax->labels = LABEL__TYPE_DECLARATION;
        syntax->offset = 0;
        return syntax;
    }


    template<typename T>
    T*
    Parser::createBlock(SyntaxKind kind)
    {
        T* syntax = createSyntax<T>(kind);
        syntax->labels = LABEL__BLOCK;
        return syntax;
    }


    template<typename T>
    T*
    Parser::createNamedExpression(SyntaxKind kind)
    {
        T* syntax = createSyntax<T>(kind);
        syntax->labels = LABEL__NAMED_EXPRESSION;
        syntax->name = createName();
        return syntax;
    }
}


#endif //ELET_PARSER_IMPL_H
