#include "X86_64Writer.h"
#include <cassert>

namespace elet::domain::compiler::instruction::output
{


X86_64Writer::X86_64Writer(List<std::uint8_t>* output) :
    AssemblyWriterInterface(output)
{

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
        bw->writeByte(ExtGroup5_FarCallRegistryBits | MOD_DISP0 | Rm5);
        routine->stubAddress = _offset;
        bw->writeDoubleWord(0);
    }
}


void
X86_64Writer::writeStubHelper()
{
    auto top = _offset;
    // leaq dyld_private, r11
    bw->writeByte(REX_PREFIX_MAGIC | REX_PREFIX_W | REX_PREFIX_R);
    bw->writeByte(OpCode::Lea_Gv_M);
    bw->writeByte(MOD_DISP0 | Rm::Rm5 | OpCodeRegister::Reg3);
    dyldPrivateOffset = _offset;
    bw->writeDoubleWord(0);

    // push r11
    bw->writeByte(REX_PREFIX_MAGIC | REX_PREFIX_B);
    bw->writeByte(OpCode::Push_rBX);

    // jmpq dyld_stub_binder
    bw->writeByte(OpCode::ExtGroup5);
    bw->writeByte(OpCode::ExtGroup5_FarCallRegistryBits | MOD_DISP0 | Rm5);
    _dyldStubBinderRelocationAddress = _offset;
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
        bw->writeByte(Push_Iz);
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
X86_64Writer::writeFunction(FunctionRoutine* routine)
{
    if (routine->hasWrittenOutput)
    {
        writeFunctionRelocationAddresses(routine);
        return;
    }
    routine->offset = _offset;
    writeFunctionRelocationAddresses(routine);
    uint64_t stackSize = getStackSizeFromFunctionParameters(routine);
    uint64_t stackOffset = 0;
    uint64_t routineSize = 0;
    routineSize += writeFunctionPrologue(stackSize);
    routineSize += writeFunctionParameters(routine, stackOffset);
    routineSize += writeFunctionInstructions(routine);
    if (routine->isStartFunction)
    {
        bw->writeByte(OpCode::Xor_Eb_Gb);
        bw->writeByte(Mod::Mod3 | OpCodeRegister::Reg_RAX | Rm::Rm0);
        routineSize += 2;
    }
    writeFunctionEpilogue(stackSize, routineSize);
    for (const auto& subRoutine : routine->subRoutines)
    {
        writeFunction(subRoutine);
    }
    routine->hasWrittenOutput = true;
    if (routine->isStartFunction)
    {
        exportedRoutines.add(routine);
    }
    internalRoutines.add(routine);
}


void
X86_64Writer::writeFunctionRelocationAddresses(FunctionRoutine* routine)
{
    for (const auto& reloactionAddress : routine->relocationAddresses)
    {
        bw->writeDoubleWordAtAddress(routine->offset - reloactionAddress.second, reloactionAddress.first);
    }
    routine->relocationAddresses.clear();
}


uint64_t
X86_64Writer::writeFunctionInstructions(FunctionRoutine* routine)
{
    size_t size = 0;
    for (const auto& instruction : routine->instructions)
    {
        switch (instruction->kind)
        {
            case InstructionKind::Call:
                size += writeCallInstruction(reinterpret_cast<CallInstruction*>(instruction), routine);
                break;
        }
    }
    return size;
}


uint64_t
X86_64Writer::writeCallInstruction(CallInstruction* callInstruction, FunctionRoutine* parentRoutine)
{
    uint64_t size = 0;
    size += writeCallInstructionArguments(callInstruction);
    bw->writeByte(OP_CALL_NEAR);
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
                routine->relocationAddresses.add({ _offset, _offset + 4 });
                bw->writeDoubleWord(0);
                parentRoutine->subRoutines.add(routine);
            }
            size += 5;
            return size;
        }
        case RoutineKind::External:
        {
            auto routine = reinterpret_cast<ExternalRoutine*>(callInstruction->routine);
            routine->relocationAddresses.add(_offset);
            bw->writeDoubleWord(0);
            externalRoutines.add(routine);
            size += 5;
            return size;
        }
        default:
            throw std::exception();
    }
}


void
X86_64Writer::writePointer(std::uint64_t address)
{
    bw->writeDoubleWord(address);
}


