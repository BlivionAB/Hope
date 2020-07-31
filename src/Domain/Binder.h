#ifndef ELET_BINDER_H
#define ELET_BINDER_H


#include <map>
#include <Foundation/Utf8StringView.h>
#include "Syntax.h"

namespace elet::domain::compiler
{


using namespace foundation;


struct BindingWork
{
    Declaration*
    declaration;

    File*
    file;

    BindingWork(Declaration* declaration, File* file):
        declaration(declaration),
        file(file)
    { }
};


class Binder
{

public:

    Binder();

    void
    performWork(BindingWork& work);

    void
    bindFunction(FunctionDeclaration* declaration);

    using SymbolMap = std::map<Utf8StringView, Declaration*>;

private:

    static
    thread_local
    std::map<Utf8StringView, Declaration*>*
    fileDeclaration;

    void
    bindAssemblyBlock(AssemblyBlock* assemblyBlock, SymbolMap& symbols);

    void
    tryBindOperand(output::Operand* operand, SymbolMap& symbols);

    void
    bindCallExpression(CallExpression* callExpression);

    List<compiler::Expression*>
    _forwardedReferences;
};


}


#endif //ELET_BINDER_H
