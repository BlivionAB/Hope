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
        writeByte(OP_EXT_GROUP5);
        writeByte(OP_EXT_GROUP5_FAR_CALL_REG_BITS | MOD_DISP0 | RM_EBP);
        routine->stubRelocationAddress = _currentOffset;
        writeDoubleWord(0);
    }
}


void
X86_64Writer::writeStubHelper()
{
    auto top = _currentOffset;
    // leaq dyld_private, r11
    writeByte(REX_PREFIX_MAGIC | REX_PREFIX_W | REX_PREFIX_R);
    writeByte(OP_LEA_Gv_M);
    writeByte(MOD_DISP0 | RM_EBP | REG_EBX);
    __dataDataRelocationAddress = _currentOffset;
    writeDoubleWord(0);

    // push r11
    writeByte(REX_PREFIX_MAGIC | REX_PREFIX_B);
    writeByte(OP_PUSH_rBX);

    // nop
    writeByte(OP_NOP);

    for (const auto& routine : _externalRoutines)
    {


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
    if (routine->hasWrittenOutput) {
        return;
    }
    routine->offset = _currentOffset;
    writeFunctionPrelude();
    writeFunctionParameters(routine);
    writeFunctionInstructions(routine);
    writeFunctionPostlude();
    routine->hasWrittenOutput = true;
}


void
X86_64Writer::writeFunctionInstructions(const FunctionRoutine* routine)
{
    for (const auto& instruction : routine->instructions)
    {
        switch (instruction->kind)
        {
            case InstructionKind::Call:
                writeCallInstruction(reinterpret_cast<CallInstruction*>(instruction));
                break;
        }
    }
}


void
X86_64Writer::writeCallInstruction(CallInstruction* callInstruction)
{
    writeCallInstructionArguments(callInstruction);
    writeByte(OP_CALL_NEAR);
    switch (callInstruction->routine->kind)
    {
        case RoutineKind::Function:
        {
            auto routine = reinterpret_cast<FunctionRoutine*>(callInstruction->routine);
            if (routine->offset)
            {
                writeDoubleWord(routine->offset - _currentOffset);
            }
            else
            {
                writeDoubleWord(_currentOffset + OFFSET_SIZE - 1 /* -1 is for OP_CALL_NEAR*/);
            }
            writeFunctionRoutine(routine);
            break;
        }
        case RoutineKind::External:
        {
            auto routine = reinterpret_cast<ExternalRoutine*>(callInstruction->routine);
            routine->relocationAddress = _currentOffset;
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
    writeQuadWord(address);
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
            writeByte(REX_PREFIX_MAGIC | REX_PREFIX_W);
            writeByte(OP_LEA_Gv_M);
            writeByte(reg | RM_EBP | MOD_DISP0);
            (*string)->relocationAddress = _currentOffset;
            writeDoubleWord(0);
            _strings.add(*string);
        }
        ++i;
    }
}


void
X86_64Writer::writeMoveFromOffset(uint8_t reg, size_t offset)
{
    writeByte(OP_MOV_Gv_Ev);
    writeByte(MODRM_EBP_DISP8 | reg);
    writeDoubleWord(-offset);
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
    writeByte(OP_MOV_Ev_Gv);
    writeByte(MODRM_EBP_DISP8 | _callingConvention.registers[index]);
    writeByte(-_localStackOffset);
}


void
X86_64Writer::writeFunctionPrelude()
{
    writeByte(OP_PUSH_rBP);
    writeByte(REX_PREFIX_MAGIC | REX_PREFIX_W);
    writeByte(OP_MOV_Ev_Gv);
    writeByte(MODRM_EBP | REG_ESP);
}


void
X86_64Writer::writeFunctionPostlude()
{
    writeByte(OP_POP_rBP);
    writeByte(OP_RET);
}


}
