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
        if (isStartFunction)
        {
            setStartFunctionSymbolName(function, functionDeclaration);
        }

        transformFunctionParameters(functionDeclaration, function, stackOffset);
        transformLocalStatements(functionDeclaration->body->statements, function, stackOffset);
        function->stackSize = getAlignedStackSizeFromStackOffset(stackOffset);

        _currentRoutineStack.pop();
        return function;
    }


    void
    Transformer::setStartFunctionSymbolName(output::FunctionRoutine* function, const ast::FunctionDeclaration* functionDeclaration) const
    {
        assert(function->isStartFunction && "Function must be start function.");

        switch (_compilerOptions.objectFileTarget)
        {
            case ObjectFileTarget::Pe32:
                function->name = "main";
                break;
            case ObjectFileTarget::StashIR:
            case ObjectFileTarget::MachO:
                function->name = "_main";
                break;
            default:
                throw std::runtime_error("Unknown ObjectFileTarget in setStartFunctionSymbolName");
        }
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
                output::StoreRegisterInstruction* store = new output::StoreRegisterInstruction(static_cast<output::OperandRegister>(static_cast<int>(output::OperandRegister::Arg0) + i), stackOffset, param->registerSize);
                addInstruction(store);

                // TODO: The reference instruction should be on property basis.
                parameterDeclaration->referenceInstruction = store;
                stackOffset += static_cast<uint64_t>(param->registerSize);
                function->parameterSize += static_cast<uint8_t>(param->registerSize);
            }
        }
    }


    List<output::Instruction*>*
    Transformer::transformLocalStatements(List<ast::Syntax*>& statements, output::FunctionRoutine* function, uint64_t& stackOffset)
    {
        List<output::Instruction*>* list = new List<output::Instruction*>();
        for (const auto& statement : statements)
        {
            switch (statement->kind)
            {
                case ast::SyntaxKind::VariableDeclaration:
                    transformVariableDeclaration(reinterpret_cast<ast::VariableDeclaration*>(statement), function, stackOffset);
                    break;
                case ast::SyntaxKind::CallExpression:
                    transformCallExpression(reinterpret_cast<ast::CallExpression*>(statement), function, stackOffset);
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
        output::CanonicalExpression expression = transformExpression(returnStatement->expression, stackOffset);
        if (std::holds_alternative<output::ImmediateValue>(expression))
        {
            output::ImmediateValue immediateValue = std::get<output::ImmediateValue>(expression);
            addInstruction(new output::MoveImmediateInstruction(output::OperandRegister::Return, immediateValue.value, returnStatement->expectedType->size()));
        }
        else if (std::holds_alternative<output::OperandRegister>(expression))
        {
            assert((*returnStatement->expectedType != TypeKind::Void) && "Type should not equal void here. The type checker should have already failed.");
            addInstruction(new output::MoveRegisterToRegisterInstruction(output::OperandRegister::Return, output::OperandRegister::Scratch0, returnStatement->expectedType->size()));
        }
        else
        {
            // Returns void
        }
    }


    void
    Transformer::transformVariableDeclaration(ast::VariableDeclaration* variable, output::FunctionRoutine* function, uint64_t& stackOffset)
    {
        RegisterSize registerSize = variable->resolvedType->size();
        output::CanonicalExpression canonicalExpression = transformExpression(variable->expression, stackOffset);
        if (std::holds_alternative<output::ImmediateValue>(canonicalExpression))
        {
            auto immediateValue = std::get<output::ImmediateValue>(canonicalExpression);
            output::StoreImmediateInstruction* storeInstruction = new output::StoreImmediateInstruction(immediateValue.value, stackOffset, registerSize);
            variable->referenceInstruction = storeInstruction;
            addInstruction(storeInstruction);
        }
        else
        {
            output::OperandRegister operandRegister = std::get<output::OperandRegister>(canonicalExpression);
            output::StoreRegisterInstruction* storeInstruction = new output::StoreRegisterInstruction(output::OperandRegister::Scratch0, stackOffset, registerSize);
            variable->referenceInstruction = storeInstruction;
            addInstruction(storeInstruction);
        }
        function->localVariableSize += static_cast<uint8_t>(registerSize);
    }


    output::CanonicalExpression
    Transformer::transformExpression(ast::Expression* expression, uint64_t& stackOffset)
    {
        return transformExpression(expression, stackOffset, nullptr);
    }


    output::CanonicalExpression
    Transformer::transformExpression(ast::Expression* expression, uint64_t& stackOffset, ast::Type* forcedType)
    {
        switch (expression->kind)
        {
            case ast::SyntaxKind::BinaryExpression:
                return transformBinaryExpression(reinterpret_cast<ast::BinaryExpression*>(expression), stackOffset, forcedType);
            case ast::SyntaxKind::IntegerLiteral:
            {
                ast::IntegerLiteral* integerLiteral = reinterpret_cast<ast::IntegerLiteral*>(expression);
                ast::Type* type = forcedType ? forcedType : integerLiteral->resolvedType;
                if (integerLiteral->isNegative)
                {
                    return output::ImmediateValue(type->kind, -integerLiteral->value);
                }
                else
                {
                    return output::ImmediateValue(type->kind, integerLiteral->value);
                }
            }
            case ast::SyntaxKind::CharacterLiteral:
            {
                ast::TypeKind typeKind = forcedType ? forcedType->kind : TypeKind::U8;
                ast::CharacterLiteral* characterLiteral = reinterpret_cast<ast::CharacterLiteral*>(expression);
                return output::ImmediateValue(typeKind, characterLiteral->value);
            }
            case ast::SyntaxKind::PropertyExpression:
                return transformPropertyExpression(reinterpret_cast<ast::PropertyExpression*>(expression));
            default:
                throw std::runtime_error("Unknown expression for transformation.");
        }
    }


    output::OperandRegister
    Transformer::transformImmediateToRegisterExpression(output::OperandRegister left, uint64_t right, ast::BinaryExpression* binaryExpression)
    {
        _scratchRegisterIndex--;
        output::OperandRegister scratchRegister = borrowScratchRegister();
        RegisterSize registerSize = binaryExpression->resolvedType->size();
        switch (binaryExpression->binaryOperatorKind)
        {
            case ast::BinaryOperatorKind::Plus:
                addInstruction(new output::AddImmediateToRegisterInstruction(scratchRegister, left, right, registerSize));
                break;
            case ast::BinaryOperatorKind::Minus:
                addInstruction(new output::SubtractImmediateToRegisterInstruction(scratchRegister, left, right, registerSize));
                break;
            default:
                assert("Not implemented binary operator in 'transformImmediateToRegisterExpression'.");
        }
        return scratchRegister;
    }


    output::ImmediateValue
    Transformer::transformImmediateToImmediateBinaryExpression(ast::BinaryOperatorKind binaryOperatorKind, const output::ImmediateValue& left, const output::ImmediateValue& right)
    {
        IntegerKind integerKind = left.integerType.kind;
        output::ImmediateValue immediateValue(left.integerType, 0);
        switch (binaryOperatorKind)
        {
            case ast::BinaryOperatorKind::BitwiseAnd:
                switch (integerKind)
                {
                    case IntegerKind::S32:
                        immediateValue.value = static_cast<int32_t>(left.value) & static_cast<int32_t>(right.value);
                        break;
                    case IntegerKind::S64:
                        immediateValue.value = static_cast<int64_t>(left.value) & static_cast<int64_t>(right.value);
                        break;
                    case IntegerKind::U32:
                        immediateValue.value = static_cast<uint32_t>(left.value) & static_cast<uint32_t>(right.value);
                        break;
                    case IntegerKind::U64:
                        immediateValue.value = left.value & right.value;
                        break;
                    default:
                        throw std::runtime_error("Unknown type kind for bitwise and.");
                }
                return immediateValue;

            case ast::BinaryOperatorKind::BitwiseXor:
                switch (integerKind)
                {
                    case IntegerKind::S32:
                        immediateValue.value = static_cast<int32_t>(left.value) ^ static_cast<int32_t>(right.value);
                        break;
                    case IntegerKind::S64:
                        immediateValue.value = static_cast<int64_t>(left.value) ^ static_cast<int64_t>(right.value);
                        break;
                    case IntegerKind::U32:
                        immediateValue.value = static_cast<uint32_t>(left.value) ^ static_cast<uint32_t>(right.value);
                        break;
                    case IntegerKind::U64:
                        immediateValue.value = left.value ^ right.value;
                        break;
                    default:
                        throw std::runtime_error("Unknown type kind for bitwise and.");
                }
                return immediateValue;
            case ast::BinaryOperatorKind::BitwiseOr:
                switch (integerKind)
                {
                    case IntegerKind::S32:
                        immediateValue.value = static_cast<int32_t>(left.value) | static_cast<int32_t>(right.value);
                        break;
                    case IntegerKind::S64:
                        immediateValue.value = static_cast<int64_t>(left.value) | static_cast<int64_t>(right.value);
                        break;
                    case IntegerKind::U32:
                        immediateValue.value = static_cast<uint32_t>(left.value) | static_cast<uint32_t>(right.value);
                        break;
                    case IntegerKind::U64:
                        immediateValue.value = left.value | right.value;
                        break;
                    default:
                        throw std::runtime_error("Unknown type kind for bitwise and.");
                }
                return immediateValue;
            case ast::BinaryOperatorKind::Multiply:
            {
                switch (integerKind)
                {
                    case IntegerKind::S32:
                        immediateValue.value = left.value * right.value;
                        break;
                    case IntegerKind::S64:
                        immediateValue.value = static_cast<uint64_t>(static_cast<int32_t>(left.value) * static_cast<int32_t>(right.value));
                        break;
                    case IntegerKind::U32:
                        immediateValue.value = static_cast<uint64_t>(left.value) * static_cast<uint64_t>(right.value);
                        break;
                    case IntegerKind::U64:
                        immediateValue.value = static_cast<uint32_t>(left.value) * static_cast<uint32_t>(right.value);
                        break;
                    default:
                        throw std::runtime_error("Should not contain lower than 32bit ints.");
                }
                return immediateValue;
            }

            case ast::BinaryOperatorKind::Plus:
            {
                switch (integerKind)
                {
                    case IntegerKind::U8:
                        immediateValue.value = static_cast<uint8_t>(left.value) + static_cast<uint8_t>(right.value);
                        break;
                    case IntegerKind::S8:
                        immediateValue.value = static_cast<int8_t>(left.value) + static_cast<int8_t>(right.value);
                        break;
                    case IntegerKind::U16:
                        immediateValue.value = static_cast<uint16_t>(left.value) + static_cast<uint16_t>(right.value);
                        break;
                    case IntegerKind::S16:
                        immediateValue.value = static_cast<int16_t>(left.value) + static_cast<int16_t>(right.value);
                        break;
                    case IntegerKind::U32:
                        immediateValue.value = static_cast<uint32_t>(left.value) + static_cast<uint32_t>(right.value);
                        break;
                    case IntegerKind::S32:
                        immediateValue.value = static_cast<int32_t>(left.value) + static_cast<int32_t>(right.value);
                        break;
                    case IntegerKind::S64:
                        immediateValue.value = static_cast<int64_t>(left.value) + static_cast<int64_t>(right.value);
                        break;
                    case IntegerKind::U64:
                        immediateValue.value = static_cast<uint64_t>(left.value) + static_cast<uint64_t>(right.value);
                        break;
                }
                return immediateValue;
            }
            default:
                throw std::runtime_error("Not implemented binary expression operator for immediate immediateValue.");
        }
    }


    void
    Transformer::transformCallExpression(const ast::CallExpression* callExpression, output::FunctionRoutine* function, uint64_t& stackOffset)
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
        function->calleeParameterSize = 0;
        if (functionDeclaration->body)
        {
            callInstruction->routine = transformFunctionDeclaration(functionDeclaration);
            uint8_t thisCallParameterSize = reinterpret_cast<output::FunctionRoutine*>(callInstruction->routine)->parameterSize;
            if (function->calleeParameterSize < thisCallParameterSize)
            {
                function->calleeParameterSize = thisCallParameterSize;
            }
        }
        else
        {
            Utf8String* externalSymbol = new Utf8String();
            if (functionDeclaration->isCReference)
            {
                switch (_compilerOptions.objectFileTarget)
                {
                    case ObjectFileTarget::Pe32:
                        *externalSymbol = Utf8String("__imp_") + functionDeclaration->symbol->name.toString();
                        break;
                    case ObjectFileTarget::MachO:
                        *externalSymbol = Utf8String("_") + functionDeclaration->symbol->name.toString();
                        break;
                    default:
                        assert("Unknown object file target.");
                }
            }
            else
            {
                *externalSymbol = functionDeclaration->symbol->name.toString();
            }
            callInstruction->routine = new output::ExternalRoutine(*externalSymbol);
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
            referenceInstruction->registerSize);
        addInstruction(loadInstruction);
    }


    void
    Transformer::transformArgumentStringLiteral(ast::StringLiteral* stringLiteral, uint64_t argumentIndex)
    {
        output::String* cstring = addStaticConstantString(stringLiteral);
        output::MoveAddressToRegisterInstruction* store = new output::MoveAddressToRegisterInstruction(
            getOperandRegisterFromArgumentIndex(argumentIndex), 0, RegisterSize::Pointer);
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
    Transformer::transformPropertyExpression(ast::PropertyExpression* propertyExpression)
    {
        output::MemoryAllocation* referenceInstruction = propertyExpression->referenceDeclaration->referenceInstruction;
        output::LoadInstruction* loadInstruction = new output::LoadInstruction(borrowScratchRegister(), referenceInstruction->stackOffset, referenceInstruction->registerSize);
        addInstruction(loadInstruction);
        return loadInstruction->destination;
    }


    output::OperandRegister
    Transformer::transformRegisterToRegisterBinaryExpression(ast::BinaryExpression* binaryExpression, output::OperandRegister target, output::OperandRegister value)
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
        RegisterSize registerSize = binaryExpression->resolvedType->size();
        switch (binaryExpression->binaryOperatorKind)
        {
            case ast::BinaryOperatorKind::Plus:
                addInstruction(new output::AddRegisterToRegisterInstruction(destination, target, value, registerSize));
                break;
            case ast::BinaryOperatorKind::Minus:
                addInstruction(new output::SubtractRegisterToRegisterInstruction(destination, target, value, registerSize));
                break;
            case ast::BinaryOperatorKind::Multiply:
                addInstruction(new output::MultiplySignedRegisterToRegisterInstruction(destination, target, value, registerSize));
                break;
            case ast::BinaryOperatorKind::Divide:
                if (binaryExpression->resolvedType->sign() == ast::type::Signedness::Signed)
                {
                    addInstruction(new output::DivideSignedRegisterToRegisterInstruction(destination, target, value, registerSize));
                }
                else
                {
                    addInstruction(new output::DivideUnsignedRegisterToRegisterInstruction(destination, target, value, registerSize));
                }
                break;
            case ast::BinaryOperatorKind::Modulo:
                if (binaryExpression->resolvedType->sign() == ast::type::Signedness::Signed)
                {
                    addInstruction(new output::ModuloSignedRegisterToRegisterInstruction(destination, target, value, registerSize));
                }
                else
                {
                    addInstruction(new output::ModuloUnsignedRegisterToRegisterInstruction(destination, target, value, registerSize));
                }
                break;
            case ast::BinaryOperatorKind::BitwiseAnd:
                addInstruction(new output::AndRegisterToRegisterInstruction(destination, target, value, registerSize));
                break;
            case ast::BinaryOperatorKind::BitwiseXor:
                addInstruction(new output::XorRegisterToRegisterInstruction(destination, target, value, registerSize));
                break;
            case ast::BinaryOperatorKind::BitwiseOr:
                addInstruction(new output::OrRegisterToRegisterInstruction(destination, target, value, registerSize));
                break;
            default:
                throw std::runtime_error("Not implemented binary operator kind for memory to memory binary expression.");
        }
        return destination;
    }


    output::CanonicalExpression
    Transformer::transformBinaryExpression(ast::BinaryExpression* binaryExpression, uint64_t& stackOffset, ast::Type* forcedType)
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
        ast::Type* type = forcedType ? forcedType : binaryExpression->resolvedType;
        // Transform binary expressions first and then regular expressions.
        if (binaryExpression->left->kind == ast::SyntaxKind::BinaryExpression)
        {
            leftOutput = transformExpression(binaryExpression->left, stackOffset, type);
        }
        if (binaryExpression->right->kind == ast::SyntaxKind::BinaryExpression)
        {
            rightOutput = transformExpression(binaryExpression->right, stackOffset, type);
        }
        if (std::holds_alternative<std::monostate>(leftOutput))
        {
            leftOutput = transformExpression(binaryExpression->left, stackOffset, type);
        }
        if (std::holds_alternative<std::monostate>(rightOutput))
        {
            rightOutput = transformExpression(binaryExpression->right, stackOffset, type);
        }

        if (std::holds_alternative<output::OperandRegister>(leftOutput) && std::holds_alternative<output::OperandRegister>(rightOutput))
        {
            output::OperandRegister target = std::get<output::OperandRegister>(leftOutput);
            output::OperandRegister value = std::get<output::OperandRegister>(rightOutput);

            // Make operand registers have an ascending order
            if (target < value)
            {
                return transformRegisterToRegisterBinaryExpression(binaryExpression, target, value);
            }
            else
            {
                return transformRegisterToRegisterBinaryExpression(binaryExpression, value, target);
            }
        }
        else if (std::holds_alternative<output::ImmediateValue>(leftOutput) && std::holds_alternative<output::ImmediateValue>(rightOutput))
        {
            return transformImmediateToImmediateBinaryExpression(binaryExpression->binaryOperatorKind, std::get<output::ImmediateValue>(leftOutput), std::get<output::ImmediateValue>(rightOutput));
        }
        else if (std::holds_alternative<output::OperandRegister>(leftOutput))
        {
            return transformImmediateToRegisterExpression(std::get<output::OperandRegister>(leftOutput), std::get<output::ImmediateValue>(rightOutput).value, binaryExpression);
        }
        else // if (std::holds_alternative<output::OperandRegister>(rightOutput))
        {
            return transformImmediateToRegisterExpression(std::get<output::OperandRegister>(rightOutput), std::get<output::ImmediateValue>(leftOutput).value, binaryExpression);
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


    IntegerKind
    Transformer::getIntegerTypeFromTypeKind(TypeKind typeKind)
    {
        switch (typeKind)
        {
            case TypeKind::Char:
            case TypeKind::U8:
                return IntegerKind::U8;
            case TypeKind::S8:
                return IntegerKind::S8;
            case TypeKind::U16:
                return IntegerKind::U16;
            case TypeKind::S16:
                return IntegerKind::S16;
            case TypeKind::U32:
                return IntegerKind::U32;
            case TypeKind::S32:
                return IntegerKind::S32;
            case TypeKind::U64:
                return IntegerKind::U64;
            case TypeKind::S64:
                return IntegerKind::S64;
        }
        assert("Cannot convert typekind to integer type.");
    }
}
