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


class Checker
{
public:

    void
    checkTopLevelDeclaration(const ast::Declaration* declaration);

    void
    checkFunctionDeclaration(const ast::FunctionDeclaration *functionDeclaration);

    void
    checkCallExpression(const ast::CallExpression* callExpression);

private:

    const ast::SourceFile*
    _sourceFile;
};


}


#endif //ELET_CHECKER_H
