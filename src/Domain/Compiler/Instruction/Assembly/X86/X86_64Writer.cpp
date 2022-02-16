#include "X86_64Writer.h"
#include <cassert>

namespace elet::domain::compiler::instruction::output::x86
{
    X86_64Writer::X86_64Writer(List<std::uint8_t>* output) :
        AssemblyWriterInterface(output)
    {
    }


    void
    X86_64Writer::writeFunctionPrologue(FunctionRoutine* function)
    {
        switch (_callingConvention)
        {
            case CallingConvention::SysV:
                writeSysVPrologue(function);
                break;
            case CallingConvention::MicrosoftX64:
                writeMicrosoftX64Prologue(function);
                break;
            default:
                assert("Unknown calling convention.");
        }
    }


    void
    X86_64Writer::writeSysVPrologue(FunctionRoutine* function)
    {
        bw->writeInstructionsInFunction(
            {
                OneByteOpCode::Push_rBP,
                OpCodePrefix::RexMagic | OpCodePrefix::RexW,
                OneByteOpCode::Mov_Ev_Gv,
                ModBits::Mod3 | RegBits::Reg_RSP | RmBits::Rm_RBP
            },
            function);

        if (function->stackSize > 0)
        {
            writeSubtractImmediateInstruction(OperandRegister::StackPointer, function->stackSize, function);
        }
    }


    void
    X86_64Writer::writeMicrosoftX64Prologue(FunctionRoutine* function)
    {
        uint8_t stackAllocationSize = getMicrosoftX64StackAllocationSize(function);
        writeSubtractImmediateInstruction(OperandRegister::StackPointer, stackAllocationSize, function);
    }


    void
    X86_64Writer::writeFunctionEpilogue(FunctionRoutine* function)
    {
        switch (_callingConvention)
        {
            case CallingConvention::SysV:
                writeSysVFunctionEpilogue(function);
                break;
            case CallingConvention::MicrosoftX64:
                writeMicrosoftX64Epilogue(function);
                break;
            default:
                assert("Unknown calling convention.");
        }
    }


    void
    X86_64Writer::writeSysVFunctionEpilogue(FunctionRoutine* function)
    {
        if (function->inferReturnZero)
        {
            bw->writeInstructionsInFunction(
                {
                    OneByteOpCode::Xor_Ev_Gv,
                    ModBits::Mod3 | RegBits::Reg_RAX | RmBits::Rm_RAX
                },
                function);
        }
        if (function->stackSize > 0)
        {
            writeAddImmediateInstruction(OperandRegister::StackPointer, function->stackSize, function);
        }
        bw->writeInstructionsInFunction(
            {
                OneByteOpCode::Pop_rBP,
                OneByteOpCode::Ret,
            },
            function);
    }


    void
    X86_64Writer::writeMicrosoftX64Epilogue(FunctionRoutine* function)
    {
        uint8_t stackAllocationSize = getMicrosoftX64StackAllocationSize(function);
        writeAddImmediateInstruction(OperandRegister::StackPointer, stackAllocationSize, function);
        bw->writeInstructionInFunction(OneByteOpCode::Ret, function);
    }

    uint8_t
    X86_64Writer::getMicrosoftX64StackAllocationSize(const FunctionRoutine* function) const
    {
        uint8_t stackAllocationSize = function->localVariableSize + 40; // 32 for four registers and 8 for return address
        if (function->calleeParameterSize > 32)
        {
            stackAllocationSize += function->calleeParameterSize;
        }
        return stackAllocationSize;
    }


    void
    X86_64Writer::writeStubs()
    {
        for (const auto& routine : externalRoutineList)
        {
            for (const auto& relocationAddress : routine->relocationAddressList)
            {
                bw->writeDoubleWordAtAddress(_offset - (relocationAddress + 4), relocationAddress);
            }
            bw->writeByte(ExtGroup5);
            bw->writeByte(ExtGroup5_NearCallRegBits | ModBits::Mod0 | Rm5);
            routine->stubAddress = _offset;
            bw->writeDoubleWord(0);
        }
    }


    void
    X86_64Writer::writeStubHelper()
    {
        auto top = _offset;
        // leaq dyld_private, r11
        bw->writeByte(RexMagic | RexW | RexR);
        bw->writeByte(OneByteOpCode::Lea_Gv_M);
        bw->writeByte(ModBits::Mod0 | RmBits::Rm5 | RegBits::Reg3);
        dyldPrivateOffset = _offset;
        bw->writeDoubleWord(0);

        // push r11
        bw->writeByte(RexMagic | RexB);
        bw->writeByte(OneByteOpCode::Push_rBX);

        // jmpq dyld_stub_binder
        bw->writeByte(OneByteOpCode::ExtGroup5);
        bw->writeByte(OneByteOpCode::ExtGroup5_NearCallRegBits | ModBits::Mod0 | Rm5);
        Utf8String* dyldStubBinderString = new Utf8String("dyld_stub_binder");
        Utf8StringView string = Utf8StringView(*dyldStubBinderString);
        ExternalRoutine* dyldStubBinderRoutine = new ExternalRoutine(string);
        gotBoundRoutines.add(dyldStubBinderRoutine);
        dyldStubBinderRoutine->relocationAddressList.add(_offset);
        bw->writeDoubleWord(0);

        // nop
        bw->writeByte(Nop);

        for (const auto& routine : externalRoutineList)
        {
            routine->stubHelperAddress = _offset;
            bw->writeByte(OneByteOpCode::Push_Iz);
            bw->writeDoubleWord(0);
            bw->writeByte(OP_NEAR_JMP_Jz);
            bw->writeDoubleWord(top - _offset - 4);
        }
    }


