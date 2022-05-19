#ifndef ELET_SYNTAX_DECLARATIONS_H
#define ELET_SYNTAX_DECLARATIONS_H

#include "Syntax.h"
#include "Syntax.Block.h"
#include "Syntax.Type.h"

namespace elet::domain::compiler::instruction::output
{
    struct StoreRegisterInstruction;
    struct FunctionRoutine;
    struct MemoryAllocation;
}


namespace elet::domain::compiler::ast
{
    using namespace elet::domain::compiler::instruction;
    struct StatementBlock;
    struct DomainDeclaration;
    struct InterfaceDeclaration;

    namespace type
    {
        struct Interface;
        struct Signature;
    }

    namespace Decoration
    {
        struct ControlFlowStatement : Syntax
        {
        };


        struct IfStatement : ControlFlowStatement
        {

        };


        struct Declaration : Syntax
        {
            ControlFlowStatement*
            controlFlowStatement;
        };
    };


    struct Declaration : Syntax
    {
        Name*
        name;

        size_t
        offset;

        Symbol*
        symbol;

        SourceFile*
        sourceFile;

        AccessibilityType
        accessability;

        const DomainDeclaration*
        domain;

        Type*
        declarationType;

        Type*
        expressionType;

        output::MemoryAllocation*
        referenceInstruction;

        List<Decoration::Declaration*>
        decorations;
    };


    template<typename T>
    struct SyntaxList : public List<T>, Syntax
    {

    };


    struct DomainDeclaration : Declaration
    {
        SyntaxList<Name*>*
        names;

        DomainType
        type;

        DeclarationBlock*
        block;

        Name*
        implementsClause;

        std::map<Utf8StringView, ast::Declaration*>
        usages;

        type::Interface*
        implements;
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

        instruction::output::StoreRegisterInstruction*
        outputDeclaration;
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

        // Used for checker
        type::Signature*
        signature;

        // Used for deciding
        output::FunctionRoutine*
        routine;

        bool
        isCReference;
    };
}


#include "Domain/Compiler/Instruction/Instruction.h"

#endif //ELET_SYNTAX_DECLARATIONS_H
