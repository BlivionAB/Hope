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
        output::CanonicalExpression expression = transformExpression(returnStatement->expression, stackOffset, registerSize);
        if (std::holds_alternative<output::ImmediateValue>(expression))
        {
            uint64_t immediateValue = std::get<output::ImmediateValue>(expression).value;
            addInstruction(new output::MoveImmediateInstruction(output::OperandRegister::Return, immediateValue, registerSize));
        }
        else if (std::holds_alternative<output::OperandRegister>(expression))
        {
            addInstruction(new output::MoveRegisterInstruction(output::OperandRegister::Return, output::OperandRegister::Scratch0));
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
        output::CanonicalExpression canonicalExpression = transformExpression(variable->expression, stackOffset, registerSize);
        if (std::holds_alternative<output::ImmediateValue>(canonicalExpression))
        {
            output::StoreImmediateInstruction* storeInstruction = new output::StoreImmediateInstruction(std::get<output::ImmediateValue>(canonicalExpression).value, stackOffset, registerSize);
            variable->referenceInstruction = storeInstruction;
            addInstruction(storeInstruction);
        }
        else
        {
            output::StoreRegisterInstruction* storeInstruction = new output::StoreRegisterInstruction(output::OperandRegister::Scratch0, stackOffset, registerSize);
            variable->referenceInstruction = storeInstruction;
            addInstruction(storeInstruction);
        }
    }


    output::CanonicalExpression
    Transformer::transformExpression(ast::Expression* expression, uint64_t& stackOffset, RegisterSize& registerSize)
    {
        switch (expression->kind)
        {
            case ast::SyntaxKind::BinaryExpression:
                return transformBinaryExpression(reinterpret_cast<ast::BinaryExpression*>(expression), stackOffset, registerSize);
            case ast::SyntaxKind::IntegerLiteral:
            {
                ast::IntegerLiteral* integerLiteral = reinterpret_cast<ast::IntegerLiteral*>(expression);
                registerSize = integerLiteral->resolvedType->size();
                if (integerLiteral->isNegative)
                {
                    return output::ImmediateValue(integerLiteral->resolvedType->kind, -integerLiteral->value);
                }
                else
                {
                    return output::ImmediateValue(integerLiteral->resolvedType->kind, integerLiteral->value);
                }
            }
            case ast::SyntaxKind::PropertyExpression:
                return transformPropertyExpression(reinterpret_cast<ast::PropertyExpression*>(expression),registerSize);
            default:
                throw std::runtime_error("Unknown expression for transformation.");
        }
    }


    output::OperandRegister
    Transformer::transformImmediateToRegisterExpression(output::OperandRegister left, uint64_t right, ast::BinaryOperatorKind binaryOperatorKind)
    {
        _scratchRegisterIndex--;
        output::OperandRegister scratchRegister = borrowScratchRegister();
        switch (binaryOperatorKind)
        {
            case ast::BinaryOperatorKind::Plus:

                break;
        }
        return scratchRegister;
    }


    output::ImmediateValue
    Transformer::transformImmediateToImmediateBinaryExpression(ast::BinaryOperatorKind binaryOperatorKind, const output::ImmediateValue& left, const output::ImmediateValue& right)
    {
        output::ImmediateValue immediateValue(left.type, 0);
        switch (binaryOperatorKind)
        {
            case ast::BinaryOperatorKind::BitwiseAnd:
                switch (left.type)
                {
                    case TypeKind::S32:
                        immediateValue.value = static_cast<int32_t>(left.value) & static_cast<int32_t>(right.value);
                        break;
                    case TypeKind::S64:
                        immediateValue.value = static_cast<int64_t>(left.value) & static_cast<int64_t>(right.value);
                        break;
                    case TypeKind::U32:
                        immediateValue.value = static_cast<uint32_t>(left.value) & static_cast<uint32_t>(right.value);
                        break;
                    case TypeKind::U64:
                        immediateValue.value = left.value & right.value;
                        break;
                    default:
                        throw std::runtime_error("Unknown type kind for bitwise and.");
                }
                return immediateValue;

            case ast::BinaryOperatorKind::BitwiseXor:
                switch (left.type)
                {
                    case TypeKind::S32:
                        immediateValue.value = static_cast<int32_t>(left.value) ^ static_cast<int32_t>(right.value);
                        break;
                    case TypeKind::S64:
                        immediateValue.value = static_cast<int64_t>(left.value) ^ static_cast<int64_t>(right.value);
                        break;
                    case TypeKind::U32:
                        immediateValue.value = static_cast<uint32_t>(left.value) ^ static_cast<uint32_t>(right.value);
                        break;
                    case TypeKind::U64:
                        immediateValue.value = left.value ^ right.value;
                        break;
                    default:
                        throw std::runtime_error("Unknown type kind for bitwise and.");
                }
                return immediateValue;
            case ast::BinaryOperatorKind::BitwiseOr:
                switch (left.type)
                {
                    case TypeKind::S32:
                        immediateValue.value = static_cast<int32_t>(left.value) | static_cast<int32_t>(right.value);
                        break;
                    case TypeKind::S64:
                        immediateValue.value = static_cast<int64_t>(left.value) | static_cast<int64_t>(right.value);
                        break;
                    case TypeKind::U32:
                        immediateValue.value = static_cast<uint32_t>(left.value) | static_cast<uint32_t>(right.value);
                        break;
                    case TypeKind::U64:
                        immediateValue.value = left.value | right.value;
                        break;
                    default:
                        throw std::runtime_error("Unknown type kind for bitwise and.");
                }
                return immediateValue;
            case ast::BinaryOperatorKind::Multiply:
            {
                switch (left.type)
                {
                    case TypeKind::S32:
                        immediateValue.value = left.value * right.value;
                        break;
                    case TypeKind::S64:
                        immediateValue.value = static_cast<uint64_t>(static_cast<int32_t>(left.value) * static_cast<int32_t>(right.value));
                        break;
                    case TypeKind::U32:
                        immediateValue.value = static_cast<uint64_t>(left.value) * static_cast<uint64_t>(right.value);
                        break;
                    case TypeKind::U64:
                        immediateValue.value = static_cast<uint32_t>(left.value) * static_cast<uint32_t>(right.value);
                        break;
                    default:
                        throw std::runtime_error("Should not contain lower than 32bit ints.");
                }
                return immediateValue;
            }

            case ast::BinaryOperatorKind::Plus:
            {
                switch (left.type)
                {
                    case TypeKind::S32:
                        immediateValue.value = left.value + right.value;
                        break;
                    case TypeKind::S64:
                        immediateValue.value = static_cast<uint64_t>(static_cast<int32_t>(left.value) + static_cast<int32_t>(right.value));
                        break;
                    case TypeKind::U32:
                        immediateValue.value = static_cast<uint64_t>(left.value) + static_cast<uint64_t>(right.value);
                        break;
                    case TypeKind::U64:
                        immediateValue.value = static_cast<uint32_t>(left.value) + static_cast<uint32_t>(right.value);
                        break;
                    default:
                        throw std::runtime_error("Should not contain lower than 32bit ints.");
                }
                return immediateValue;
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


    output::OperandRegister
    Transformer::transformPropertyExpression(ast::PropertyExpression* propertyExpression, RegisterSize& registerSize)
    {
        output::MemoryAllocation* referenceInstruction = propertyExpression->referenceDeclaration->referenceInstruction;
        output::LoadInstruction* loadInstruction = new output::LoadInstruction(borrowScratchRegister(), referenceInstruction->stackOffset, referenceInstruction->allocationSize);
        addInstruction(loadInstruction);
        return loadInstruction->destination;
    }


    output::OperandRegister
    Transformer::transformRegisterToRegisterBinaryExpression(ast::BinaryOperatorKind binaryOperatorKind, output::OperandRegister target, output::OperandRegister value)
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


        // Decrease scratch register index, since we should leave them back after usage
        _scratchRegisterIndex -= 2;

        output::OperandRegister destination = borrowScratchRegister();
        switch (binaryOperatorKind)
        {
            case ast::BinaryOperatorKind::Plus:
                addInstruction(new output::AddRegisterInstruction(destination, target, value));
                break;
            case ast::BinaryOperatorKind::Minus:
                addInstruction(new output::SubtractRegisterInstruction(destination, target, value));
                break;
            case ast::BinaryOperatorKind::Multiply:
                addInstruction(new output::MultiplyRegisterInstruction(destination, target, value));
                break;
            case ast::BinaryOperatorKind::Divide:
                addInstruction(new output::DivideRegisterInstruction(destination, target, value));
                break;
            case ast::BinaryOperatorKind::BitwiseAnd:
                addInstruction(new output::AndRegisterInstruction(destination, target, value));
                break;
            case ast::BinaryOperatorKind::BitwiseXor:
                addInstruction(new output::XorRegisterInstruction(destination, target, value));
                break;
            case ast::BinaryOperatorKind::BitwiseOr:
                addInstruction(new output::OrRegisterInstruction(destination, target, value));
                break;
            default:
                throw std::runtime_error("Not implemented binary operator kind for memory to memory binary expression.");
        }
        return destination;
    }


    output::CanonicalExpression
    Transformer::transformBinaryExpression(ast::BinaryExpression* binaryExpression, uint64_t& stackOffset, RegisterSize& registerSize)
    {
        // (1) if both hand-sides are binary expressions evaluate both. left first.
        // (2) Continue (1) until you find a binary expression without binary expression.
        // (3) Transform binary expression by storing them in registers an add instruction that operate on them.

        // Case: x * y + z * v:
        // ldr x1, x // Mark x1 as busy
        // ldr x2, y // Mark x2 as busy
        // mul x1, x1, x2 // Mark x2 as available
        // ldr x2, z // Mark x2 as busy
        // ldr x3, v // Mark x3 as busy
        // mul x2, x2, x3 // Mark x3 as available
        // add x1, x1, x2 // Mark x2 as available
        //
        //
        // Case x * y + z * v + x * y:
        // ldr x1, x // Mark x1 as busy
        // ldr x2, y // Mark x2 as busy
        // mul x1, x1, x2 // Mark x2 as available
        // ldr x2, z // Mark x2 as busy
        // ldr x3, v // Mark x3 as busy
        // mul x2, x2, x3 // Mark x3 as available
        // add x1, x1, x2 // Mark x2 as available
        // ldr x2, x // Mark x2 as busy
        // ldr x3, y // Mark x3 as busy
        // mul x2, x2, x3 // Mark x3 as available
        // add x1, x1, x2 // Mark x2 as available
        //
        //
        // Case x ^ y & z | v:
        // ldr x1, y // Mark x1 as busy
        // ldr x2, z // Mark x2 as busy
        // and x1, x1, x2 // Mark x2 as available
        // ldr x2, x // Mark x2 as busy
        // xor x1, x1, x2 // Mark x2 as available
        // ldr x2, v // Mark x2 as busy
        // or x1, x1, x2 // Mark x2 as available
        //

        output::CanonicalExpression leftOutput;
        output::CanonicalExpression rightOutput;

        // Transform binary expressions first and then regular expressions.
        if (binaryExpression->left->kind == ast::SyntaxKind::BinaryExpression)
        {
            leftOutput = transformExpression(binaryExpression->left, stackOffset, registerSize);
        }
        if (binaryExpression->right->kind == ast::SyntaxKind::BinaryExpression)
        {
            rightOutput = transformExpression(binaryExpression->right, stackOffset, registerSize);
        }
        if (std::holds_alternative<std::monostate>(leftOutput))
        {
            leftOutput = transformExpression(binaryExpression->left, stackOffset, registerSize);
        }
        if (std::holds_alternative<std::monostate>(rightOutput))
        {
            rightOutput = transformExpression(binaryExpression->right, stackOffset, registerSize);
        }

        if (std::holds_alternative<output::OperandRegister>(leftOutput) && std::holds_alternative<output::OperandRegister>(rightOutput))
        {
            output::OperandRegister target = std::get<output::OperandRegister>(leftOutput);
            output::OperandRegister value = std::get<output::OperandRegister>(rightOutput);

            // Make operand registers have an ascending order
            if (target < value)
            {
                return transformRegisterToRegisterBinaryExpression(binaryExpression->binaryOperatorKind, target, value);
            }
            else
            {
                return transformRegisterToRegisterBinaryExpression(binaryExpression->binaryOperatorKind, value, target);
            }
        }
        else if (std::holds_alternative<output::ImmediateValue>(leftOutput) && std::holds_alternative<output::ImmediateValue>(rightOutput))
        {
            return transformImmediateToImmediateBinaryExpression(binaryExpression->binaryOperatorKind, std::get<output::ImmediateValue>(leftOutput), std::get<output::ImmediateValue>(rightOutput));
        }
        else if (std::holds_alternative<output::OperandRegister>(leftOutput))
        {
            return transformImmediateToRegisterExpression(std::get<output::OperandRegister>(leftOutput), std::get<output::ImmediateValue>(rightOutput).value, binaryExpression->binaryOperatorKind);
        }
        else // if (std::holds_alternative<output::OperandRegister>(rightOutput))
        {
            return transformImmediateToRegisterExpression(std::get<output::OperandRegister>(rightOutput), std::get<output::ImmediateValue>(leftOutput).value, binaryExpression->binaryOperatorKind);
        }
    }


    output::OperandRegister
    Transformer::borrowScratchRegister()
    {
        uint8_t scratchRegister = static_cast<uint8_t>(output::OperandRegister::Scratch0) + _scratchRegisterIndex;
        _scratchRegisterIndex++;
        return static_cast<output::OperandRegister>(scratchRegister);
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