uint64_t
X86_64Writer::writeCallInstructionArguments(CallInstruction* callInstruction)
{
    uint64_t size = 0;
    unsigned int i = 0;
    for (const auto& argument : callInstruction->arguments)
    {
        auto reg = _callingConvention.registers[i];
        if (auto parameter = std::get_if<ParameterDeclaration*>(&argument->value))
        {
            writeMoveFromOffset(reg, (*parameter)->stackOffset);
            size += 4;
        }
        else if (auto localVariable = std::get_if<LocalVariableDeclaration*>(&argument->value))
        {
            writeMoveFromOffset(reg, (*localVariable)->stackOffset);
            size += 4;
        }
        else if (auto string = std::get_if<String*>(&argument->value))
        {
            bw->writeByte(REX_PREFIX_MAGIC | REX_PREFIX_W);
            bw->writeByte(Lea_Gv_M);
            bw->writeByte(reg | Rm5 | MOD_DISP0);
            (*string)->relocationAddress = _offset;
            bw->writeDoubleWord(0);
            size += 7;
            _strings.add(*string);
        }
        ++i;
    }
    return size;
}


void
X86_64Writer::writeMoveFromOffset(uint8_t reg, size_t offset)
{
    assert(("Offset must be smaller then INT8_MAX", offset < INT8_MAX));
    bw->writeByte(REX_PREFIX_MAGIC | REX_PREFIX_W);
    bw->writeByte(OP_MOV_Gv_Ev);
    bw->writeByte(MODRM_EBP_DISP8 | reg);
    bw->writeByte(-offset);
}


uint64_t
X86_64Writer::writeFunctionParameters(const FunctionRoutine* routine, uint64_t& stackOffset)
{
    uint64_t size = 0;
    for (unsigned int i = 0; i < routine->parameters.size(); ++i)
    {
        auto parameter = routine->parameters[i];
        if (i < _callingConvention.registers.size())
        {
            size += writeParameter(parameter->size, i, stackOffset);
            parameter->stackOffset = stackOffset;
        }
    }
    return size;
}


uint64_t
X86_64Writer::getStackSizeFromFunctionParameters(const FunctionRoutine* routine)
{
    uint64_t stackOffset = 0;
    for (unsigned int i = 0; i < routine->parameters.size(); ++i)
    {
        auto parameter = routine->parameters[i];
        if (i < _callingConvention.registers.size())
        {
            stackOffset += parameter->size;
        }
    }
    uint64_t rest = stackOffset % 16;
    if (rest != 0)
    {
        stackOffset += 16 - rest;
    }
    return stackOffset;
}


uint64_t
X86_64Writer::writeParameter(uint64_t size, unsigned int index, uint64_t& stackOffset)
{
    stackOffset += size;
    assert(("_localStackOffset must be smaller then INT8_MAX", stackOffset < INT8_MAX));
    bw->writeByte(REX_PREFIX_MAGIC | REX_PREFIX_W);
    bw->writeByte(OP_MOV_Ev_Gv);
    bw->writeByte(MODRM_EBP_DISP8 | _callingConvention.registers[index]);
    bw->writeByte(-stackOffset);
    return 4;
}


uint64_t
X86_64Writer::writeFunctionPrologue(size_t stackSize)
{
    uint64_t size = 4;
    bw->writeByte(Push_rBP);
    bw->writeByte(REX_PREFIX_MAGIC | REX_PREFIX_W);
    bw->writeByte(OP_MOV_Ev_Gv);
    bw->writeByte(MODRM_EBP | Reg4);

    // Subtract RSP
    if (stackSize)
    {
        bw->writeByte(REX_PREFIX_MAGIC | REX_PREFIX_W);
        bw->writeByte(OpCodeExtensionGroup1::Ev_lb);
        bw->writeByte(OpCodeRegister::Reg5 | Mod::Mod3 | Rm::Rm4);
        _subtractStackAddress = _offset;
        bw->writeByte(stackSize);
        size += 4;
    }
    return size;
}


void
X86_64Writer::writeFunctionEpilogue(size_t stackSize, uint64_t routineSize)
{
    // Add back RSP
    if (stackSize)
    {
        bw->writeByte(RexPrefix::REX_PREFIX_MAGIC | RexPrefix::REX_PREFIX_W);
        bw->writeByte(OpCodeExtensionGroup1::Ev_lb);
        bw->writeByte(OpCodeRegister::Reg0 | Mod::Mod3 | Rm::Rm4);
        bw->writeByte(stackSize);
        routineSize += 4;
    }

    bw->writeByte(Pop_rBP);
    bw->writeByte(Ret);
    routineSize += 2;

    uint64_t rest = routineSize % 16;
    if (rest != 0)
    {
        writeInstructionsPadding(16 - rest);
    }
}

/// Check the chapter instruction set reference — NOP chapter.
/// https://www.intel.com/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-instruction-set-reference-manual-325383.pdf
void
X86_64Writer::writeInstructionsPadding(uint64_t length)
{
    switch (length)
    {
        case 1:
            bw->writeByte(OpCode::Nop);
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


}
