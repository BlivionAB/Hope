#include "X86_64Writer.h"


namespace elet::domain::compiler::instruction::output
{


X86_64Writer::X86_64Writer(List<std::uint8_t>* output) :
    AssemblyWriterInterface(output)
{

}


void
X86_64Writer::writeStubs()
{
    for (const auto& routine : _externalRoutines)
    {
        auto stubAddress = _currentOffset;
        _bw->writeByte(OP_EXT_GROUP5);
        _bw->writeByte(OP_EXT_GROUP5_FAR_CALL_REG_BITS | MOD_DISP0 | RM_EBP);
        for (const auto& relocationAddress: routine->relocationAddresses)
        {
            _bw->writeDoubleWordAtAddress(stubAddress - relocationAddress, relocationAddress);
        }
        _bw->writeDoubleWord(0);
    }
}


void
X86_64Writer::writeStubHelper()
{
    auto top = _currentOffset;
    // leaq dyld_private, r11
    _bw->writeByte(REX_PREFIX_MAGIC | REX_PREFIX_W | REX_PREFIX_R);
    _bw->writeByte(OP_LEA_Gv_M);
    _bw->writeByte(MOD_DISP0 | RM_EBP | REG_EBX);
    _dataDataRelocationAddress = _currentOffset;
    _bw->writeDoubleWord(0);

    // push r11
    _bw->writeByte(REX_PREFIX_MAGIC | REX_PREFIX_B);
    _bw->writeByte(OP_PUSH_rBX);

    // jmpq dyld_stub_binder
    _bw->writeByte(OP_EXT_GROUP5);
    _bw->writeByte(OP_EXT_GROUP5_FAR_CALL_REG_BITS | MOD_DISP0 | RM_EBP);
    _dyldStubBinderRelocationAddress = _currentOffset;
    _bw->writeDoubleWord(0);

    // nop
    _bw->writeByte(OP_NOP);

    for (const auto& routine : _externalRoutines)
    {
        _bw->writeByte(OP_PUSH_Iz);
        _dyldStubOffsetRelocationAddress.add(_currentOffset);
        _bw->writeDoubleWord(0);
        _bw->writeByte(OP_NEAR_JMP_Jz);
        _bw->writeDoubleWord(top - _currentOffset - 4);
    }
}


void
X86_64Writer::writeTextSection(FunctionRoutine* routine)
{
    writeFunctionRoutine(routine);
}


void
X86_64Writer::writeFunctionRoutine(FunctionRoutine* routine)
{
    if (routine->hasWrittenOutput)
    {
        writeFunctionRelocationAddresses(routine);
        return;
    }
    routine->offset = _currentOffset;
    writeFunctionRelocationAddresses(routine);
    writeFunctionPrelude();
    writeFunctionParameters(routine);
    writeFunctionInstructions(routine);
    writeFunctionPostlude();
    for (const auto& subRoutine : routine->subRoutines)
    {
        writeFunctionRoutine(subRoutine);
    }
    routine->hasWrittenOutput = true;
}


void
X86_64Writer::writeFunctionRelocationAddresses(FunctionRoutine* routine)
{
    for (const auto& reloactionAddress : routine->relocationAddresses)
    {
        _bw->writeDoubleWordAtAddress(routine->offset - reloactionAddress.second, reloactionAddress.first);
    }
    routine->relocationAddresses.clear();
}


void
X86_64Writer::writeFunctionInstructions(FunctionRoutine* routine)
{
    for (const auto& instruction : routine->instructions)
    {
        switch (instruction->kind)
        {
            case InstructionKind::Call:
                writeCallInstruction(reinterpret_cast<CallInstruction*>(instruction), routine);
                break;
        }
    }
}


void
X86_64Writer::writeCallInstruction(CallInstruction* callInstruction, FunctionRoutine* parentRoutine)
{
    writeCallInstructionArguments(callInstruction);
    _bw->writeByte(OP_CALL_NEAR);
    switch (callInstruction->routine->kind)
    {
        case RoutineKind::Function:
        {
            auto routine = reinterpret_cast<FunctionRoutine*>(callInstruction->routine);
            if (routine->offset)
            {
                _bw->writeDoubleWord(routine->offset - _currentOffset);
            }
            else
            {
                routine->relocationAddresses.add({ _currentOffset, _currentOffset + 4 });
                _bw->writeDoubleWord(0);
                parentRoutine->subRoutines.add(routine);
            }
            break;
        }
        case RoutineKind::External:
        {
            auto routine = reinterpret_cast<ExternalRoutine*>(callInstruction->routine);
            routine->relocationAddresses.add(_currentOffset);
            writePointer(0);
            _externalRoutines.add(routine);
            break;
        }
        default:
            throw std::exception();
    }
}


void
X86_64Writer::writePointer(std::uint64_t address)
{
    _bw->writeDoubleWord(address);
}


void
X86_64Writer::writeCallInstructionArguments(CallInstruction* callInstruction)
{
    unsigned int i = 0;
    for (const auto& argument : callInstruction->arguments)
    {
        auto reg = _callingConvention.registers[i];
        if (auto parameter = std::get_if<ParameterDeclaration*>(&argument->value))
        {
            writeMoveFromOffset(reg, (*parameter)->stackOffset);
        }
        else if (auto localVariable = std::get_if<LocalVariableDeclaration*>(&argument->value))
        {
            writeMoveFromOffset(reg, (*localVariable)->stackOffset);
        }
        else if (auto string = std::get_if<String*>(&argument->value))
        {
            _bw->writeByte(REX_PREFIX_MAGIC | REX_PREFIX_W);
            _bw->writeByte(OP_LEA_Gv_M);
            _bw->writeByte(reg | RM_EBP | MOD_DISP0);
            (*string)->relocationAddress = _currentOffset;
            _bw->writeDoubleWord(0);
            _strings.add(*string);
        }
        ++i;
    }
}


void
X86_64Writer::writeMoveFromOffset(uint8_t reg, size_t offset)
{
    _bw->writeByte(REX_PREFIX_MAGIC | REX_PREFIX_W);
    _bw->writeByte(OP_MOV_Gv_Ev);
    _bw->writeByte(MODRM_EBP_DISP8 | reg);
    _bw->writeByte(-offset);
}


void
X86_64Writer::writeFunctionParameters(const FunctionRoutine* routine)
{
    _localStackOffset = 0;
    for (unsigned int i = 0; i < routine->parameters.size(); ++i)
    {
        auto parameter = routine->parameters[i];
        if (i < _callingConvention.registers.size())
        {
            writeParameter(parameter->size, i);
        }
    }
}


void
X86_64Writer::writeParameter(size_t size, unsigned int index)
{
    _localStackOffset += size;
    _bw->writeByte(OP_MOV_Ev_Gv);
    _bw->writeByte(MODRM_EBP_DISP8 | _callingConvention.registers[index]);
    _bw->writeByte(-_localStackOffset);
}


void
X86_64Writer::writeFunctionPrelude()
{
    _bw->writeByte(OP_PUSH_rBP);
    _bw->writeByte(REX_PREFIX_MAGIC | REX_PREFIX_W);
    _bw->writeByte(OP_MOV_Ev_Gv);
    _bw->writeByte(MODRM_EBP | REG_ESP);
}


void
X86_64Writer::writeFunctionPostlude()
{
    _bw->writeByte(OP_POP_rBP);
    _bw->writeByte(OP_RET);
}


}
