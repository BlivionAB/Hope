#ifndef ELET_SYNTAX_KIND_H
#define ELET_SYNTAX_KIND_H

#include <cinttypes>

namespace elet::domain::compiler::ast
{
    enum class SyntaxKind : uint8_t
    {
        Unknown,
        AssemblyBlock,
        AssemblyBody,
        ArrayLiteral,
        MetadataProperty,
        DeclarationMetadata,
        FunctionDeclaration,
        EnumDeclaration,
        FunctionBody,
        ModuleDeclaration,
        DomainAccessUsage,
        ModuleAccessExpression,
        PropertyDeclaration,
        ConstructorDeclaration,
        ConstructorImplementation,
        UsingStatement,
        ParameterDeclarationList,
        ParameterDeclaration,
        Type,
        ArgumentList,
        StringLiteral,
        BooleanLiteral,
        IntegerLiteral,
        Tuple,
        NamedUsage,
        WildcardUsage,
        Punctuation,
        Name,
        Modifier,

        // Modifiers, Labels
        AccessabilityLabel,

        // Statements
        IfStatement,
        ReturnStatement,

        // Expressions
        LengthOfExpression,
        AddressOfExpression,
        BinaryExpression,
        CallExpression,
        PropertyExpression,
        PropertyAccessExpression,

        // Declarations
        ObjectDeclaration,
        InterfaceDeclaration,
        VariableDeclaration,
        DomainDeclaration,

        // Blocks
        DeclarationBlock,
        ExternCBlock,

        FunctionMetadata,
    };
}
#endif //ELET_SYNTAX_KIND_H
