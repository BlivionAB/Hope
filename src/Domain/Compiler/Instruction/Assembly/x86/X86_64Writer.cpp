#include "X86_64Writer.h"
#include <cassert>

namespace elet::domain::compiler::instruction::output
{


X86_64Writer::X86_64Writer(List<std::uint8_t>* output) :
    AssemblyWriterInterface(output)
{
    _callingConvention = { { Reg7, Reg6, Reg2, Reg1 } };
}


void
X86_64Writer::writeStubs()
{
    for (const auto& routine : externalRoutines)
    {
        for (const auto& relocationAddress : routine->relocationAddresses)
        {
            bw->writeDoubleWordAtAddress(_offset - (relocationAddress + 4), relocationAddress);
        }
        bw->writeByte(ExtGroup5);
        bw->writeByte(ExtGroup5_NearCallRegistryBits | ModBits::Mod0 | Rm5);
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
    bw->writeByte(ModBits::Mod0 | RmBits::Rm5 | RegisterBits::Reg3);
    dyldPrivateOffset = _offset;
    bw->writeDoubleWord(0);

    // push r11
    bw->writeByte(RexMagic | RexB);
    bw->writeByte(OneByteOpCode::Push_rBX);

    // jmpq dyld_stub_binder
    bw->writeByte(OneByteOpCode::ExtGroup5);
    bw->writeByte(OneByteOpCode::ExtGroup5_NearCallRegistryBits | ModBits::Mod0 | Rm5);
    Utf8String* dyldStubBinderString = new Utf8String("dyld_stub_binder");
    Utf8StringView string = Utf8StringView(*dyldStubBinderString);
    ExternalRoutine* dyldStubBinderRoutine = new ExternalRoutine(string);
    gotBoundRoutines.add(dyldStubBinderRoutine);
    dyldStubBinderRoutine->relocationAddresses.add(_offset);
    bw->writeDoubleWord(0);

    // nop
    bw->writeByte(Nop);

    for (const auto& routine : externalRoutines)
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
    bw->writeInstructionsInFunction({
                                        OpCodePrefix::RexMagic | OpCodePrefix::RexW,
                                        OneByteOpCode::Mov_Gv_Ev}, function);
    writeEbpReferenceBytes(storeRegisterInstruction->stackOffset, storeRegisterInstruction->target, function);
}


void
X86_64Writer::writePushInstruction(PushInstruction* pushInstruction, FunctionRoutine* function)
{
    bw->writeByteInFunction(OneByteOpCode::Push_rBP, function);
}


void
X86_64Writer::writePopInstruction(PopInstruction* popInstruction, FunctionRoutine* function)
{
    bw->writeByteInFunction(OneByteOpCode::Pop_rBP, function);
}


void
X86_64Writer::writeStoreImmediateInstruction(StoreImmediateInstruction* storeImmediateInstruction, FunctionRoutine* function)
{
    bw->writeByte(OneByteOpCode::Mov_Ev_Iz);
    bw->writeByte(ModBits::Mod1 | RmBits::Rm5);
    bw->writeByte(-storeImmediateInstruction->stackOffset);
    bw->writeDoubleWord(std::get<int32_t>(storeImmediateInstruction->value));
    function->codeSize += 7;
}


void
X86_64Writer::writeCallInstruction(CallInstruction* callInstruction, FunctionRoutine* function)
{
    bw->writeByte(CallNear);
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
            routine->relocationAddresses.add(_offset);
            bw->writeDoubleWord(0);
            externalRoutines.add(routine);
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
X86_64Writer::writeMoveRegisterInstruction(MoveRegisterInstruction* moveRegisterInstruction, FunctionRoutine* function)
{
    // It's an unnecessary move, since they are the same register.
    if (moveRegisterInstruction->destination == OperandRegister::Return && moveRegisterInstruction->target == OperandRegister::Left)
    {
        return;
    }
    bw->writeInstructionsInFunction({
                                        OpCodePrefix::RexMagic | OpCodePrefix::RexW,
                                        OneByteOpCode::Mov_Ev_Gv,
                                        static_cast<uint8_t>(ModBits::Mod3
                                                             | getRegisterBitsFromOperandRegister(
                                            moveRegisterInstruction->destination)
                                                             | getRmBitsFromOperandRegister(
                                            moveRegisterInstruction->target))
                                    }, function);
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
X86_64Writer::writeLoadInstruction(LoadInstruction* loadInstruction, FunctionRoutine* function)
{
    if (loadInstruction->destination == OperandRegister::Right && nextInstructionIs(InstructionKind::AddRegister))
    {
        bw->writeByte(OneByteOpCode::Add_Gv_Ev);
        function->codeSize += 1;
        writeModRmAndStackOffset(loadInstruction, function);

        // Skip next instruction, since we already processed it.
        _instructionIterator->next();
        return;
    }

    bw->writeByte(OpCodePrefix::RexMagic | OpCodePrefix::RexW);
    bw->writeByte(OneByteOpCode::Mov_Ev_Gv);
    function->codeSize += 2;
    writeModRmAndStackOffset(loadInstruction, function);
}


void
X86_64Writer::writeEbpReferenceBytes(uint64_t stackOffset, OperandRegister operandRegister, FunctionRoutine* function)
{
    RegisterBits registerBits = getRegisterBitsFromOperandRegister(operandRegister);
    int64_t convertedStackOffset = static_cast<int64_t>(stackOffset);
    if (convertedStackOffset <= INT8_MAX && convertedStackOffset >= INT8_MIN)
    {
        bw->writeByte(ModBits::Mod1 | registerBits | RmBits::EbpPlusDisp8);
        bw->writeByte(-convertedStackOffset);
        function->codeSize += 2;
    }
    else if (convertedStackOffset <= INT32_MAX && convertedStackOffset >= INT32_MIN)
    {
        bw->writeByte(ModBits::Mod2 | registerBits | RmBits::EbpPlusDisp32);
        bw->writeDoubleWord(-convertedStackOffset);
        function->codeSize += 5;
    }
    else
    {
        throw std::runtime_error("The stack offset is larger than 32 bit.");
    }
}


void
X86_64Writer::writeModRmAndStackOffset(LoadInstruction* loadInstruction, FunctionRoutine* function)
{
    writeEbpReferenceBytes(loadInstruction->stackOffset, loadInstruction->destination, function);
}


void
X86_64Writer::writeAddRegisterInstruction(AddRegisterInstruction* addRegisterInstruction, FunctionRoutine* function)
{
    throw std::runtime_error("Not implemented");
}


void
X86_64Writer::writeReturnInstruction(ReturnInstruction* returnInstruction, FunctionRoutine* function)
{
    bw->writeByteInFunction(OneByteOpCode::Ret, function);
}


RegisterBits
X86_64Writer::getRegisterBitsFromOperandRegister(OperandRegister operandRegister)
{
    switch (operandRegister)
    {
        case OperandRegister::Arg0:
            return RegisterBits::Reg_RDI;
        case OperandRegister::Left:
            return RegisterBits::Reg_RAX;
        case OperandRegister::Right:
            return RegisterBits::Reg_RCX;
        case OperandRegister::Return:
            return RegisterBits::Reg_RAX;
        case OperandRegister::StackPointer:
            return RegisterBits::Reg_RSP;
        case OperandRegister::FramePointer:
            return RegisterBits::Reg_RBP;
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
        case OperandRegister::Left:
            return RmBits::Rm_RAX;
        case OperandRegister::Right:
            return RmBits::Rm_RBX;
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
X86_64Writer::writeMoveAddressInstruction(MoveAddressInstruction* moveAddressInstruction, FunctionRoutine* function)
{
    bw->writeInstructionsInFunction({
        OpCodePrefix::RexMagic | OpCodePrefix::RexW,
        OneByteOpCode::Lea_Gv_M,
        static_cast<uint8_t>(ModBits::Mod0 | getRegisterBitsFromOperandRegister(moveAddressInstruction->destination) | RmBits::Rm5)
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

}