    void
    X86_64Writer::writeTextSection(FunctionRoutine* routine)
    {
        writeFunction(routine);
    }


    void
    X86_64Writer::writeFunctionRelocationAddresses(FunctionRoutine* routine)
    {
        for (const auto& relocationAddress : routine->relocationAddresses)
        {
            bw->writeDoubleWordAtAddress(routine->offset - relocationAddress.value1, relocationAddress.offset);
        }
        routine->relocationAddresses.clear();
    }


    void
    X86_64Writer::writeStoreRegisterInstruction(StoreRegisterInstruction* storeRegisterInstruction, FunctionRoutine* function)
    {
        uint8_t rexByte = uint8_t(OpCodePrefix::RexMagic);
        if (storeRegisterInstruction->stackOffset < INT8_MAX)
        {
            rexByte |= uint8_t(OpCodePrefix::RexW);
        }
        if (_callingConvention == CallingConvention::MicrosoftX64)
        {
            Register _register = getRegisterFromOperandRegister(storeRegisterInstruction->target);
            if (_register == Register::R8 || _register == Register::R9)
            {
                rexByte |= uint8_t(OpCodePrefix::RexR);
            }
        }
        bw->writeInstructionsInFunction({
            rexByte,
            OneByteOpCode::Mov_Ev_Gv
        }, function);
        switch (_callingConvention)
        {
            case CallingConvention::SysV:
                writeEbpReferenceBytes(storeRegisterInstruction->stackOffset, storeRegisterInstruction->target, function);
                break;
            case CallingConvention::MicrosoftX64:
                writeEspReferenceBytes(storeRegisterInstruction->stackOffset, storeRegisterInstruction->target, function);
                break;
            default:
                assert("Unknown calling convention in writeStoreRegisterInstruction.");
        }
    }


    void
    X86_64Writer::writeStoreImmediateInstruction(StoreImmediateInstruction* instruction, FunctionRoutine* function)
    {
        if (instruction->destinationSize >= RegisterSize::Dword)
        {
            writeQuadInstructionMultipleInFunction({
                OneByteOpCode::Mov_Ev_Iz,
                ModBits::Mod1 | RmBits::Rm5 },
                    instruction, function);
            bw->writeByteInFunction(-instruction->stackOffset, function);
            bw->writeDoubleWordInFunction(instruction->value, function);
        }
        else if (instruction->destinationSize == RegisterSize::Byte)
        {
            writeQuadInstructionMultipleInFunction({
                OneByteOpCode::Mov_Eb_Ib,
                ModBits::Mod1 | RmBits::Rm5 },
                instruction, function);
            bw->writeByteInFunction(-instruction->stackOffset, function);
            bw->writeByteInFunction(instruction->value, function);
        }
        else
        {
            writeQuadInstructionMultipleInFunction({
                OpCodePrefix::OperandSizePrefix,
                OneByteOpCode::Mov_Ev_Iz,
                ModBits::Mod1 | RmBits::Rm5 },
                    instruction, function);
            bw->writeByteInFunction(-instruction->stackOffset, function);
            bw->writeWordInFunction(instruction->value, function);

        }
    }


    void
    X86_64Writer::writeCallInstruction(output::CallInstruction* callInstruction, FunctionRoutine* function)
    {
        if (_callingConvention == CallingConvention::MicrosoftX64 && callInstruction->routine->kind == RoutineKind::External)
        {
            bw->writeByte(OneByteOpCode::ExtGroup5);
            bw->writeByte(OneByteOpCode::ExtGroup5_NearCallRegBits | ModBits::Mod0 | RmBits::Rm5);
        }
        else
        {
            bw->writeByte(OneByteOpCode::CallNear);
        }
        switch (callInstruction->routine->kind)
        {
            case RoutineKind::Function:
            {
                auto routine = reinterpret_cast<FunctionRoutine*>(callInstruction->routine);
                if (routine->offset)
                {
                    bw->writeDoubleWord(routine->offset - _offset);
                }
                else
                {
                    routine->relocationAddresses.emplace(_offset, _offset + 4);
                    bw->writeDoubleWord(0);
                    function->subRoutines.add(routine);
                }
                function->codeSize += 5;
                break;
            }
            case RoutineKind::External:
            {
                auto routine = reinterpret_cast<ExternalRoutine*>(callInstruction->routine);
                routine->relocationAddressList.add(_offset);
                bw->writeDoubleWord(0);
                externalRoutineList.add(routine);
                function->codeSize += 5;
                break;
            }
            default:
                throw std::runtime_error("Unknown routine kind.");
        }
    }


