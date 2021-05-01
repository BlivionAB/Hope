#ifndef ELET_SYNTAX_DEFINITION_H
#define ELET_SYNTAX_DEFINITION_H

#include "Syntax.h"
#include "Syntax.Block.h"

namespace elet::domain::compiler::ast
{
    struct StatementBlock;
    struct DomainDeclaration;

    struct Declaration : Syntax
    {
        Name*
        name;

        std::size_t
        offset;

        Symbol*
        symbol;

        SourceFile*
        sourceFile;

        AccessibilityType
        __accessability;

        std::size_t
        __size;

        const DomainDeclaration*
        __domain;
    };


    struct DomainDeclaration : Declaration
    {
        List<Name*>
        names;

        DomainType
        type;

        DeclarationBlock*
        block;

        Name*
        implementsClause;

        std::map<Utf8StringView, ast::Declaration*>
        usages;
    };


    struct ConstructorDeclaration : Declaration
    {
        ParameterDeclarationList*
        parameterList;
    };


    struct PropertyDeclaration : Declaration
    {
        ParameterDeclarationList*
        parameters;

        TypeAssignment*
        type;

        bool
        isStatic;
    };


    struct InterfaceDeclaration : Declaration
    {
        List<PropertyDeclaration*>
        properties;
    };


    struct ParameterDeclarationList : Syntax
    {
        List<ParameterDeclaration*>
        parameters;
    };


    struct ClassDeclaration : Declaration
    {
        List<PropertyDeclaration*>
        properties;

        ConstructorDeclaration*
        constructor;
    };


    struct EnumDeclaration : Declaration
    {
        List<Name*>
        values;
    };


    struct LocalDeclaration : Declaration
    {
        std::size_t
        baseAddressOffset;
    };


    struct ParameterDeclaration : LocalDeclaration
    {
        TypeAssignment*
        type;

        Utf8String
        display;
    };


    struct VariableDeclaration : LocalDeclaration
    {
        TypeAssignment*
        type;

        Punctuation*
        equals;

        Expression*
        expression;
    };


    struct ConstructorImplementation : Declaration
    {
        ParameterDeclarationList*
        parameterList;

        List<CallExpression*>*
        initializationList;

        StatementBlock*
        body;
    };



    struct FunctionDeclaration : Declaration
    {
        ParameterDeclarationList*
        parameterList;

        TypeAssignment*
        type;

        Name*
        from;

        StatementBlock*
        body;

        // Used for deciding
        List<output::Parameter*>
        instructionParameters;
    };
}


#endif //ELET_SYNTAX_DEFINITION_H
