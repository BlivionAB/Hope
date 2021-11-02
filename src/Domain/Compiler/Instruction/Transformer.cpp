#include "Transformer.h"

namespace elet::domain::compiler::instruction
{
    using namespace compiler;


    Transformer::Transformer(std::mutex& dataMutex, CompilerOptions& compilerOptions) :
        _dataMutex(dataMutex),
        _compilerOptions(compilerOptions),
        _defaultFunctionStackOffset(getDefaultFunctionStackOffset(compilerOptions))
    {

    }


    output::FunctionRoutine*
    Transformer::transform(ast::Declaration* declaration)
    {
        if (declaration->kind == ast::SyntaxKind::FunctionDeclaration)
        {
            ast::FunctionDeclaration* functionDeclaration = reinterpret_cast<ast::FunctionDeclaration*>(declaration);
            output::FunctionRoutine* routine = transformFunctionDeclaration(functionDeclaration);
            routine->isStartFunction = functionDeclaration->signature->isStartFunction;
            functionDeclaration->routine = routine;
            return routine;
        }
        else
        {
            throw std::runtime_error("TEMP: Unknown declaration.");
        }
    }


    output::FunctionRoutine*
    Transformer::createFunctionRoutine(const Utf8StringView& name)
    {
        return new output::FunctionRoutine(name);
    }


    output::FunctionRoutine*
    Transformer::transformFunctionDeclaration(const ast::FunctionDeclaration* functionDeclaration)
    {
        if (functionDeclaration->routine)
        {
            return functionDeclaration->routine;
        }
        output::FunctionRoutine* function = createFunctionRoutine(functionDeclaration->name->name);
        _currentRoutineStack.push(function);
        uint64_t stackOffset = _defaultFunctionStackOffset;
        bool isStartFunction = function->isStartFunction = functionDeclaration->signature->isStartFunction;
        function->inferReturnZero = isStartFunction && functionDeclaration->signature->hasReturnStatementOnAllBranches;
        function->name = isStartFunction ? "_main" : functionDeclaration->symbol->externalSymbol;
        transformFunctionParameters(functionDeclaration, function, stackOffset);
        transformLocalStatements(functionDeclaration->body->statements, stackOffset);
        function->stackSize = getAlignedStackSizeFromStackOffset(stackOffset);
        _currentRoutineStack.pop();
        return function;
    }


    void
    Transformer::transformFunctionParameters(const ast::FunctionDeclaration* functionDeclaration, output::FunctionRoutine* function, uint64_t& stackOffset)
    {
        unsigned int i = 0;
        for (ast::ParameterDeclaration* parameterDeclaration : functionDeclaration->parameterList->parameters)
        {
            List<output::ParameterDeclaration*> parameterList = segmentParameterDeclaration(parameterDeclaration);
            for (const auto& param : parameterList)
            {
                output::StoreRegisterInstruction* store = new output::StoreRegisterInstruction(static_cast<output::OperandRegister>(static_cast<int>(output::OperandRegister::Arg0) + i), stackOffset, param->allocationSize);
                addInstruction(store);

                // TODO: The reference instruction should be on property basis.
                parameterDeclaration->referenceInstruction = store;
                stackOffset += static_cast<int>(param->allocationSize);
            }
        }
    }


    List<output::Instruction*>*
    Transformer::transformLocalStatements(List<ast::Syntax*>& statements, uint64_t& stackOffset)
    {
        List<output::Instruction*>* list = new List<output::Instruction*>();
        for (const auto& statement : statements)
        {
            switch (statement->kind)
            {
                case ast::SyntaxKind::VariableDeclaration:
                    transformVariableDeclaration(reinterpret_cast<ast::VariableDeclaration*>(statement), stackOffset);
                    break;
                case ast::SyntaxKind::CallExpression:
                    transformCallExpression(reinterpret_cast<ast::CallExpression*>(statement), stackOffset);
                    break;
                case ast::SyntaxKind::ReturnStatement:
                    transformReturnStatement(reinterpret_cast<ast::ReturnStatement*>(statement), stackOffset);
                    break;
                default:
                    throw std::runtime_error("TEMP: Unknown local statement.");
            }
        }
        return list;
    }