    void
    X86_64Writer::writePointer(std::uint64_t address)
    {
        bw->writeDoubleWord(address);
    }


    void
    X86_64Writer::writeCStringSection()
    {
        for (const auto& string : _strings)
        {
            bw->writeDoubleWordAtAddress(_offset - (string->relocationAddress.offset + 4), string->relocationAddress.offset);

            for (const auto s : string->value)
            {
                bw->writeByte(static_cast<std::uint8_t>(s));
            }
            bw->writeByte(0);
        }
    }


    void
    X86_64Writer::writeMoveRegisterToRegisterInstruction(MoveRegisterToRegisterInstruction* instruction, FunctionRoutine* function)
    {
        // It's an unnecessary move, since they are the same register.
        if (instruction->destination == OperandRegister::Return && instruction->target == OperandRegister::Scratch0)
        {
            return;
        }
        writeQuadInstructionMultipleInFunction({
            OneByteOpCode::Mov_Ev_Gv,
            static_cast<uint8_t>(ModBits::Mod3
                | static_cast<uint8_t>(getRegisterFromOperandRegister(instruction->destination))
                | getRmBitsFromOperandRegister(instruction->target))
        }, instruction, function);
    }


    /// Check the chapter instruction set reference — NOP chapter.
    /// https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-instruction-set-reference-manual-325383.pdf
    void
    X86_64Writer::writeInstructionsPadding(uint64_t length)
    {
        switch (length)
        {
            case 1:
                bw->writeByte(OneByteOpCode::Nop);
                break;
            case 2:
                bw->writeByte(0x66);
                bw->writeByte(0x90);
                break;
            case 3:
                bw->writeByte(0x0f);
                bw->writeByte(0x1f);
                bw->writeByte(0x00);
                break;
            case 4:
                bw->writeByte(0x0f);
                bw->writeByte(0x1f);
                bw->writeByte(0x40);
                bw->writeByte(0x00);
                break;
            case 5:
                bw->writeByte(0x0f);
                bw->writeByte(0x1f);
                bw->writeByte(0x44);
                bw->writeByte(0x00);
                bw->writeByte(0x00);
                break;
            case 6:
                bw->writeByte(0x66);
                bw->writeByte(0x0f);
                bw->writeByte(0x1f);
                bw->writeByte(0x44);
                bw->writeByte(0x00);
                bw->writeByte(0x00);
                break;
            case 7:
                bw->writeByte(0x0f);
                bw->writeByte(0x1f);
                bw->writeByte(0x80);
                bw->writeByte(0x00);
                bw->writeByte(0x00);
                bw->writeByte(0x00);
                bw->writeByte(0x00);
                break;
            case 8:
                bw->writeByte(0x0f);
                bw->writeByte(0x1f);
                bw->writeByte(0x84);
                bw->writeByte(0x00);
                bw->writeByte(0x00);
                bw->writeByte(0x00);
                bw->writeByte(0x00);
                bw->writeByte(0x00);
                break;
            default:
                bw->writeByte(0x66);
                bw->writeByte(0x0f);
                bw->writeByte(0x1f);
                bw->writeByte(0x84);
                bw->writeByte(0x00);
                bw->writeByte(0x00);
                bw->writeByte(0x00);
                bw->writeByte(0x00);
                bw->writeByte(0x00);
                length -= 9;
                if (length > 0)
                {
                    writeInstructionsPadding(length);
                }
                break;
        }

    }


    void
    X86_64Writer::relocateStub(uint64_t offset, uint64_t textSegmentStartOffset, ExternalRoutine* externalRoutine)
    {
        bw->writeDoubleWordAtAddress(offset - (textSegmentStartOffset + externalRoutine->stubAddress + 4) /* It should be based after the instruction*/, textSegmentStartOffset + externalRoutine->stubAddress);
    }


    void
    X86_64Writer::relocateDyldPrivate(uint64_t dataSectionOffset, uint64_t textSegmentStartOffset)
    {
        bw->writeDoubleWordAtAddress(dataSectionOffset - (textSegmentStartOffset + dyldPrivateOffset + 4), textSegmentStartOffset + dyldPrivateOffset);
    }


    void
    X86_64Writer::relocateStubHelperOffset(uint64_t offset, uint64_t stubHelperAddress, uint64_t textSegmentStartOffset)
    {
        bw->writeDoubleWordAtAddress(offset, textSegmentStartOffset + stubHelperAddress + 1);
    }


    void
    X86_64Writer::relocateGotBoundRoutine(uint64_t gotOffset, uint64_t offset)
    {
        bw->writeDoubleWordAtAddress(gotOffset - 4 /* -4 due to we haven't added it to relocation address*/, offset);
    }


