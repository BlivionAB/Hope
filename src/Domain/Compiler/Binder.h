#ifndef ELET_BINDER_H
#define ELET_BINDER_H


#include <map>
#include <Foundation/Utf8StringView.h>
#include "Syntax/Syntax.h"
#include "Domain/Compiler/Instruction/Instruction.h"


namespace elet::domain::compiler
{

namespace ast
{
    struct Declaration;
    struct PropertyExpression;
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
    _fileDeclaration;

    typedef std::map<Utf8StringView, std::variant<std::map<Utf8StringView, ast::Declaration*>*, void*>> AccessMap;

    AccessMap*
    _globalDeclarations;

//    static
//    thread_local
//    std::map<Utf8StringView, ast::UsingStatement*>*
//    _importSymbols;

    static
    thread_local
    ast::FunctionDeclaration*
    _currentFunctionDeclaration;

    void
    bindAssemblyBlock(ast::AssemblyBlock* assemblyBlock, SymbolMap& symbols);

    void
    tryBindOperand(output::Operand* operand, SymbolMap& symbols);

    static
    void
    bindExpression(ast::Expression* expression);

    void
    bindUsingStatement(ast::UsingStatement* usingStatement);

    std::map<Utf8StringView, ast::Declaration*>*
    getDomainDeclarations(const ast::DomainAccessUsage* domainAccessUsage, const AccessMap* accessMap);

    static
    void
    bindPropertyExpression(ast::PropertyExpression* property);

    void
    bindCallExpression(ast::CallExpression* callExpression);

    List<ast::Expression*>
    _forwardedReferences;
};


}


#endif //ELET_BINDER_H