    void
    Transformer::transformReturnStatement(ast::ReturnStatement* returnStatement, uint64_t& stackOffset)
    {
        RegisterSize registerSize;
        output::CanonicalExpression expression = transformExpression(returnStatement->expression, stackOffset, output::OperandRegister::Left, registerSize);
        if (std::holds_alternative<uint64_t>(expression))
        {
            uint64_t immediateValue = std::get<uint64_t>(expression);
            addInstruction(new output::MoveImmediateInstruction(output::OperandRegister::Return, immediateValue, registerSize));
        }
        else if (std::holds_alternative<output::OperandRegister>(expression))
        {
            addInstruction(new output::MoveRegisterInstruction(output::OperandRegister::Return, output::OperandRegister::Left));
        }
        else
        {
            // Returns void
        }
    }


    void
    Transformer::transformVariableDeclaration(ast::VariableDeclaration* variable, uint64_t& stackOffset)
    {
        RegisterSize registerSize;
        output::CanonicalExpression canonicalExpression = transformExpression(variable->expression, stackOffset, output::OperandRegister::Left, registerSize);
        if (std::holds_alternative<uint64_t>(canonicalExpression))
        {
            output::StoreImmediateInstruction* storeInstruction = new output::StoreImmediateInstruction(std::get<uint64_t>(canonicalExpression), stackOffset, registerSize);
            variable->referenceInstruction = storeInstruction;
            addInstruction(storeInstruction);
        }
        else
        {
            output::StoreRegisterInstruction* storeInstruction = new output::StoreRegisterInstruction(output::OperandRegister::Left, stackOffset, registerSize);
            variable->referenceInstruction = storeInstruction;
            addInstruction(storeInstruction);
        }
    }


    output::CanonicalExpression
    Transformer::transformExpression(ast::Expression* expression, uint64_t& stackOffset, output::OperandRegister operandRegister, RegisterSize& registerSize)
    {
        switch (expression->kind)
        {
            case ast::SyntaxKind::BinaryExpression:
                return transformBinaryExpression(reinterpret_cast<ast::BinaryExpression*>(expression), stackOffset, registerSize);
            case ast::SyntaxKind::IntegerLiteral:
            {
                ast::IntegerLiteral* integerLiteral = reinterpret_cast<ast::IntegerLiteral*>(expression);
                switch (integerLiteral->resolvedType->kind)
                {
                    case TypeKind::U8:
                        registerSize = RegisterSize::Byte;
                        return static_cast<uint8_t>(integerLiteral->value);
                    case TypeKind::U16:
                        registerSize = RegisterSize::Word;
                        return static_cast<uint16_t>(integerLiteral->value);
                    case TypeKind::U32:
                        registerSize = RegisterSize::Dword;
                        return static_cast<uint32_t>(integerLiteral->value);
                    case TypeKind::U64:
                        registerSize = RegisterSize::Quad;
                        return static_cast<uint64_t>(integerLiteral->value);
                    case TypeKind::S8:
                        registerSize = RegisterSize::Byte;
                        return static_cast<uint8_t>(static_cast<int8_t>(integerLiteral->value));
                    case TypeKind::S16:
                        registerSize = RegisterSize::Word;
                        return static_cast<uint16_t>(static_cast<int16_t>(integerLiteral->value));
                    case TypeKind::S32:
                        registerSize = RegisterSize::Dword;
                        if (integerLiteral->isNegative)
                        {
                            return static_cast<uint32_t>(static_cast<int32_t>(-integerLiteral->value));
                        }
                        else
                        {
                            return static_cast<uint32_t>(static_cast<int32_t>(integerLiteral->value));
                        }
                    case TypeKind::S64:
                        registerSize = RegisterSize::Quad;
                        return static_cast<uint64_t>(static_cast<int64_t>(integerLiteral->value));
                    default:
                        throw std::runtime_error("Cannot resolve integer value.");
                }
            }
            case ast::SyntaxKind::PropertyExpression:
                return transformPropertyExpression(reinterpret_cast<ast::PropertyExpression*>(expression), operandRegister, registerSize);
            default:
                throw std::runtime_error("Unknown expression for transformation.");
        }
    }


    bool
    Transformer::isAddressValueExpression(ast::Expression* expression)
    {
        return !isImmediateValueExpression(expression);
    }


    output::ImmediateValue
    Transformer::transformToUint(ast::IntegerLiteral* integer)
    {
        return integer->value;
    }


    output::ScratchRegister*
    Transformer::transformImmediateToMemoryExpression(output::ScratchRegister* left, output::ImmediateValue right, ast::BinaryOperatorKind _operator)
    {
    //    auto scratchRegister = new ScratchRegister();
    //    scratchRegister->operation = new ImmediateToMemoryOperation(left, right, _operator);
        return nullptr;
    }


