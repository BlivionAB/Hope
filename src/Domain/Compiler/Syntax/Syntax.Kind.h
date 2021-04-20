#ifndef ELET_SYNTAX_KIND_H
#define ELET_SYNTAX_KIND_H

#include <cinttypes>

namespace elet::domain::compiler::ast
{
    enum class SyntaxKind : std::uint8_t
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
        ModuleAccessUsage,
        ModuleAccessExpression,
        PropertyDeclaration,
        ConstructorDeclaration,
        ConstructorImplementation,
        UsingStatement,
        ParameterDeclarationList,
        PropertyExpression,
        PropertyAccessExpression,
        ParameterDeclaration,
        Type,
        CallExpression,
        ArgumentList,
        StringLiteral,
        Tuple,
        NamedUsage,
        WildcardUsage,
        Punctuation,
        Name,
        Modifier,

        // Modifiers, Labels
        AccessabilityLabel,

        // Expressions
        LengthOfExpression,
        AddressOfExpression,

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
