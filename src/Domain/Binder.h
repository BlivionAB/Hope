#ifndef ELET_BINDER_H
#define ELET_BINDER_H


#include <map>
#include <Foundation/Utf8StringView.h>
#include "Syntax.h"
#include "Instruction/Instruction.h"


namespace elet::domain::compiler
{

namespace ast
{
    struct Declaration;
}

namespace instruction::output
{
    struct Operand;
    struct Parameter;
}


using namespace instruction;
using namespace foundation;


struct DeclarationWork
{
    ast::Declaration*
    declaration;

    ast::SourceFile*
    file;

    DeclarationWork(ast::Declaration* declaration, ast::SourceFile* file):
        declaration(declaration),
        file(file)
    { }
};


class Binder
{

public:

    Binder();

    void
    performWork(DeclarationWork& work);

    void
    bindFunction(ast::FunctionDeclaration* declaration);

    using SymbolMap = std::map<Utf8StringView, ast::Declaration*>;

private:

    static
    thread_local
    std::map<Utf8StringView, ast::Declaration*>*
    fileDeclaration;

    void
    bindAssemblyBlock(ast::AssemblyBlock* assemblyBlock, SymbolMap& symbols);

    void
    tryBindOperand(output::Operand* operand, SymbolMap& symbols);

    void
    bindCallExpression(ast::CallExpression* callExpression);

    List<ast::Expression*>
    _forwardedReferences;
};


}


#endif //ELET_BINDER_H
