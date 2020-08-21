#ifndef ELET_CHECKER_H
#define ELET_CHECKER_H

#include <map>
#include <Foundation/Utf8StringView.h>
#include "Syntax.h"

using namespace elet::foundation;

namespace elet::domain::compiler
{


namespace ast
{
    struct Declaration;
    struct FunctionDeclaration;
    struct CallExpression;
}

#define TYPE_CHAR       (std::uint8_t)0x1
#define TYPE_INT8      (std::uint8_t)0x2
#define TYPE_INT16     (std::uint8_t)0x3
#define TYPE_INT32     (std::uint8_t)0x4
#define TYPE_INT64     (std::uint8_t)0x5
#define TYPE_UINT8      (std::uint8_t)0x6
#define TYPE_UINT16     (std::uint8_t)0x7
#define TYPE_UINT32     (std::uint8_t)0x8
#define TYPE_UINT64     (std::uint8_t)0x9


struct Type
{
    std::uint8_t
    kind: 4,
    pointers: 2,
    _mutable: 1;

    Type()
    {

    }

    Type(std::uint8_t kind, std::uint8_t pointers):
        kind(kind),
        pointers(pointers)
    {
        _mutable = 0;
    }
};


class Checker
{
public:

    void
    checkTopLevelDeclaration(const ast::Declaration* declaration);

    void
    checkFunctionDeclaration(const ast::FunctionDeclaration *functionDeclaration);

    void
    checkCallExpression(const ast::CallExpression* callExpression);

    static
    Type
    getTypeFromExpression(const ast::Expression* expression);

    static
    bool
    isTypeAssignableToPlaceholder(Type& assignment, Type& placeholder);

    static
    Type
    getTypeFromTypeAssignment(ast::TypeAssignment* typeAssignment);

private:

    const ast::SourceFile*
    _sourceFile;
};


}


#endif //ELET_CHECKER_H
