#include "X86BaselineParser.h"
#include "../OpCode/GeneralOpCodes.h"


namespace elet::domain::compiler::instruction::x86
{


List<Instruction*>*
X86BaselineParser::parse(List<std::uint8_t>* output)
{
    auto instructions = new List<Instruction*>();
    _output = output;
    while (true)
    {
        auto instruction = new x86::Instruction();
        uint8_t opcode = getByte();
        if (opcode == 0)
        {
            break;
        }
        if ((opcode & REX_PREFIX_MASK) == REX_PREFIX_MAGIC)
        {
            if (opcode & REX_PREFIX_W)
            {
                instruction->isQuadWord = true;
            }
            instruction->bytes.add(opcode);
            opcode = getByte();
        }
        instruction->bytes.add(opcode);
        switch (opcode)
        {
            case OP_CALL_NEAR:
            {
                instruction->kind = x86::InstructionKind::Call;
                auto doubleWord = getDoubleWord();
                instruction->operand1 = x86::Jv(doubleWord);
                for (int i = 0; i < 4; ++i)
                {
                    instruction->bytes.add(doubleWord[i]);
                }
                break;
            }
            case OP_LEA_Gv_M:
            {
                std::uint8_t modrmByte = getByte();
                instruction->bytes.add(modrmByte);
                instruction->kind = x86::InstructionKind::Lea;
                instruction->operand1 = createEv(modrmByte, instruction);
                if (instruction->isQuadWord)
                {
                    instruction->operand2 = createMemoryAddress32();
                }
                else
                {
                    instruction->operand2 = createMemoryAddress16();
                }
                break;
            }
            case OP_PUSH_rBP:
                instruction->kind = x86::InstructionKind::Push;
                instruction->operand1 = x86::Register::RBP;
                break;
            case OP_MOV_Ev_Gv:
            {
                std::uint8_t modrmByte = getByte();
                instruction->bytes.add(modrmByte);
                instruction->kind = x86::InstructionKind::Mov;
                instruction->operand1 = createEv(modrmByte, instruction);
                instruction->operand2 = createGv(modrmByte, instruction->isQuadWord);
                break;
            }
            case OP_MOV_Gv_Ev:
            {
                std::uint8_t modrmByte = getByte();
                instruction->bytes.add(modrmByte);
                instruction->kind = x86::InstructionKind::Mov;
                instruction->operand1 = createGv(modrmByte, instruction->isQuadWord);
                instruction->operand2 = createEv(modrmByte, instruction);
                break;
            }
        }
        instructions->add(instruction);
    }
    return instructions;
}


x86::Ev*
X86BaselineParser::createEv(std::uint8_t modrmByte, Instruction* instruction)
{
    auto ev = new x86::Ev();
    std::uint8_t mod = modrmByte & MOD_BITS;
    std::uint8_t rm = modrmByte & RM_BITS;
    switch (mod)
    {
        case MOD_DISP8:
            ev->emplace<Register>(mapDoubleWordRegisterIndex(rm));
            if (rm == RM_EBP)
            {
                auto byte = getByte();
                ev->emplace<ByteDisplacement>(Register::EBP, byte);
                instruction->bytes.add(byte);
            }
            return ev;
        case MOD_REG:
            if (instruction->isQuadWord)
            {
                ev->emplace<Register>(mapQuadWordRegisterIndex(rm));
            }
            else
            {
                ev->emplace<Register>(mapDoubleWordRegisterIndex(rm));
            }
            return ev;
        default:
            throw std::exception();
    }
}


x86::Gv*
X86BaselineParser::createGv(std::uint8_t opcode, bool isQuadWord)
{
    std::uint8_t reg = (opcode & REG_BITS) >> 3;
    return isQuadWord ? new x86::Gv(mapQuadWordRegisterIndex(reg)) : new x86::Gv(mapDoubleWordRegisterIndex(reg));
}


Register
X86BaselineParser::mapDoubleWordRegisterIndex(std::uint8_t reg)
{
    switch (reg)
    {
        case 0:
            return Register::EAX;
        case 1:
            return Register::ECX;
        case 2:
            return Register::EDX;
        case 3:
            return Register::EBX;
        case 4:
            return Register::ESP;
        case 5:
            return Register::EBP;
        case 6:
            return Register::ESI;
        case 7:
            return Register::EDI;
        default:;
    }
}


Register
X86BaselineParser::mapQuadWordRegisterIndex(std::uint8_t reg)
{
    switch (reg)
    {
        case 0:
            return Register::RAX;
        case 1:
            return Register::RCX;
        case 2:
            return Register::RDX;
        case 3:
            return Register::RBX;
        case 4:
            return Register::RSP;
        case 5:
            return Register::RBP;
        case 6:
            return Register::RSI;
        case 7:
            return Register::RDI;
        default:;
    }
}


std::array<std::uint8_t, 4>
X86BaselineParser::getDoubleWord()
{
    std::array<std::uint8_t, 4> result = { 0, 0, 0, 0 };
    for (unsigned int i = 0; i < 4; ++i)
    {
        std::uint8_t opcode = getByte();
        result[i] = opcode;
    }
    return result;
}


std::uint8_t
X86BaselineParser::getByte()
{
    if (_cursor == _output->size())
    {
        return 0;
    }
    return (*_output)[_cursor++];
}


MemoryAddress32*
X86BaselineParser::createMemoryAddress32()
{
    std::array<std::uint8_t, 4> opcode = getDoubleWord();
    return new x86::MemoryAddress32(opcode);
}


MemoryAddress32*
X86BaselineParser::createMemoryAddress16()
{
    throw std::invalid_argument("Not implemented");
}

}