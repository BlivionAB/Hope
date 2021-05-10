#include "X86_64Writer.h"


namespace elet::domain::compiler::instruction::output
{


X86_64Writer::X86_64Writer():
    AssemblyWriterInterface()
{

}


void
X86_64Writer::writeStartRoutine(FunctionRoutine* routine, std::size_t offset)
{
    _currentOffset = offset;
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
    switch (callInstruction->routine->kind)
    {
        case RoutineKind::Function:
            writeFunctionRoutine(reinterpret_cast<FunctionRoutine*>(callInstruction->routine));
            break;
        case RoutineKind::External:
            callInstruction->offset = _currentOffset;
            writePointer(0);
            break;
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
        if (auto parameter = std::get_if<ParameterDeclaration*>(&argument->value))
        {
            writeMoveFromOffset(_callingConvention.registers[i], (*parameter)->stackOffset);
        }
        else if (auto localVariable = std::get_if<LocalVariableDeclaration*>(&argument->value))
        {
            writeMoveFromOffset(_callingConvention.registers[i], (*localVariable)->stackOffset);
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


void
X86_64Writer::writeQuadWord(std::uint64_t instruction)
{
    _currentOffset += 8;
    _routineOutput->add((std::uint8_t)instruction & (std::uint8_t)0xff);
    _routineOutput->add((std::uint8_t)(instruction >> (std::uint8_t)8) & (std::uint8_t)0xff);
    _routineOutput->add((std::uint8_t)(instruction >> (std::uint8_t)16) & (std::uint8_t)0xff);
    _routineOutput->add((std::uint8_t)(instruction >> (std::uint8_t)24) & (std::uint8_t)0xff);
    _routineOutput->add((std::uint8_t)(instruction >> (std::uint8_t)32) & (std::uint8_t)0xff);
    _routineOutput->add((std::uint8_t)(instruction >> (std::uint8_t)40) & (std::uint8_t)0xff);
    _routineOutput->add((std::uint8_t)(instruction >> (std::uint8_t)48) & (std::uint8_t)0xff);
    _routineOutput->add((std::uint8_t)(instruction >> (std::uint8_t)56) & (std::uint8_t)0xff);
}


void
X86_64Writer::writeDoubleWord(std::uint32_t instruction)
{
    _currentOffset += 4;
    _routineOutput->add(instruction & (std::uint8_t)0xff);
    _routineOutput->add((instruction >> (std::uint8_t)8) & (std::uint8_t)0xff);
    _routineOutput->add((instruction >> (std::uint8_t)16) & (std::uint8_t)0xff);
    _routineOutput->add((instruction >> (std::uint8_t)24) & (std::uint8_t)0xff);
}


void
X86_64Writer::writeByte(std::uint8_t instruction)
{
    ++_currentOffset;
    _routineOutput->add(instruction);
}

}
