//
// Created by Tingan Ho on 2021-04-17.
//

#ifndef ELET_SYNTAX_BLOCK_H
#define ELET_SYNTAX_BLOCK_H


#include "Syntax.h"


namespace elet::domain::compiler::ast
{
    struct StatementBlock : Syntax
    {
        List<Syntax *>
        statements;

        NameToDeclarationMap
        symbols;
    };


    struct ExternCBlock : Syntax
    {
        List<Declaration*>
        declarations;
    };


    struct AssemblyBody : Syntax
    {
        List<output::Instruction*>*
        instructions;
    };


    struct AssemblyBlock : Syntax
    {
        DeclarationMetadata*
        metadata;

        AssemblyBody*
        body;
    };


    struct DeclarationBlock : Syntax
    {
        List<Syntax*>
        declarations;

        NameToDeclarationMap
        symbols;
    };

    enum class AccessibilityType
    {
        Public,
        Private,
    };


    struct AccessabilityLabel : Syntax
    {
        List<Declaration*>
        declarations;

        AccessibilityType
        __type;
    };
}
#endif //ELET_SYNTAX_BLOCK_H
