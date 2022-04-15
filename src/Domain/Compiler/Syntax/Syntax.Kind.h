#ifndef ELET_SYNTAX_KIND_H
#define ELET_SYNTAX_KIND_H

#include <cinttypes>

namespace elet::domain::compiler::ast
{
    enum class SyntaxKind : uint8_t
    {
        EndOfFile,
        Unknown,
        ErrorNode,
        SyntaxList,

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
        CharacterLiteral,
        StringLiteral,
        BooleanLiteral,
        IntegerLiteral,
        BinaryLiteral,
        DecimalLiteral,
        HexadecimalLiteral,
        IntegerSuffix,
        LengthOfExpression,
        AddressOfExpression,
        BinaryExpression,
        BinaryOperator,
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
