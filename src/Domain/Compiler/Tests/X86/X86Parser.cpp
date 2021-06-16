#include "X86Parser.h"
#include "Domain/Compiler/Instruction/Assembly/x86/OpCode/GeneralOpCodes.h"
#include <cmath>

namespace elet::domain::compiler::test
{


X86Parser::X86Parser()
{

}


List<Instruction*>
X86Parser::parse(List<std::uint8_t>& output)
{
    List<Instruction*> instructions;
    _output = &output;
    while (true)
    {
        auto instruction = new Instruction();
        uint8_t opcode = getByte();
        if (opcode == 0)
        {
            break;
        }
        if ((opcode & RexMask) == OpCodePrefix::RexMagic)
        {
            if (opcode & RexW)
            {
                instruction->size = SizeKind::Quad;
            }
            instruction->bytes.add(opcode);
            opcode = getByte();
        }
        bool hasOperandSizePrefix = false;
        if (opcode == OpCodePrefix::OperandSizePrefix)
        {
            instruction->bytes.add(opcode);
            instruction->size = SizeKind::Word;
            opcode = getByte();
            hasOperandSizePrefix = true;
        }
        if (opcode == OpCodePrefix::TwoByteEscape)
        {
            instruction->bytes.add(opcode);
            opcode = getByte();
            if (hasOperandSizePrefix)
            {
                parseThreeByteOpCode(opcode, instruction, instructions);
            }
            else
            {

            }
            return instructions;
        }
        parseOneByteOpCode(instructions, instruction, opcode);
    }
    return instructions;
}


void
X86Parser::parseOneByteOpCode(List<Instruction*>& instructions, Instruction* instruction, uint8_t opcode)
{
    instruction->bytes.add(opcode);
    switch (opcode)
    {
        case Ret:
            instruction->kind = InstructionKind::Ret;
            break;
        case Pop_rBP:
            instruction->kind = InstructionKind::Pop;
            instruction->operand1 = Register::rBP;
            instruction->size = SizeKind::Quad;
            break;
        case Xor_Ev_Gv:
        {
            uint8_t modrmByte = getByte();
            instruction->kind = InstructionKind::Xor;
            instruction->bytes.add(modrmByte);
            instruction->operand1 = createEvLhs(modrmByte, instruction);
            instruction->operand2 = createGv(modrmByte, instruction->size == SizeKind::Quad);
            break;
        }
        case CallNear:
        {
            instruction->kind = InstructionKind::Call;
            auto doubleWord = getDoubleWord();
            instruction->operand1 = Jv(doubleWord);
            instruction->size = SizeKind::Quad;
            for (int i = 0; i < 4; ++i)
            {
                instruction->bytes.add(doubleWord[i]);
            }
            break;
        }
        case Lea_Gv_M:
        {
            uint8_t modrmByte = getByte();
            instruction->bytes.add(modrmByte);
            instruction->kind = InstructionKind::Lea;
            instruction->operand1 = createGv(modrmByte, instruction);
            if (instruction->size == SizeKind::Quad)
            {
                instruction->operand2 = createMemoryAddress32(instruction);
            }
            else
            {
                instruction->operand2 = createMemoryAddress16();
            }
            break;
        }
        case Push_rBP:
            instruction->kind = InstructionKind::Push;
            instruction->operand1 = Register::rBP;
            instruction->size = SizeKind::Quad;
            break;
        case Mov_Ev_Gv:
        {
            uint8_t modrmByte = getByte();
            instruction->bytes.add(modrmByte);
            instruction->kind = InstructionKind::Mov;
            instruction->operand1 = createEvLhs(modrmByte, instruction);
            instruction->operand2 = createGv(modrmByte, instruction->size == SizeKind::Quad);
            break;
        }
        case Mov_Gv_Ev:
        {
            uint8_t modrmByte = getByte();
            instruction->bytes.add(modrmByte);
            instruction->kind = InstructionKind::Mov;
            instruction->operand1 = createGv(modrmByte, instruction->size == SizeKind::Quad);
            instruction->operand2 = createEvRhs(modrmByte, nullptr);
            break;
        }
        default:;
//                throw std::runtime_error("Could not find decode opcode.");
    }
    instructions.add(instruction);
}


void
X86Parser::parseThreeByteOpCode(uint8_t opcode, Instruction* instruction, List<Instruction*>& instructions)
{
    instruction->bytes.add(opcode);
    switch (opcode)
    {
        case ThreeByteOpCode::Nop_0_Ev:
            uint8_t modrmByte = getByte();
            instruction->bytes.add(modrmByte);
            instruction->kind = InstructionKind::Nop;
            instruction->operand1 = Register::rAX;
            instruction->operand2 = createEvRhs(modrmByte, instruction);
            break;
    }
    instructions.add(instruction);
}


Ev*
X86Parser::createEvLhs(std::uint8_t modrmByte, Instruction* instruction)
{
    auto ev = new Ev();
    std::uint8_t mod = modrmByte & MOD_BITS;
    std::uint8_t rm = modrmByte & RM_BITS;
    switch (mod)
    {
        case Mod::Mod1:
            ev->emplace<Register>(mapDoubleWordRegisterIndex(rm));
            if (rm == Rm5)
            {
                auto byte = getByte();
                ev->emplace<ByteDisplacement>(Register::rBP, byte);
                instruction->bytes.add(byte);
            }
            return ev;
        case Mod::Mod2:
            if (rm == Rm::Rm4)
            {
                uint8_t sib = getByte();
                instruction->bytes.add(sib);
                std::array<uint8_t, 4> dw = getDoubleWord();
                uint8_t base = sib & Sib::BaseMask;
                uint8_t index = sib & Sib::IndexMask >> 3;
                uint8_t scale = std::pow(2, sib & Sib::ScaleMask >> 5);
                auto sibDisplacement = new SibDisplacement(static_cast<Register>(base), static_cast<Register>(index), scale);
                ev->emplace<LongDisplacement>(sibDisplacement, dw);
                for (int i = 0; i < 4; ++i)
                {
                    instruction->bytes.add(dw[i]);
                }
            }
            else
            {
                throw std::runtime_error("Unimplemented Ev register");
            }
            return ev;
        case Mod::Mod3:
            if (instruction->size == SizeKind::Quad)
            {
                ev->emplace<Register>(mapQuadWordRegisterIndex(rm));
            }
            else
            {
                ev->emplace<Register>(mapDoubleWordRegisterIndex(rm));
            }
            return ev;
        default:
            throw std::runtime_error("Unknown mod byte / address form.");
    }
}

Ev*
X86Parser::createEvRhs(std::uint8_t modrmByte, Instruction* instruction)
{
    auto ev = new Ev();
    uint8_t reg = (modrmByte & REG_BITS) >> 3;
    std::uint8_t mod = modrmByte & MOD_BITS;
    std::uint8_t rm = modrmByte & RM_BITS;
    switch (mod)
    {
        case Mod::Mod2:
            if (rm == Rm::Rm4)
            {
                uint8_t sib = getByte();
                instruction->bytes.add(sib);
                std::array<uint8_t, 4> dw = getDoubleWord();
                for (int i = 0; i < 4; ++i)
                {
                    instruction->bytes.add(dw[i]);
                }
            }
            break;
        default:
            throw std::runtime_error("Not implemented");
    }
    ev->emplace<Register>(mapDoubleWordRegisterIndex(reg));
    return ev;
}


Gv*
X86Parser::createGv(std::uint8_t opcode, bool isQuadWord)
{
    std::uint8_t reg = (opcode & REG_BITS) >> 3;
    return isQuadWord ? new Gv(mapQuadWordRegisterIndex(reg)) : new Gv(mapDoubleWordRegisterIndex(reg));
}


Register
X86Parser::mapDoubleWordRegisterIndex(std::uint8_t reg)
{
    switch (reg)
    {
        case 0:
            return Register::rAX;
        case 1:
            return Register::rCX;
        case 2:
            return Register::rDX;
        case 3:
            return Register::rBX;
        case 4:
            return Register::rSP;
        case 5:
            return Register::rBP;
        case 6:
            return Register::rSI;
        case 7:
            return Register::rDI;
        default:;
    }
}


Register
X86Parser::mapQuadWordRegisterIndex(std::uint8_t reg)
{
    switch (reg)
    {
        case 0:
            return Register::rAX;
        case 1:
            return Register::rCX;
        case 2:
            return Register::rDX;
        case 3:
            return Register::rBX;
        case 4:
            return Register::rSP;
        case 5:
            return Register::rBP;
        case 6:
            return Register::rSI;
        case 7:
            return Register::rDI;
        default:;
    }
}


std::array<std::uint8_t, 4>
X86Parser::getDoubleWord()
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
X86Parser::getByte()
{
    if (_cursor == _output->size())
    {
        return 0;
    }
    return (*_output)[_cursor++];
}


MemoryAddress32*
X86Parser::createMemoryAddress32(Instruction* instruction)
{
    std::array<uint8_t, 4> opcode = getDoubleWord();
    for (int i = 0; i < 4; ++i)
    {
        instruction->bytes.add(opcode[i]);
    }
    return new MemoryAddress32(opcode);
}


MemoryAddress32*
X86Parser::createMemoryAddress16()
{
    throw std::invalid_argument("Not implemented");
}

}