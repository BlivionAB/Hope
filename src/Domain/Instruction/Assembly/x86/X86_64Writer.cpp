//
// Created by Tien Heng Ho on 2020-07-25.
//

#include "X86_64Writer.h"


namespace elet::domain::compiler::instruction::output
{


X86_64Writer::X86_64Writer(std::map<output::Routine*, List<std::uint8_t>*> &output, std::mutex& workMutex):
    AssemblyWriterInterface(output, workMutex)
{

}


List<std::uint8_t>*
X86_64Writer::writeRoutine(Routine *routine)
{
    _output = new List<std::uint8_t>();
    if (routine->kind == RoutineKind::Routine)
    {
        return _output;
    }
    else
    {
        for (auto instruction : *routine->instructions)
        {
            switch (instruction->type)
            {
                case embedded::InstructionType::SysCall:
                    _output->add(OPCODE_TWO_BYTE_PREFIX);
                    _output->add(OPCODE_SYSCALL);
                    break;
                case embedded::InstructionType::Store64:
                    _output->add(REX_PREFIX_MAGIC | REX_PREFIX_W);
                case embedded::InstructionType::Store32:
                    if (instruction->operand1->kind == OperandKind::Register && instruction->operand2->kind == OperandKind::Int)
                    {
                        auto _register = reinterpret_cast<output::Register*>(instruction->operand1);
                        writeMoveImmediateOpcode(_register->index);
                    }
                    break;
            }
        }
        return _output;
    }
}


void
X86_64Writer::writeMoveImmediateOpcode(unsigned int registerIndex)
{
    switch (registerIndex)
    {
        case 1:
            _output->add(OPCODE_MOVE_IMMEDIATE_VALUE_TO_RAX);
            break;
        case 2:
            _output->add(OPCODE_MOVE_IMMEDIATE_VALUE_TO_RCX);
            break;
        case 3:
            _output->add(OPCODE_MOVE_IMMEDIATE_VALUE_TO_RDX);
            break;
        case 4:
            _output->add(OPCODE_MOVE_IMMEDIATE_VALUE_TO_RBX);
            break;
        case 5:
            _output->add(OPCODE_MOVE_IMMEDIATE_VALUE_TO_RSP);
            break;
        case 6:
            _output->add(OPCODE_MOVE_IMMEDIATE_VALUE_TO_RBP);
            break;
        case 7:
            _output->add(OPCODE_MOVE_IMMEDIATE_VALUE_TO_RSI);
            break;
        case 8:
            _output->add(OPCODE_MOVE_IMMEDIATE_VALUE_TO_RDI);
            break;
        default:
            throw UnknownRegisterIndex();
    }
}

}
