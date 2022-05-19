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
                output::StoreRegisterInstruction* store = new output::StoreRegisterInstruction(static_cast<output::OperandRegister>(static_cast<int>(output::OperandRegister::Arg0) + i), stackOffset, param->destinationSize, param->sign);
                addInstruction(store);

                // TODO: The reference instruction should be on property basis.
                parameterDeclaration->referenceInstruction = store;
                stackOffset += static_cast<uint64_t>(param->destinationSize);
                function->parameterSize += static_cast<uint8_t>(param->destinationSize);
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
            addInstruction(new output::MoveImmediateInstruction(output::OperandRegister::Return, immediateValue.value.toUint64(), returnStatement->expectedType->size()));
        }
        else if (std::holds_alternative<output::RegisterResult>(expression))
        {
            assert((*returnStatement->expectedType != TypeKind::Void) && "Type should not equal void here. The type checker should have already failed.");
            output::RegisterResult result = std::get<output::RegisterResult>(expression);
            bool writtenExtension = false;
            // Note: we have to sign extend if:
            //  * placeholder-type s64 expression-type s32
            //  * placeholder-type s32 expression-type s16
            //  * placeholder-type s16 expression-type s8
            if (returnStatement->expectedType->sign() == Sign::Signed)
            {
                if (returnStatement->expectedType->size() > result.registerSize)
                {
                    addInstruction(new output::MoveSignExtendInstruction(
                        output::OperandRegister::Scratch0,
                        output::OperandRegister::Return,
                        result.registerSize,
                        returnStatement->expectedType->size()));
                    writtenExtension = true;
                }
            }
            else
            {
                // Note: we have to zero extend if:
                //  * placeholder-type u8 expression-type 16
                //  * placeholder-type u8 expression-type 32
                //  * placeholder-type u16 expression-type 32
                //  * placeholder-type u32 expression-type 64
                if (returnStatement->expectedType->size() != result.registerSize)
                {
                    addInstruction(new output::MoveZeroExtendInstruction(
                        output::OperandRegister::Scratch0,
                        output::OperandRegister::Return,
                        returnStatement->expectedType->size(),
                        result.registerSize));
                    writtenExtension = true;
                }
            }
            if (!writtenExtension)
            {
                addInstruction(new output::MoveRegisterToRegisterInstruction(
                    output::OperandRegister::Return,
                    output::OperandRegister::Scratch0,
                    returnStatement->expectedType->size()));
            }
        }
        else
        {
            // Returns void
        }
    }


    void
    Transformer::transformVariableDeclaration(ast::VariableDeclaration* variable, output::FunctionRoutine* function, uint64_t& stackOffset)
    {
        ast::Type* type = variable->declarationType;
        RegisterSize registerSize = type->size();
        Sign sign = type->sign();
        output::CanonicalExpression canonicalExpression = transformExpression(variable->expression, stackOffset);
        if (std::holds_alternative<output::ImmediateValue>(canonicalExpression))
        {
            auto immediateValue = std::get<output::ImmediateValue>(canonicalExpression);
            output::StoreImmediateInstruction* storeInstruction = new output::StoreImmediateInstruction(immediateValue.value.toUint64(), stackOffset, registerSize, sign);
            variable->referenceInstruction = storeInstruction;
            addInstruction(storeInstruction);
        }
        else
        {
            output::RegisterResult result = std::get<output::RegisterResult>(canonicalExpression);
            output::StoreRegisterInstruction* storeInstruction = new output::StoreRegisterInstruction(result._register, stackOffset, registerSize, sign);
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
            case ast::SyntaxKind::BooleanLiteral:
                return transformBooleanLiteral(reinterpret_cast<ast::BooleanLiteral*>(expression), stackOffset);
            case ast::SyntaxKind::BinaryExpression:
                return transformBinaryExpression(reinterpret_cast<ast::BinaryExpression*>(expression), stackOffset, forcedType);
            case ast::SyntaxKind::IntegerLiteral:
            {
                ast::IntegerLiteral* integerLiteral = reinterpret_cast<ast::IntegerLiteral*>(expression);
                ast::Type* type = forcedType ? forcedType : integerLiteral->operatingType;
                return output::ImmediateValue(type->kind, integerLiteral->value);
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


    output::RegisterResult
    Transformer::transformImmediateToRegisterExpression(output::OperandRegister left, Int128 right, ast::BinaryExpression* binaryExpression)
    {
        _scratchRegisterIndex--;
        output::OperandRegister scratchRegister = borrowScratchRegister();
        RegisterSize registerSize = binaryExpression->resultingType->size();
        RegisterSize boundSize = binaryExpression->resultingType->boundSize();
        uint64_t rightUint64 = right.toUint64();
        switch (binaryExpression->binaryOperatorKind)
        {
            case ast::BinaryOperatorKind::Add:
                addInstruction(new output::AddImmediateToRegisterInstruction(scratchRegister, left, rightUint64, registerSize));
                break;
            case ast::BinaryOperatorKind::Minus:
                addInstruction(new output::SubtractImmediateToRegisterInstruction(scratchRegister, left, rightUint64, registerSize));
                break;
            default:
                assert("Not implemented binary operator in 'transformImmediateToRegisterExpression'.");
        }
        return output::RegisterResult(scratchRegister, registerSize, boundSize);
    }


    output::ImmediateValue
    Transformer::transformImmediateToImmediateBinaryExpression(ast::BinaryOperatorKind binaryOperatorKind, const output::ImmediateValue& left, const output::ImmediateValue& right)
    {
        IntegerKind integerKind = left.integerType.kind;
        output::ImmediateValue immediateValue(left.integerType, 0);
        switch (binaryOperatorKind)
        {
            case ast::BinaryOperatorKind::And:
                immediateValue.value = left.value.toBool() && right.value.toBool();
                return immediateValue;

            case ast::BinaryOperatorKind::Or:
                immediateValue.value = left.value.toBool() || right.value.toBool();
                return immediateValue;

            case ast::BinaryOperatorKind::BitwiseAnd:
                immediateValue.value = left.value & right.value;
                return immediateValue;

            case ast::BinaryOperatorKind::BitwiseXor:
                immediateValue.value = left.value ^ right.value;
                return immediateValue;

            case ast::BinaryOperatorKind::BitwiseOr:
                immediateValue.value = left.value | right.value;
                return immediateValue;

            case ast::BinaryOperatorKind::Multiply:
                immediateValue.value = left.value * right.value;
                return immediateValue;

            case ast::BinaryOperatorKind::Add:
                immediateValue.value = left.value + right.value;
                return immediateValue;

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
                    case ObjectFileTarget::StashIR:
                    case ObjectFileTarget::MachO:
                        *externalSymbol = Utf8String("_") + functionDeclaration->symbol->name.toString();
                        break;
                    default:
                        throw std::runtime_error("Unknown object file target.");
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
        output::LoadUnsignedInstruction* loadInstruction = new output::LoadUnsignedInstruction(
            getOperandRegisterFromArgumentIndex(argumentIndex),
            referenceInstruction->stackOffset,
            referenceInstruction->destinationSize);
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
        if (parameter->declarationType->pointers > 0)
        {
            output::ParameterDeclaration* parameterDeclaration = new output::ParameterDeclaration(0, RegisterSize::Quad, parameter->declarationType->sign());
            parameterList.add(parameterDeclaration);
        }
        return parameterList;
    }


    output::RegisterResult
    Transformer::transformPropertyExpression(ast::PropertyExpression* propertyExpression)
    {
        output::MemoryAllocation* referenceInstruction = propertyExpression->referenceDeclaration->referenceInstruction;
        if (referenceInstruction->sign == Sign::Signed && referenceInstruction->destinationSize < RegisterSize::Dword)
        {
            output::LoadSignedInstruction* loadInstruction = new output::LoadSignedInstruction(borrowScratchRegister(), referenceInstruction->stackOffset, referenceInstruction->destinationSize);
            addInstruction(loadInstruction);
            return output::RegisterResult(loadInstruction->destination, getSupportedRegisterSize(referenceInstruction->destinationSize));
        }
        else
        {
            output::LoadUnsignedInstruction* loadInstruction = new output::LoadUnsignedInstruction(borrowScratchRegister(), referenceInstruction->stackOffset, referenceInstruction->destinationSize);
            addInstruction(loadInstruction);
            return output::RegisterResult(loadInstruction->destination, getSupportedRegisterSize(referenceInstruction->destinationSize));
        }
    }


    output::RegisterResult
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

        RegisterSize boundSize = binaryExpression->operatingType->boundSize();
        RegisterSize registerSize = getSupportedRegisterSize(boundSize);
        if (binaryExpression->binaryOperatorKind == ast::BinaryOperatorKind::Modulo)
        {
            output::OperandRegister divisionResultRegister = borrowScratchRegister();
            _scratchRegisterIndex -= 3;
            output::OperandRegister destination = borrowScratchRegister();
            if (binaryExpression->resultingType->sign() == Sign::Signed)
            {
                addInstruction(new output::ModuloSignedRegisterToRegisterInstruction(destination, target, value, divisionResultRegister, registerSize));
            }
            else
            {
                addInstruction(new output::ModuloUnsignedRegisterToRegisterInstruction(destination, target, value, divisionResultRegister, registerSize));
            }
            return output::RegisterResult(destination, registerSize, boundSize);
        }
        _scratchRegisterIndex -= 2;
        output::OperandRegister destination = borrowScratchRegister();
        switch (binaryExpression->binaryOperatorKind)
        {
            case ast::BinaryOperatorKind::Add:
                addInstruction(new output::AddRegisterToRegisterInstruction(destination, target, value, registerSize));
                break;
            case ast::BinaryOperatorKind::Minus:
                addInstruction(new output::SubtractRegisterToRegisterInstruction(destination, target, value, registerSize));
                break;
            case ast::BinaryOperatorKind::Multiply:
                addInstruction(new output::MultiplySignedRegisterToRegisterInstruction(destination, target, value, registerSize));
                break;
            case ast::BinaryOperatorKind::Divide:
                if (binaryExpression->resultingType->sign() == Sign::Signed)
                {
                    addInstruction(new output::DivideSignedRegisterToRegisterInstruction(destination, target, value, registerSize));
                }
                else
                {
                    addInstruction(new output::DivideUnsignedRegisterToRegisterInstruction(destination, target, value, registerSize));
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
                if (binaryExpression->binaryOperatorKind == ast::BinaryOperatorKind::Modulo)
                {
                    break;
                }
                throw std::runtime_error("Not implemented binary operator kind for memory to memory binary expression.");
        }

        return output::RegisterResult(destination, registerSize, boundSize);
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
        // Case x % y:
        // ldr x1, y // Mark x1 as busy
        // ldr x2, z // Mark x2 as busy
        // smod x1, x3, x2, x1 // Mark x3 as busy. Mark x3, x2 as available.
        //
        //
        // Case var a = x / y; var b = x % y
        // ldr x1, x // Mark x1 as busy
        // ldr x2, y // Mark x2 as busy
        // sdiv x3, x2, x1
        // str x3, a
        // ssub x1, x3, x2, x1 // Mark x3, x2 as available.
        // str x1, b

        output::CanonicalExpression leftOutput;
        output::CanonicalExpression rightOutput;
        ast::Type* type = forcedType ? forcedType : binaryExpression->resultingType;
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

        if (std::holds_alternative<output::RegisterResult>(leftOutput) && std::holds_alternative<output::RegisterResult>(rightOutput))
        {
            output::RegisterResult target = std::get<output::RegisterResult>(leftOutput);
            output::RegisterResult value = std::get<output::RegisterResult>(rightOutput);

            return transformRegisterToRegisterBinaryExpression(binaryExpression, target._register, value._register);
        }
        else if (std::holds_alternative<output::ImmediateValue>(leftOutput) && std::holds_alternative<output::ImmediateValue>(rightOutput))
        {
            return transformImmediateToImmediateBinaryExpression(binaryExpression->binaryOperatorKind, std::get<output::ImmediateValue>(leftOutput), std::get<output::ImmediateValue>(rightOutput));
        }
        else if (std::holds_alternative<output::RegisterResult>(leftOutput))
        {
            return transformImmediateToRegisterExpression(std::get<output::RegisterResult>(leftOutput)._register, std::get<output::ImmediateValue>(rightOutput).value, binaryExpression);
        }
        else // if (std::holds_alternative<output::RegisterResult>(rightOutput))
        {
            return transformImmediateToRegisterExpression(std::get<output::RegisterResult>(rightOutput)._register, std::get<output::ImmediateValue>(leftOutput).value, binaryExpression);
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


    RegisterSize
    Transformer::getSupportedRegisterSize(RegisterSize size)
    {
        switch (size)
        {
            case RegisterSize::Quad:
            case RegisterSize::Dword:
                return size;
            default:
                return RegisterSize::Dword;
        }
    }


    output::CanonicalExpression
    Transformer::transformBooleanLiteral(ast::BooleanLiteral* booleanLiteral, uint64_t& offset)
    {
        if (booleanLiteral->value)
        {
            return output::ImmediateValue(IntegerKind::U8, 1);
        }
        else
        {
            return output::ImmediateValue(IntegerKind::U8, 0);
        }
    }
}
