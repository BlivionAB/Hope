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
                instruction->is64BitOperandSize = true;
            }
            opcode = getByte();
        }
        switch (opcode)
        {
            case OP_CALL_NEAR:
                instruction->kind = x86::InstructionKind::Call;
                instruction->operand1 = x86::Jv(getDoubleWord());
                break;
            case OP_PUSH_rBP:
                instruction->kind = x86::InstructionKind::Push;
                instruction->operand1 = x86::Register::RBP;
                break;
            case OP_MOV_Ev_Gv:
            {
                std::uint8_t modrmByte = getByte();
                instruction->kind = x86::InstructionKind::Mov;
                instruction->operand1 = createEv(modrmByte);
                instruction->operand2 = createGv(modrmByte);
                break;
            }

        }
        instructions->add(instruction);
    }
    return instructions;
}


x86::Ev*
X86BaselineParser::createEv(std::uint8_t modrmByte)
{
    auto ev = new x86::Ev();
    std::uint8_t mod = modrmByte & MOD_BITS;
    std::uint8_t rm = modrmByte & RM_BITS;
    switch (mod)
    {
        case MOD_DISP8:
            ev->emplace<Register>(mapRegIndex(rm));
            if (rm == RM5)
            {
                ev->emplace<ByteDisplacement>(Register::ESP, getByte());
            }
            return ev;
        case MOD_REG:
            ev->emplace<Register>(mapRegIndex(rm));
            return ev;
        default:
            throw std::exception();
    }
}


x86::Gv*
X86BaselineParser::createGv(std::uint8_t opcode)
{
    std::uint8_t reg = (opcode & REG_BITS) >> 3;
    return new x86::Gv(mapRegIndex(reg));
}


Register
X86BaselineParser::mapRegIndex(std::uint8_t reg)
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



std::uint32_t
X86BaselineParser::getDoubleWord()
{
    std::uint32_t result = 0;
    unsigned int offset = 0;
    for (unsigned int i = 0; i < 4; ++i)
    {
        std::uint8_t opcode = getByte();
        result |= (opcode << offset);
        ++offset;
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

}