#ifndef ELET_BINDER_H
#define ELET_BINDER_H


#include <map>
#include <Foundation/Utf8StringView.h>
#include "Syntax/Syntax.h"
#include "Parser.h"
#include "Domain/Compiler/Instruction/Instruction.h"


namespace elet::domain::compiler::instruction::output
{
    struct Operand;
    struct Parameter;
}


namespace elet::domain::compiler::ast
{
    using namespace instruction;
    using namespace foundation;


    struct BindingWork
    {
        Syntax*
        declaration;

        BindingWork(Syntax* declaration):
            declaration(declaration)
        { }
    };


    typedef std::map<Utf8StringView, std::variant<std::map<Utf8StringView, Declaration*>*, void*>> DomainDeclarationMap;


    class Binder
    {

    public:

        Binder();

        void
        performWork(BindingWork& work, const DomainDeclarationMap* domainDeclarationMap);

        void
        bindFunctionDeclaration(FunctionDeclaration* declaration);

        using SymbolMap = std::map<Utf8StringView, Declaration*>;

        static
        DomainDeclaration*
        startDomainDeclaration;

        static
        InterfaceDeclaration*
        startDomainInterfaceDeclaration;

    private:

        void
        bindDomainDeclaration(DomainDeclaration* domain);

        static
        void
        bindExpression(Expression* expression);

        void
        bindUsingStatement(UsingStatement* usingStatement);

        std::map<Utf8StringView, Declaration*>*
        getDomainDeclarations(const List<Name*>& domains, unsigned int domainIndex, const DomainDeclarationMap* domainDeclarationMap);

        static
        void
        bindPropertyExpression(PropertyExpression* property);

        void
        bindCallExpression(CallExpression* callExpression);

        const DomainDeclarationMap*
        _domainDeclarationMap;

        const std::map<Utf8StringView, Declaration*>
        _usages;

    //    static
    //    thread_local
    //    std::map<Utf8StringView, UsingStatement*>*
    //    _importSymbols;

        static
        thread_local
        FunctionDeclaration*
        _currentFunctionDeclaration;

        List<Expression*>
        _forwardedReferences;

        void
        bindStatementBlock(StatementBlock* block);

        void
        bindVariableDeclaration(VariableDeclaration* variableDeclaration);

        void
        bindIfStatement(const IfStatement* ifStatement);

        static void
        bindBinaryExpression(BinaryExpression* binaryExpression);
    };
}


#endif //ELET_BINDER_H