    void
    X86_64Writer::writeLoadUnsignedInstruction(LoadUnsignedInstruction* instruction, FunctionRoutine* function)
    {
        uint8_t rexByte = 0;
        RegisterSize registerSize = instruction->destinationSize;
        if (registerSize == RegisterSize::Quad || registerSize == RegisterSize::Dword)
        {
            if (instruction->destinationSize == RegisterSize::Quad)
            {
                rexByte |= OpCodePrefix::RexMagic | OpCodePrefix::RexW;
            }
            if (rexByte != 0)
            {
                bw->writeInstructionInFunction(rexByte, function);
            }
            bw->writeInstructionInFunction(OneByteOpCode::Mov_Gv_Ev, function);
        }
        else if (registerSize == RegisterSize::Byte)
        {
            bw->writeInstructionsInFunction({
                OneByteOpCode::TwoByteOpCodePrefix,
                OneByteOpCode::Movzx_Gv_Eb }, function);
        }
        else
        {
            bw->writeInstructionsInFunction({
                OneByteOpCode::TwoByteOpCodePrefix,
                OneByteOpCode::Movzx_Gv_Ew }, function);
        }
        writeStackReferenceBytes(instruction, function);
    }


    void
    X86_64Writer::writeLoadSignedInstruction(LoadSignedInstruction* instruction, FunctionRoutine* function)
    {
        uint8_t rexByte = 0;
        RegisterSize registerSize = instruction->destinationSize;
        if (registerSize == RegisterSize::Quad || registerSize == RegisterSize::Dword)
        {
            if (instruction->destinationSize == RegisterSize::Quad)
            {
                rexByte |= OpCodePrefix::RexMagic | OpCodePrefix::RexW;
            }
            if (rexByte != 0)
            {
                bw->writeInstructionInFunction(rexByte, function);
            }
            bw->writeInstructionInFunction(OneByteOpCode::Mov_Gv_Ev, function);
        }
        else if (registerSize == RegisterSize::Byte)
        {
            bw->writeInstructionsInFunction({
                OneByteOpCode::TwoByteOpCodePrefix,
                OneByteOpCode::Movsx_Gv_Eb }, function);
        }
        else
        {
            bw->writeInstructionsInFunction({
                OneByteOpCode::TwoByteOpCodePrefix,
                OneByteOpCode::Movsx_Gv_Ew }, function);
        }
        writeStackReferenceBytes(instruction, function);
    }


    void
    X86_64Writer::writeStackReferenceBytes(StackOffsetInstruction* instruction, FunctionRoutine* function)
    {
        if (_callingConvention == CallingConvention::MicrosoftX64)
        {
            writeEspReferenceBytes(instruction->stackOffset, instruction->destination, function);
        }
        else
        {
            writeEbpReferenceBytes(instruction->stackOffset, instruction->destination, function);
        }
    }


    void
    X86_64Writer::writeEspReferenceBytes(uint64_t stackOffset, OperandRegister operandRegister, FunctionRoutine* function)
    {
        Register _register = getRegisterFromOperandRegister(operandRegister);
        int64_t convertedStackOffset = static_cast<int64_t>(stackOffset);
        if (convertedStackOffset <= -INT8_MIN && convertedStackOffset >= -INT8_MAX)
        {
            bw->writeInstructionsInFunction(
                {
                    static_cast<uint8_t>(ModBits::Mod1 | getRegBitsFromRegister(_register) | RmBits::Rm4),
                    static_cast<uint8_t>(Sib_ScaleBits::SB_Scale0 | Sib_IndexBits::IB_Index4 | Sib_RegBits::RB_ESP),
                    static_cast<uint8_t>(getMicrosoftX64StackAllocationSize(function) - convertedStackOffset)
                }, function);
        }
        else if (convertedStackOffset <= -INT32_MIN && convertedStackOffset >= -INT32_MAX)
        {
            bw->writeInstructionsInFunction(
                {
                    static_cast<uint8_t>(ModBits::Mod2 | getRegBitsFromRegister(_register) | RmBits::Rm4),
                    static_cast<uint8_t>(Sib_ScaleBits::SB_Scale0 | Sib_IndexBits::IB_Index4 | Sib_RegBits::RB_ESP),
                }, function);
            bw->writeDoubleWordInFunction(getMicrosoftX64StackAllocationSize(function) - convertedStackOffset, function);
        }
        else
        {
            throw std::runtime_error("The stack offset is larger than 32 bit.");
        }
    }


    void
    X86_64Writer::writeEbpReferenceBytes(uint64_t stackOffset, OperandRegister operandRegister, FunctionRoutine* function)
    {
        Register _register = getRegisterFromOperandRegister(operandRegister);
        uint8_t registerBits = getRegBitsFromRegister(_register);
        writeEbpReferenceBytes(stackOffset, registerBits, function);
    }


