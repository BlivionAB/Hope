#include "X86_64Writer.h"


namespace elet::domain::compiler::instruction::output
{


void
X86_64Writer::writeRoutine(Routine *routine)
{
    _routineOutput = new List<std::uint8_t>();
    if (routine->kind == RoutineKind::Function)
    {
        _routineOutput->add(OP_PUSH_rBP);
        _routineOutput->add(REX_PREFIX_MAGIC | REX_PREFIX_W);
        _routineOutput->add(OP_MOV_Ev_Gv);
        _routineOutput->add(AF32__EBP__TO__ESP);
        auto _function = reinterpret_cast<Function*>(routine);
        std::size_t parameterOffset = 0;
        for (auto parameter : _function->parameters)
        {
//            parameterOffset += parameter->size;
//            _routineOutput->add(OP_MOV_Ev_Gv);
//            switch (parameter->index)
//            {
//                case 0:
//                    if (parameterOffset < DISP8_MAX_VALUE)
//                    {
//                        _routineOutput->add(AF32__EBP_PLUS_DISP8__TO__EDI);
//                    }
//                    else
//                    {
//                        _routineOutput->add(AF32__EBP_PLUS_DISP32__TO__EDI);
//                    }
//                    break;
//            }
        }
        for (auto instruction : *_function->instructions)
        {
            switch (instruction->type)
            {
                case embedded::InstructionType::SysCall:
                    _routineOutput->add(OPCODE_TWO_BYTE_PREFIX);
                    _routineOutput->add(OPCODE_SYSCALL);
                    break;
                case embedded::InstructionType::Store64:
//                    _routineOutput->add(REX_PREFIX_MAGIC | REX_PREFIX_W);
                case embedded::InstructionType::Store32:
                    if (instruction->operand1->kind == OperandKind::Register)
                    {
                        auto _register = reinterpret_cast<Register*>(instruction->operand1);
                        if (instruction->operand2->kind == OperandKind::Int32)
                        {
                            auto int32 = reinterpret_cast<Int32*>(instruction->operand2);
                            writeMoveInt32ToRegisterOpcode(_register->index, int32);
                        }
                        else if (instruction->operand2->kind == OperandKind::Int64)
                        {
                            // Put this here temporarily, unitl we have support for references.
                            _routineOutput->add(REX_PREFIX_MAGIC | REX_PREFIX_W);

                            auto int64 = reinterpret_cast<Int64*>(instruction->operand2);
                            writeMoveInt64ToRegisterOpcode(_register->index, int64);
                        }
                    }
                    break;
            }
        }
    }

    routine->machineInstructions = _routineOutput;
}

void
X86_64Writer::writeMoveImmediateOpcode(unsigned int registerIndex)
{
    switch (registerIndex)
    {
        case 0:
            _routineOutput->add(OP_MOV_rAX_Iv);
            break;
        case 1:
            _routineOutput->add(OP_MOV_rCX_Iv);
            break;
        case 2:
            _routineOutput->add(OP_MOV_rBX_Iv);
            break;
        case 3:
            _routineOutput->add(OP_MOV_rDX_Iv);
            break;
        case 4:
            _routineOutput->add(OP_MOV_rSP_Iv);
            break;
        case 5:
            _routineOutput->add(OP_MOV_rBP_Iv);
            break;
        case 6:
            _routineOutput->add(OP_MOV_rSI_Iv);
            break;
        case 7:
            _routineOutput->add(OP_MOV_rDI_Iv);
            break;
        default:
            throw UnknownRegisterIndex();
    }
}


void
X86_64Writer::addInt32(Int32* value)
{
    std::uint32_t integer = value->value;
    _routineOutput->add(integer & (std::uint8_t)0xff);
    _routineOutput->add((integer >> (std::uint8_t)8) & (std::uint8_t)0xff);
    _routineOutput->add((integer >> (std::uint8_t)16) & (std::uint8_t)0xff);
    _routineOutput->add((integer >> (std::uint8_t)24) & (std::uint8_t)0xff);
}


void
X86_64Writer::addInt64(Int64* value)
{
    std::uint64_t integer = value->value;
    _routineOutput->add((std::uint8_t)integer & (std::uint8_t)0xff);
    _routineOutput->add((std::uint8_t)(integer >> (std::uint8_t)8) & (std::uint8_t)0xff);
    _routineOutput->add((std::uint8_t)(integer >> (std::uint8_t)16) & (std::uint8_t)0xff);
    _routineOutput->add((std::uint8_t)(integer >> (std::uint8_t)24) & (std::uint8_t)0xff);
    _routineOutput->add((std::uint8_t)(integer >> (std::uint8_t)32) & (std::uint8_t)0xff);
    _routineOutput->add((std::uint8_t)(integer >> (std::uint8_t)40) & (std::uint8_t)0xff);
    _routineOutput->add((std::uint8_t)(integer >> (std::uint8_t)48) & (std::uint8_t)0xff);
    _routineOutput->add((std::uint8_t)(integer >> (std::uint8_t)56) & (std::uint8_t)0xff);
}


void
X86_64Writer::writeMoveInt64ToRegisterOpcode(unsigned int registerIndex, Int64* value)
{
    writeMoveImmediateOpcode(registerIndex);
    addInt64(value);
}


void
X86_64Writer::writeMoveInt32ToRegisterOpcode(unsigned int registerIndex, Int32* value)
{
    writeMoveImmediateOpcode(registerIndex);
    addInt32(value);
}


}