    uint64_t
    Transformer::transformImmediateBinaryExpression(ast::BinaryExpression* binaryExpression)
    {
        switch (binaryExpression->binaryOperatorKind)
        {
            case ast::BinaryOperatorKind::Plus:
            {
                ast::IntegerLiteral* left = reinterpret_cast<ast::IntegerLiteral*>(binaryExpression->left);
                ast::IntegerLiteral* right = reinterpret_cast<ast::IntegerLiteral*>(binaryExpression->right);
                if (left->resolvedType->sign() == ast::type::Signedness::Signed)
                {
                    switch (left->resolvedType->size())
                    {
                        case RegisterSize::Quad:
                            return left->value + right->value;
                        case RegisterSize::Dword:
                            return static_cast<uint64_t>(static_cast<int32_t>(left->value) + static_cast<int32_t>(right->value));
                        default:
                            throw std::runtime_error("Should not contain lower than 32bit ints.");
                    }
                }
                else
                {
                    switch (left->resolvedType->size())
                    {
                        case RegisterSize::Quad:
                            return static_cast<uint64_t>(left->value) + static_cast<uint64_t>(right->value);
                        case RegisterSize::Dword:
                            return static_cast<uint32_t>(left->value) + static_cast<uint32_t>(right->value);
                        default:
                            throw std::runtime_error("Should not contain lower than 32bit ints.");
                    }
                }
            }
            default:
                throw std::runtime_error("Not implemented binary expression operator for immediate immediateValue.");
        }
    }


    void
    Transformer::transformCallExpression(const ast::CallExpression* callExpression, uint64_t& stackOffset)
    {
        auto callInstruction = new output::CallInstruction();
        uint64_t argumentIndex = 0;
        for (const auto& argument : callExpression->argumentList->arguments)
        {
            switch (argument->kind)
            {
                case ast::SyntaxKind::StringLiteral:
                    transformArgumentStringLiteral(reinterpret_cast<ast::StringLiteral*>(argument), argumentIndex);
                    break;
                case ast::SyntaxKind::PropertyExpression:
                    transformArgumentPropertyExpression(reinterpret_cast<ast::PropertyExpression*>(argument), argumentIndex);
                    break;
                default:;
            }
        }
        const ast::FunctionDeclaration* functionDeclaration = reinterpret_cast<ast::FunctionDeclaration*>(callExpression->referenceDeclaration);
        if (functionDeclaration->body)
        {
            callInstruction->routine = transformFunctionDeclaration(functionDeclaration);
        }
        else
        {
            callInstruction->routine = new output::ExternalRoutine(callExpression->referenceDeclaration->symbol->externalSymbol);
        }
        addInstruction(callInstruction);
    }


    void
    Transformer::addInstruction(output::Instruction* instruction)
    {
        _currentRoutineStack.top()->instructions.add(instruction);
    }


    void
    Transformer::transformArgumentPropertyExpression(ast::PropertyExpression* propertyExpression, uint64_t argumentIndex)
    {
        output::MemoryAllocation* referenceInstruction = propertyExpression->referenceDeclaration->referenceInstruction;
        output::LoadInstruction* loadInstruction = new output::LoadInstruction(
            getOperandRegisterFromArgumentIndex(argumentIndex),
            referenceInstruction->stackOffset,
            referenceInstruction->allocationSize);
        addInstruction(loadInstruction);
    }


    void
    Transformer::transformArgumentStringLiteral(ast::StringLiteral* stringLiteral, uint64_t argumentIndex)
    {
        output::String* cstring = addStaticConstantString(stringLiteral);
        output::MoveAddressInstruction* store = new output::MoveAddressInstruction(
            getOperandRegisterFromArgumentIndex(argumentIndex), 0);
        store->constant = cstring;
        addInstruction(store);
    }


    output::OperandRegister
    Transformer::getOperandRegisterFromArgumentIndex(uint64_t argumentIndex)
    {
        return static_cast<output::OperandRegister>(static_cast<uint64_t>(output::OperandRegister::Arg0) + argumentIndex);
    }


    Utf8StringView
    Transformer::getSymbolReference(ast::NamedExpression *expression)
    {
        return expression->name->name;
    }