    void
    X86_64Writer::writeEbpReferenceBytes(uint64_t stackOffset, uint8_t registerBits, FunctionRoutine* function)
    {
        int64_t convertedStackOffset = static_cast<int64_t>(stackOffset);
        if (convertedStackOffset <= -INT8_MIN && convertedStackOffset >= -INT8_MAX)
        {
            bw->writeInstructionsInFunction({
                static_cast<uint8_t>(ModBits::Mod1 | registerBits | RmBits::EbpPlusDisp8),
                static_cast<uint8_t>(-convertedStackOffset)
                }, function);
        }
        else if (convertedStackOffset <= -INT32_MIN && convertedStackOffset >= -INT32_MAX)
        {
            bw->writeInstructionInFunction(ModBits::Mod2 | registerBits | RmBits::EbpPlusDisp32, function);
            bw->writeDoubleWordInFunction(-convertedStackOffset, function);
        }
        else
        {
            throw std::runtime_error("The stack offset is larger than 32 bit.");
        }
    }


    void
    X86_64Writer::writeModRmAndStackOffset(LoadUnsignedInstruction* loadInstruction, FunctionRoutine* function)
    {
        writeEbpReferenceBytes(loadInstruction->stackOffset, loadInstruction->destination, function);
    }


    void
    X86_64Writer::writeAddRegisterToRegisterInstruction(AddRegisterToRegisterInstruction* instruction, FunctionRoutine* function)
    {
        writeQuadInstructionInFunction(OneByteOpCode::Add_Ev_Gv, instruction, function);
        RmBits target = getRmBitsFromOperandRegister(instruction->value);
        RegBits destination = getRegBitsFromOperandRegister(instruction->destination);
        bw->writeInstructionInFunction(ModBits::Mod3 | target | destination, function);
    }


    void
    X86_64Writer::writeAddAddressToRegisterInstruction(AddAddressToRegisterInstruction* instruction, FunctionRoutine* function)
    {
        writeQuadInstructionInFunction(OneByteOpCode::Add_Gv_Ev, instruction, function);
        writeEbpReferenceBytes(instruction->stackOffset, instruction->destination, function);
    }


    void
    X86_64Writer::writeMultiplySignedRegisterAddressInstruction(MultiplySignedAddressToRegisterInstruction* instruction, FunctionRoutine* function)
    {
        writeQuadInstructionMultipleInFunction({ OneByteOpCode::TwoByteOpCodePrefix, OneByteOpCode::Imul_Gv_Ev }, instruction, function);
        writeEbpReferenceBytes(instruction->stackOffset, instruction->destination, function);
    }


    void
    X86_64Writer::writeMultiplySignedRegisterToRegisterInstruction(MultiplySignedRegisterToRegisterInstruction* instruction, FunctionRoutine* function)
    {
        writeQuadInstructionMultipleInFunction({
            OpCodePrefix::TwoByteEscape,
            OneByteOpCode::Imul_Gv_Ev }, instruction, function);
        RmBits target = getRmBitsFromOperandRegister(instruction->value);
        RegBits destination = getRegBitsFromOperandRegister(instruction->destination);
        bw->writeInstructionInFunction(ModBits::Mod3 | target | destination, function);
    }


    void
    X86_64Writer::writeDivideUnsignedRegisterAddressInstruction(DivideUnsignedAddressToRegisterInstruction* instruction, FunctionRoutine* function)
    {
        bw->writeInstructionsInFunction({
            OneByteOpCode::Xor_Ev_Gv,
                ModBits::Mod3 | RegBits::Reg_RDX | RmBits::Rm2 },
            function);
        writeQuadInstructionInFunction(OneByteOpCode::ExtGroup3, instruction, function);
        writeEbpReferenceBytes(instruction->stackOffset, OneByteOpCode::ExtGroup3_DivRegBits, function);
    }


    void
    X86_64Writer::writeDivideSignedRegisterAddressInstruction(DivideSignedAddressToRegisterInstruction* instruction, FunctionRoutine* function)
    {
        writeQuadInstructionInFunction(OneByteOpCode::Cdq, instruction, function);
        writeQuadInstructionInFunction(OneByteOpCode::ExtGroup3, instruction, function);
        writeEbpReferenceBytes(instruction->stackOffset, OneByteOpCode::ExtGroup3_IdivRegBits, function);
    }


    void
    X86_64Writer::writeDivideSignedRegisterToRegisterInstruction(DivideSignedRegisterToRegisterInstruction* instruction, FunctionRoutine* function)
    {
        writeQuadInstructionMultipleInFunction({ OneByteOpCode::Cdq, OneByteOpCode::ExtGroup3 }, instruction, function);
        bw->writeByteInFunction(
            ModBits::Mod3 | OneByteOpCode::ExtGroup3_IdivRegBits |
            getRmBitsFromOperandRegister(instruction->value), function);
    }


