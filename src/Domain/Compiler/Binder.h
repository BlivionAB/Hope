#ifndef ELET_BINDER_H
#define ELET_BINDER_H


#include <map>
#include <Foundation/Utf8StringView.h>
#include "Syntax/Syntax.h"
#include "Parser.h"
#include "Domain/Compiler/Instruction/Instruction.h"


namespace elet::domain::compiler
{

namespace ast
{
    struct Declaration;
    struct DomainDeclaration;
    struct PropertyExpression;
    typedef std::map<Utf8StringView, std::variant<std::map<Utf8StringView, ast::Declaration*>*, void*>> DomainDeclarationMap;
}

namespace instruction::output
{
    struct Operand;
    struct Parameter;
}


using namespace instruction;
using namespace foundation;


struct BindingWork
{
    ast::Syntax*
    declaration;

    ast::SourceFile*
    file;

    BindingWork(ast::Syntax* declaration, ast::SourceFile* file):
        declaration(declaration),
        file(file)
    { }
};


class Binder
{

public:

    Binder();

    void
    performWork(BindingWork& work, const ast::DomainDeclarationMap* domainDeclarationMap);

    void
    bindFunction(ast::FunctionDeclaration* declaration);

    using SymbolMap = std::map<Utf8StringView, ast::Declaration*>;

private:

    static
    thread_local
    std::map<Utf8StringView, ast::Declaration*>*
    _fileDeclaration;

    const ast::DomainDeclarationMap*
    _domainDeclarationMap;

    const std::map<Utf8StringView, ast::Declaration*>
    _usages;

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
    getDomainDeclarations(const List<ast::Name*>& domains, unsigned int domainIndex, const ast::DomainDeclarationMap* domainDeclarationMap);

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