    output::String*
    Transformer::addStaticConstantString(ast::StringLiteral* stringLiteral)
    {
        auto cstring = new output::String(Utf8StringView(stringLiteral->stringStart, stringLiteral->stringEnd));
        _cstrings.add(cstring);
        return cstring;
    }


    List<output::ParameterDeclaration*>
    Transformer::segmentParameterDeclaration(ast::ParameterDeclaration* parameter)
    {
        List<output::ParameterDeclaration*> parameterList;
        if (parameter->resolvedType->pointers > 0)
        {
            output::ParameterDeclaration* parameterDeclaration = new output::ParameterDeclaration(0, RegisterSize::Quad);
            parameter->referenceInstruction = parameterDeclaration;
            parameterList.add(parameterDeclaration);
        }
        return parameterList;
    }


    bool
    Transformer::isImmediateValueExpression(ast::Expression* expression)
    {
        return expression->kind == ast::SyntaxKind::IntegerLiteral;
    }


    output::OperandRegister
    Transformer::transformPropertyExpression(ast::PropertyExpression* propertyExpression, output::OperandRegister operandRegister, RegisterSize& registerSize)
    {
        output::MemoryAllocation* referenceInstruction = propertyExpression->referenceDeclaration->referenceInstruction;
        output::LoadInstruction* loadInstruction = new output::LoadInstruction(operandRegister, referenceInstruction->stackOffset, referenceInstruction->allocationSize);
        addInstruction(loadInstruction);
        return output::OperandRegister::Left;
    }


    void
    Transformer::transformMemoryToMemoryBinaryExpression(ast::BinaryOperatorKind binaryOperatorKind)
    {
        // Str [Sp + 0], 3
        // Str [Sp + 4], 3
        // Ldr Op1, [Sp + 0]
        // Ldr Op2, [Sp + 4]
        // Add Op1, Op1, Op2
        // Note, we can implicitly just add both destination registers!.
        //
        // A catch in x86. It supports directly mem to register addition. So, it should take the previous move's memory
        // address and apply it on the operation.
        switch (binaryOperatorKind)
        {
            case ast::BinaryOperatorKind::Plus:
                addInstruction(new output::AddRegisterInstruction());
                break;
            default:
                throw std::runtime_error("Not implemented binary operator kind for memory to memory binary expression.");
        }
    }


    output::CanonicalExpression
    Transformer::transformBinaryExpression(ast::BinaryExpression* binaryExpression, uint64_t& stackOffset, RegisterSize& registerSize)
    {
        // memory * immediate
        //   immediate * memory (it's cummutative)
        // immediate * immediate
        // memory * memory
        auto left = transformExpression(binaryExpression->left, stackOffset, output::OperandRegister::Left, registerSize);
        if (std::holds_alternative<uint64_t>(left))
        {
            auto right = transformExpression(binaryExpression->right, stackOffset, output::OperandRegister::Left, registerSize);
            if (std::holds_alternative<uint64_t>(right))
            {
                return transformImmediateBinaryExpression(binaryExpression);
            }

            // rhs is memory here
    //        return transformImmediateToMemoryExpression(std::get<output::ScratchRegister*>(right), transformToUint(reinterpret_cast<ast::DecimalLiteral*>(binaryExpression->left)), binaryExpression->binaryOperatorKind);
        }
        // lhs is memory from here

        else if (isAddressValueExpression(binaryExpression->right))
        {
            transformExpression(binaryExpression->right, stackOffset, output::OperandRegister::Right, registerSize);
            transformMemoryToMemoryBinaryExpression(binaryExpression->binaryOperatorKind);
        }
        //            return transformImmediateToMemoryExpression(transformExpression(binaryExpression->left), transformToUint(reinterpret_cast<ast::DecimalLiteral*>(binaryExpression->right)), binaryExpression->binaryOperatorKind);
        return output::OperandRegister::Left;
    }


    uint64_t
    Transformer::getDefaultFunctionStackOffset(const CompilerOptions& compilerOptions)
    {
        switch (compilerOptions.assemblyTarget)
        {
            case AssemblyTarget::x86_64:
                return 0;
            case AssemblyTarget::Aarch64:
                return 16;
            default:
                return 0;
        }
    }


    uint64_t
    Transformer::getAlignedStackSizeFromStackOffset(uint64_t stackOffset)
    {
        uint64_t rest = stackOffset % 16;
        if (rest != 0)
        {
            return (16 - rest) + stackOffset;
        }
        return stackOffset;
    }
}