    void
    X86_64Writer::writeModuloUnsignedRegisterAddressInstruction(ModuloUnsignedAddressToRegisterInstruction* instruction, FunctionRoutine* function)
    {
        writeQuadInstructionMultipleInFunction({
            OneByteOpCode::Xor_Ev_Gv,
                ModBits::Mod3 | RegBits::Reg_RDX | RmBits::Rm2 },
            instruction, function);
        writeQuadInstructionInFunction(OneByteOpCode::ExtGroup3, instruction, function);
        writeEbpReferenceBytes(instruction->stackOffset, OneByteOpCode::ExtGroup3_DivRegBits, function);
        MoveRegisterToRegisterInstruction move(OperandRegister::Scratch2, instruction->destination, instruction->destinationSize);
        writeMoveRegisterToRegisterInstruction(&move, function);
    }


    void
    X86_64Writer::writeModuloSignedRegisterAddressInstruction(ModuloSignedAddressToRegisterInstruction* instruction, FunctionRoutine* function)
    {
        writeQuadInstructionInFunction(OneByteOpCode::Cdq, instruction, function);
        writeQuadInstructionInFunction(OneByteOpCode::ExtGroup3, instruction, function);
        writeEbpReferenceBytes(instruction->stackOffset, OneByteOpCode::ExtGroup3_IdivRegBits, function);
        MoveRegisterToRegisterInstruction move(OperandRegister::Scratch2, instruction->destination, instruction->destinationSize);
        writeMoveRegisterToRegisterInstruction(&move, function);
    }


    void
    X86_64Writer::writeAddImmediateInstruction(AddImmediateToRegisterInstruction* addImmediateInstruction, FunctionRoutine* function)
    {
        writeAddImmediateInstruction(addImmediateInstruction->destination, addImmediateInstruction->value, function);
    }


    void
    X86_64Writer::writeAddImmediateInstruction(OperandRegister destination, uint64_t value, FunctionRoutine* function)
    {
        if (value <= -INT8_MIN)
        {
            bw->writeInstructionsInFunction({
                OpCodePrefix::RexMagic | OpCodePrefix::RexW,
                OneByteOpCode::ImmediateGroup1_Ev_Ib,
                static_cast<uint8_t>(ModBits::Mod3 | static_cast<uint8_t>(RegBits::ImmediateGroup1_Add) | getRmBitsFromOperandRegister(destination))
                }, function);
            bw->writeInstructionInFunction(value, function);
        }
        else
        {
            bw->writeInstructionsInFunction({
                OpCodePrefix::RexMagic | OpCodePrefix::RexW,
                OneByteOpCode::ImmediateGroup1_Ev_Iz,
                static_cast<uint8_t>(ModBits::Mod3 | static_cast<uint8_t>(RegBits::ImmediateGroup1_Add) | getRmBitsFromOperandRegister(destination))
                }, function);
            bw->writeDoubleWordInFunction(value, function);
        }
    }


    void
    X86_64Writer::writeSubtractImmediateInstruction(SubtractImmediateToRegisterInstruction* subtractImmediateInstruction, FunctionRoutine* function)
    {
        writeSubtractImmediateInstruction(subtractImmediateInstruction->destination, subtractImmediateInstruction->value, function);
    }


    void
    X86_64Writer::writeSubtractImmediateInstruction(OperandRegister destination, uint64_t value, FunctionRoutine* function)
    {
        if (value <= INT8_MAX)
        {
            bw->writeInstructionsInFunction({
                OpCodePrefix::RexMagic | OpCodePrefix::RexW,
                OneByteOpCode::ImmediateGroup1_Ev_Ib,
                static_cast<uint8_t>(ModBits::Mod3 | static_cast<uint8_t>(RegBits::ImmediateGroup1_Sub) | getRmBitsFromOperandRegister(destination))
                }, function);
            bw->writeInstructionInFunction(value, function);
        }
        else
        {
            bw->writeInstructionsInFunction({
                OpCodePrefix::RexMagic | OpCodePrefix::RexW,
                OneByteOpCode::ImmediateGroup1_Ev_Iz,
                static_cast<uint8_t>(ModBits::Mod3 | static_cast<uint8_t>(RegBits::ImmediateGroup1_Sub) | getRmBitsFromOperandRegister(destination))
                }, function);
            bw->writeDoubleWordInFunction(value, function);
        }
    }


    void
    X86_64Writer::writeSubtractRegisterAddressInstruction(SubtractRegisterToAddressInstruction* subtractRegisterAddressInstruction, FunctionRoutine* function)
    {
        writeQuadInstructionInFunction(OneByteOpCode::Sub_Gv_Ev, subtractRegisterAddressInstruction, function);
        writeEbpReferenceBytes(subtractRegisterAddressInstruction->stackOffset, subtractRegisterAddressInstruction->destination, function);
    }


    void
    X86_64Writer::writeSubtractRegisterToRegisterInstruction(SubtractRegisterToRegisterInstruction* instruction, FunctionRoutine* function)
    {
        writeQuadInstructionInFunction(OneByteOpCode::Sub_Ev_Gv, instruction, function);
        RmBits target = getRmBitsFromOperandRegister(instruction->value);
        RegBits destination = getRegBitsFromOperandRegister(instruction->destination);
        bw->writeInstructionInFunction(ModBits::Mod3 | target | destination, function);
    }


    void
    X86_64Writer::writeReturnInstruction(ReturnInstruction* returnInstruction, FunctionRoutine* function)
    {
        bw->writeInstructionInFunction(OneByteOpCode::Ret, function);
    }


    Register
    X86_64Writer::getRegisterFromOperandRegister(OperandRegister operandRegister)
    {
        switch (_callingConvention)
        {
            case CallingConvention::SysV:
                switch (operandRegister)
                {
                    case OperandRegister::Arg0:
                        return Register::Rdi;
                    case OperandRegister::Scratch0:
                        return Register::Rax;
                    case OperandRegister::Scratch1:
                        return Register::Rcx;
                    case OperandRegister::Scratch2:
                        return Register::Rdx;
                    case OperandRegister::Return:
                        return Register::Rax;
                    case OperandRegister::StackPointer:
                        return Register::Rsp;
                    case OperandRegister::FramePointer:
                        return Register::Rbp;
                    default:
                        throw std::runtime_error("Not implemented operand register.");
                }
                break;

            case CallingConvention::MicrosoftX64:
                switch (operandRegister)
                {
                    case OperandRegister::Arg0:
                        return Register::Rcx;
                    case OperandRegister::Arg1:
                        return Register::Rdx;
                    case OperandRegister::Arg2:
                        return Register::R8;
                    case OperandRegister::Arg3:
                        return Register::R9;
                    default:
                        throw std::runtime_error("Not implemented operand register.");
                }
                break;
        }
    }


    RegBits
    X86_64Writer::getRegBitsFromOperandRegister(OperandRegister operandRegister)
    {
        switch (operandRegister)
        {
            case OperandRegister::Arg0:
                return RegBits::Reg_RDI;
            case OperandRegister::Scratch0:
                return RegBits::Reg_RAX;
            case OperandRegister::Scratch1:
                return RegBits::Reg_RCX;
            case OperandRegister::Scratch2:
                return RegBits::Reg_RDX;
            case OperandRegister::FramePointer:
                return RegBits::Reg_RBP;
            case OperandRegister::StackPointer:
                return RegBits::Reg_RSP;
            case OperandRegister::Return:
                return RegBits::Reg_RAX;
            default:
                throw std::runtime_error("Not implemented operand register.");
        }
    }


    RmBits
    X86_64Writer::getRmBitsFromOperandRegister(OperandRegister operandRegister)
    {
        switch (operandRegister)
        {
            case OperandRegister::Arg0:
                return RmBits::Rm_RDI;
            case OperandRegister::Scratch0:
                return RmBits::Rm_RAX;
            case OperandRegister::Scratch1:
                return RmBits::Rm_RCX;
            case OperandRegister::Scratch2:
                return RmBits::Rm_RDX;
            case OperandRegister::FramePointer:
                return RmBits::Rm_RBP;
            case OperandRegister::StackPointer:
                return RmBits::Rm_RSP;
            case OperandRegister::Return:
                return RmBits::Rm_RAX;
            default:
                throw std::runtime_error("Not implemented operand register.");
        }
    }


    void
    X86_64Writer::writeMoveImmediateInstruction(MoveImmediateInstruction* moveImmediateInstruction, FunctionRoutine* function)
    {
        writeQuadInstructionMultipleInFunction({
            OneByteOpCode::Mov_Ev_Iz,
            static_cast<uint8_t>(ModBits::Mod3 | static_cast<uint8_t>(getRegisterFromOperandRegister(
                moveImmediateInstruction->destination)))
        }, moveImmediateInstruction, function);
        bw->writeDoubleWordInFunction(moveImmediateInstruction->value, function);
    }


    void
    X86_64Writer::writeMoveAddressInstruction(MoveAddressToRegisterInstruction* moveAddressInstruction, FunctionRoutine* function)
    {
        Register _register = getRegisterFromOperandRegister(moveAddressInstruction->destination);
        uint8_t registerBits = getRegBitsFromRegister(_register);
        bw->writeInstructionsInFunction({
            OpCodePrefix::RexMagic | OpCodePrefix::RexW,
            OneByteOpCode::Lea_Gv_M,
            static_cast<uint8_t>(ModBits::Mod0 | registerBits | RmBits::Rm5)
        }, function);
        auto constant = moveAddressInstruction->constant;
        if (std::holds_alternative<output::String*>(constant))
        {
            output::String* string = std::get<output::String*>(constant);
            string->relocationAddress.offset = _offset;
            bw->writeDoubleWordInFunction(0, function);
            _strings.add(string);
        }
        else
        {
            throw std::runtime_error("Does not support constants other than strings.");
        }
    }


    void
    X86_64Writer::writeInstructionsPadding(FunctionRoutine* function)
    {
        uint64_t rest = function->codeSize % 16;
        if (rest != 0)
        {
            writeInstructionsPadding(16 - rest);
        }
    }

    void
    X86_64Writer::setCallingConvention(CallingConvention callingConvention)
    {
        _callingConvention = callingConvention;
        switch (callingConvention)
        {
            case CallingConvention::SysV:
                _parameterRegisters = { RegBits::Reg_RDI, RegBits::Reg_RSI, RegBits::Reg_RCX, RegBits::Reg_RDX };
                break;
            case CallingConvention::MicrosoftX64:
                _parameterRegisters = { RegBits::Reg_RCX, RegBits::Reg_RDX, RegBits::Reg_R8, RegBits::Reg_R9 };
                break;
            default:
                assert("Unknown calling convention.");
        }
    }


    uint8_t
    X86_64Writer::getRegBitsFromRegister(Register _register)
    {
        switch (_register)
        {
            case Register::Rax:
            case Register::R8:
                return RegBits::Reg_RAX;
            case Register::Rcx:
            case Register::R9:
                return RegBits::Reg_RCX;
            case Register::Rdx:
            case Register::R10:
                return RegBits::Reg_RDX;
            case Register::Rbx:
            case Register::R11:
                return RegBits::Reg_RBX;
            case Register::Rbp:
            case Register::R12:
                return RegBits::Reg_RBP;
            case Register::Rsp:
            case Register::R13:
                return RegBits::Reg_RSP;
            case Register::Rsi:
            case Register::R14:
                return RegBits::Reg_RSI;
            case Register::Rdi:
            case Register::R15:
                return RegBits::Reg_RDI;
        }
    }


    void
    X86_64Writer::writeQuadInstructionInFunction(uint8_t encoding, Instruction* instruction, FunctionRoutine* function)
    {
        if (instruction->destinationSize == RegisterSize::Quad)
        {
            bw->writeInstructionInFunction(OpCodePrefix::RexMagic | OpCodePrefix::RexW, function);
        }
        bw->writeByte(encoding);
        function->codeSize += 1;
    }

    void
    X86_64Writer::writeQuadInstructionMultipleInFunction(std::initializer_list<uint8_t> encodingMultiple, Instruction* instruction,
        FunctionRoutine* function)
    {
        if (instruction->destinationSize == RegisterSize::Quad)
        {
            bw->writeInstructionInFunction(OpCodePrefix::RexMagic | OpCodePrefix::RexW, function);
        }
        for (const uint8_t encoding : encodingMultiple)
        {
            bw->writeByte(encoding);
        }
        function->codeSize += encodingMultiple.size();
    }


    void
    X86_64Writer::writeMoveZeroExtendInstruction(MoveZeroExtendInstruction* instruction, FunctionRoutine* function)
    {
        if (instruction->targetSize == RegisterSize::Byte && instruction->destinationSize == RegisterSize::Dword)
        {
            bw->writeInstructionsInFunction({
                OneByteOpCode::TwoByteOpCodePrefix,
                OneByteOpCode::Movzx_Gv_Eb }, function);
        }
        else if (instruction->targetSize == RegisterSize::Word && instruction->destinationSize == RegisterSize::Dword)
        {
            bw->writeInstructionsInFunction({
                OneByteOpCode::TwoByteOpCodePrefix,
                OneByteOpCode::Movzx_Gv_Ew }, function);
        }
        else
        {
            assert("Unsupported RegisterSize.");
        }
        RmBits destination = getRmBitsFromOperandRegister(instruction->destination);
        RegBits target = getRegBitsFromOperandRegister(instruction->target);
        bw->writeInstructionInFunction(
            ModBits::Mod3 | target | destination, function);
    }


    void
    X86_64Writer::writeMoveSignExtendInstruction(MoveSignExtendInstruction* instruction, FunctionRoutine* function)
    {
        if (instruction->targetSize == RegisterSize::Byte && instruction->destinationSize == RegisterSize::Dword)
        {
            bw->writeInstructionsInFunction(
                { OneByteOpCode::TwoByteOpCodePrefix, OneByteOpCode::Movsx_Gv_Eb }, function);
            RmBits destination = getRmBitsFromOperandRegister(instruction->destination);
            RegBits target = getRegBitsFromOperandRegister(instruction->target);
            bw->writeInstructionInFunction(
                ModBits::Mod3 | target | destination, function);
        }
        else if (instruction->targetSize == RegisterSize::Word && instruction->destinationSize == RegisterSize::Dword)
        {
            bw->writeInstructionInFunction(OneByteOpCode::Cwde, function);
        }
        else if (instruction->targetSize == RegisterSize::Dword && instruction->destinationSize == RegisterSize::Quad)
        {
            bw->writeInstructionsInFunction({
                OpCodePrefix::RexMagic | OpCodePrefix::RexW,
                OneByteOpCode::Cwde }, function);
        }
        else
        {
            assert("Unsupported RegisterSize.");
        }
    }
}
