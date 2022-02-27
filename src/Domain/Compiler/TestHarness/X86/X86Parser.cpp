#include "X86Parser.h"
#include "Domain/Compiler/Instruction/Assembly/x86/OpCode/GeneralOpCodes.h"
#include <cmath>
#include <assert.h>

namespace elet::domain::compiler::test::x86
{
    X86Parser::X86Parser()
    {

    }


    void
    X86Parser::parse(List<Instruction>& instructions, List<uint8_t>& output, size_t offset, size_t size)
    {
        _output = &output;
        _offset = offset;
        _cursor = offset;
        _size = size;

        while (_cursor - _offset < _size)
        {
            Instruction& instruction = *reinterpret_cast<Instruction*>(instructions.emplace());
            uint8_t opcode = getByte(instruction);
            if (opcode == 0)
            {
                break;
            }
            if ((opcode & RexMask) == OpCodePrefix::RexMagic)
            {
                if (opcode & RexW)
                {
                    instruction.size = SizeKind::Quad;
                }
                if (opcode & RexB)
                {
                    instruction.rexb = true;
                }
                opcode = getByte(instruction);
            }
            bool hasOperandSizePrefix = false;
            if (opcode == OpCodePrefix::OperandSizePrefix)
            {
                instruction.size = SizeKind::Word;
                opcode = getByte(instruction);
                hasOperandSizePrefix = true;
            }
            if (opcode == OpCodePrefix::TwoByteEscape)
            {
                opcode = getByte(instruction);
                if (opcode == OpCodePrefix::ThreeByteEscape1 || opcode == OpCodePrefix::ThreeByteEscape2)
                {
                    opcode = getByte(instruction);
                    parseThreeByteOpCode(opcode, instruction, instructions);
                }
                else
                {
                    parseTwoByteOpCode(opcode, instruction);
                }
                continue;
            }
            parseOneByteOpCode(instructions, instruction, opcode);
        }
    }


    void
    X86Parser::parseOneByteOpCode(List<Instruction>& instructions, Instruction& instruction, uint8_t opcode)
    {
        switch (opcode)
        {
            case OneByteOpCode::ImmediateGroup1_Ev_Ib:
            {
                uint8_t modrmByte = getByte(instruction);
                uint8_t offset = getByte(instruction); // Note only one byte offset from "Ib".
                uint8_t op = (modrmByte & ModRmMask::Reg);
                switch (op)
                {
                    case static_cast<uint8_t>(RegBits::ImmediateGroup1_Add):
                        instruction.kind = InstructionKind::Add;
                        break;
                    case static_cast<uint8_t>(RegBits::ImmediateGroup1_Sub):
                        instruction.kind = InstructionKind::Sub;
                        break;
                    default:
                        throw std::runtime_error("Unknown destination code from modrmByte.");
                }
                instruction.operand1 = createE(modrmByte, instruction, true);
                instruction.operand2 = Ib(offset);
                break;
            }
            case OneByteOpCode::Ret:
                instruction.kind = InstructionKind::Ret;
                break;
            case OneByteOpCode::Pop_rBP:
                instruction.kind = InstructionKind::Pop;
                instruction.operand1 = Register::rBP;
                instruction.size = SizeKind::Quad;
                break;
            case OneByteOpCode::Xor_Ev_Gv:
            {
                uint8_t modrmByte = getByte(instruction);
                instruction.kind = InstructionKind::Xor;
                instruction.operand1 = createE(modrmByte, instruction, true);
                instruction.operand2 = createGv(modrmByte, instruction.size == SizeKind::Quad);
                break;
            }
            case OneByteOpCode::JmpNear:
                instruction.kind = InstructionKind::Jmp;
                instruction.operand1.emplace<MemoryAddress32>(getDoubleWord(instruction));
                break;
            case OneByteOpCode::CallNear:
            {
                instruction.kind = InstructionKind::Call;
                auto doubleWord = getDoubleWord(instruction);
                instruction.operand1 = Jv(doubleWord);
                instruction.size = SizeKind::Quad;
                break;
            }
            case OneByteOpCode::Lea_Gv_M:
            {
                uint8_t modrmByte = getByte(instruction);
                instruction.kind = InstructionKind::Lea;
                instruction.operand1 = createGv(modrmByte, true);
                if (instruction.size == SizeKind::Quad)
                {
                    instruction.operand2 = createMemoryAddress32(instruction);
                }
                else
                {
    //                instruction.operand2 = createMemoryAddress16();
                }
                break;
            }
            case OneByteOpCode::Push_Iz:
                instruction.kind = InstructionKind::Push;
                instruction.operand1.emplace<Iz>(getDoubleWord(instruction));
                break;
            case OneByteOpCode::Push_rBX:
                instruction.kind = InstructionKind::Push;
                if (instruction.rexb)
                {
                    instruction.operand1 = Register::r11;
                }
                else
                {
                    instruction.operand1 = Register::rBX;
                }
                instruction.size = SizeKind::Quad;
                break;
            case OneByteOpCode::Push_rBP:
                instruction.kind = InstructionKind::Push;
                instruction.operand1 = Register::rBP;
                instruction.size = SizeKind::Quad;
                break;
            case OneByteOpCode::Mov_Eb_Ib:
            {
                uint8_t modrmByte = getByte(instruction);
                instruction.kind = InstructionKind::Mov;
                instruction.operand1 = createE(modrmByte, instruction, true);
                instruction.operand2.emplace<Ib>(getByte(instruction));
                break;
            }
            case OneByteOpCode::Mov_Ev_Iz:
            {
                uint8_t modrmByte = getByte(instruction);
                instruction.kind = InstructionKind::Mov;
                instruction.operand1 = createE(modrmByte, instruction, true);
                if (instruction.size >= SizeKind::Long)
                {
                    instruction.operand2.emplace<Iz>(getDoubleWord(instruction));
                }
                else if (instruction.size == SizeKind::Word)
                {
                    instruction.operand2.emplace<Iz>(getWord(instruction));
                }
                break;
            }
            case OneByteOpCode::Mov_Ev_Gv:
            {
                uint8_t modrmByte = getByte(instruction);
                instruction.kind = InstructionKind::Mov;
                instruction.operand1 = createE(modrmByte, instruction, true);
                instruction.operand2 = createGv(modrmByte, instruction.size == SizeKind::Quad);
                break;
            }
            case OneByteOpCode::Mov_Gv_Ev:
            {
                uint8_t modrmByte = getByte(instruction);
                instruction.kind = InstructionKind::Mov;
                instruction.operand1 = createGv(modrmByte, instruction.size == SizeKind::Quad);
                instruction.operand2 = createE(modrmByte, instruction, true);
                break;
            }
            case OneByteOpCode::ExtGroup3:
            {
                uint8_t modrmByte = getByte(instruction);
                uint8_t regBits = ModRmMask::Reg & modrmByte;
                switch (regBits)
                {
                    case OneByteOpCode::ExtGroup3_DivRegBits:
                        instruction.kind = InstructionKind::Div;
                        instruction.operand1 = createE(modrmByte, instruction, true);
                        break;
                    case OneByteOpCode::ExtGroup3_IdivRegBits:
                        instruction.kind = InstructionKind::Idiv;
                        instruction.operand1 = createE(modrmByte, instruction, true);
                        break;
                    default:
                        assert("Unknown regbits.");
                }
                break;
            }
            case OneByteOpCode::ExtGroup5:
            {
                uint8_t modrmByte = getByte(instruction);
                instruction.kind = InstructionKind::Jmp;
                instruction.operand1 = createE(modrmByte, instruction, true);

                // Note, it is forced(f64) 64-bits, See opcode map for reference.
                instruction.size = SizeKind::Quad;
                break;
            }
            case OneByteOpCode::Nop:
                instruction.kind = InstructionKind::Nop;
                break;
            case OneByteOpCode::Cdq:
                instruction.kind = InstructionKind::Cdq;
                break;
            case OneByteOpCode::Cwde:
                instruction.kind = InstructionKind::Cwde;
                break;
            case OneByteOpCode::Add_Ev_Gv:
            {
                uint8_t modrmByte = getByte(instruction);
                instruction.kind = InstructionKind::Add;
                instruction.operand1 = createE(modrmByte, instruction, true);
                instruction.operand2 = createGv(modrmByte, instruction.size == SizeKind::Quad);
                break;
            }
            case OneByteOpCode::Add_Gv_Ev:
            {
                uint8_t modrmByte = getByte(instruction);
                instruction.kind = InstructionKind::Add;
                instruction.operand1 = createGv(modrmByte, instruction.size == SizeKind::Quad);
                instruction.operand2 = createE(modrmByte, instruction, true);
                break;
            }
            case OneByteOpCode::Sub_Ev_Gv:
            {
                uint8_t modrmByte = getByte(instruction);
                instruction.kind = InstructionKind::Sub;
                instruction.operand1 = createE(modrmByte, instruction, true);
                instruction.operand2 = createGv(modrmByte, instruction.size == SizeKind::Quad);
                break;
            }
            case OneByteOpCode::Sub_Gv_Ev:
            {
                uint8_t modrmByte = getByte(instruction);
                instruction.kind = InstructionKind::Sub;
                instruction.operand1 = createGv(modrmByte, instruction.size == SizeKind::Quad);
                instruction.operand2 = createE(modrmByte, instruction, true);
                break;
            }
            default:
                throw std::runtime_error("Could not find decode opcode.");
        }
    }



    void
    X86Parser::parseTwoByteOpCode(uint8_t opcode, Instruction& instruction)
    {
        switch (opcode)
        {
            case OneByteOpCode::Movzx_Gv_Eb:
            {
                uint8_t modrmByte = getByte(instruction);
                instruction.kind = InstructionKind::Movzx;
                instruction.operand1 = createGv(modrmByte, instruction.size == SizeKind::Quad);
                instruction.operand2 = createE(modrmByte, instruction, true);
                if (std::holds_alternative<Ev*>(instruction.operand2))
                {
                    Ev* ev = std::get<Ev*>(instruction.operand2);
                    if (std::holds_alternative<Register>(*ev))
                    {
                        instruction.targetOperandSize = SizeKind::Byte;
                    }
                }
                break;
            }
            case OneByteOpCode::Movzx_Gv_Ew:
            {
                uint8_t modrmByte = getByte(instruction);
                instruction.kind = InstructionKind::Movzx;
                instruction.operand1 = createGv(modrmByte, instruction.size == SizeKind::Quad);
                instruction.operand2 = createE(modrmByte, instruction, true);
                if (std::holds_alternative<Ev*>(instruction.operand2))
                {
                    Ev* ev = std::get<Ev*>(instruction.operand2);
                    if (std::holds_alternative<Register>(*ev))
                    {
                        instruction.targetOperandSize = SizeKind::Word;
                    }
                }
                break;
            }
            case OneByteOpCode::Movsx_Gv_Eb:
            {
                uint8_t modrmByte = getByte(instruction);
                instruction.kind = InstructionKind::Movsx;
                instruction.operand1 = createGv(modrmByte, instruction.size == SizeKind::Quad);
                instruction.operand2 = createE(modrmByte, instruction, true);
                if (std::holds_alternative<Ev*>(instruction.operand2))
                {
                    Ev* ev = std::get<Ev*>(instruction.operand2);
                    if (std::holds_alternative<Register>(*ev))
                    {
                        instruction.targetOperandSize = SizeKind::Byte;
                    }
                }
                break;
            }
            case OneByteOpCode::Movsx_Gv_Ew:
            {
                uint8_t modrmByte = getByte(instruction);
                instruction.kind = InstructionKind::Movsx;
                instruction.operand1 = createGv(modrmByte, instruction.size == SizeKind::Quad);
                instruction.operand2 = createE(modrmByte, instruction, true);
                if (std::holds_alternative<Ev*>(instruction.operand2))
                {
                    Ev* ev = std::get<Ev*>(instruction.operand2);
                    if (std::holds_alternative<Register>(*ev))
                    {
                        instruction.targetOperandSize = SizeKind::Word;
                    }
                }
                break;
            }
            case OneByteOpCode::Imul_Gv_Ev:
            {
                uint8_t modrmByte = getByte(instruction);
                instruction.kind = InstructionKind::Imul;
                instruction.operand1 = createGv(modrmByte, instruction.size == SizeKind::Quad);
                instruction.operand2 = createE(modrmByte, instruction, true);
                break;
            }
            case ThreeByteOpCode::Nop_0_Ev:
            {
                uint8_t modrmByte = getByte(instruction);
                instruction.kind = InstructionKind::Nop;
                instruction.operand1 = createE(modrmByte, instruction, true);
                break;
            }
        }
    }


    void
    X86Parser::parseThreeByteOpCode(uint8_t opcode, Instruction& instruction, List<Instruction>& instructions)
    {
        switch (opcode)
        {
            case ThreeByteOpCode::Nop_0_Ev:
                uint8_t modrmByte = getByte(instruction);
                instruction.kind = InstructionKind::Nop;
                instruction.operand1 = createE(modrmByte, instruction, true);
                break;
        }
    }


    Ev*
    X86Parser::createE(uint8_t modrmByte, Instruction& instruction, bool useOnlyRmField)
    {
        auto ev = new Ev();
        if (useOnlyRmField)
        {
            uint8_t mod = modrmByte & ModRmMask::Mod;
            uint8_t rm = modrmByte & ModRmMask::Rm;
            switch (mod)
            {
                case ModBits::Mod0:
                    if (rm == RmBits::Rm5)
                    {
                        std::array<uint8_t, 4> dw = getDoubleWord(instruction);
                        ev->emplace<MemoryAddress32>(dw);
                    }
                    else
                    {
                        ev->emplace<RegisterDisplacement>(mapDoubleWordRegisterIndex(rm));
                    }
                    return ev;
                case ModBits::Mod1:
                    if (rm == RmBits::Rm4)
                    {
                        uint8_t sib = getByte(instruction);
                        uint8_t offset = getByte(instruction);
                        uint8_t base = sib & SibBits::BaseMask;
                        uint8_t index = sib & SibBits::IndexMask >> 3;
                        uint8_t scale = std::pow(2, sib & SibBits::ScaleMask >> 5);
                        auto sibDisplacement = new SibDisplacement(static_cast<Register>(base), static_cast<Register>(index), scale);
                        ev->emplace<ByteDisplacement>(sibDisplacement, offset);
                    }
                    else
                    {
                        auto byte = getByte(instruction);
                        ev->emplace<ByteDisplacement>(mapDoubleWordRegisterIndex(rm), byte);
                    }
                    return ev;
                case ModBits::Mod2:
                    if (rm == RmBits::Rm4)
                    {
                        uint8_t sib = getByte(instruction);
                        std::array<uint8_t, 4> dw = getDoubleWord(instruction);
                        uint8_t base = sib & SibBits::BaseMask;
                        uint8_t index = sib & SibBits::IndexMask >> 3;
                        uint8_t scale = std::pow(2, sib & SibBits::ScaleMask >> 5);
                        auto sibDisplacement = new SibDisplacement(static_cast<Register>(base), static_cast<Register>(index), scale);
                        ev->emplace<LongDisplacement>(sibDisplacement, dw);
                    }
                    else
                    {
                        std::array<uint8_t, 4> disp = getDoubleWord(instruction);
                        ev->emplace<LongDisplacement>(mapQuadWordRegisterIndex(rm), disp);
                    }
                    return ev;
                case ModBits::Mod3:
                    if (instruction.size == SizeKind::Quad)
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
    }


    Gv*
    X86Parser::createGv(std::uint8_t opcode, bool isQuadWord)
    {
        std::uint8_t reg = (opcode & ModRmMask::Reg) >> 3;
        return isQuadWord ? new Gv(mapQuadWordRegisterIndex(reg)) : new Gv(mapDoubleWordRegisterIndex(reg));
    }


    Register
    X86Parser::mapDoubleWordRegisterIndex(uint8_t reg)
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


    std::array<uint8_t, 4>
    X86Parser::getDoubleWord(Instruction& instruction)
    {
        std::array<uint8_t, 4> result = { 0, 0, 0, 0 };
        for (unsigned int i = 0; i < 4; ++i)
        {
            uint8_t opcode = getByte(instruction);
            result[i] = opcode;
        }
        return result;
    }

    std::array<uint8_t, 2>
    X86Parser::getWord(Instruction& instruction)
    {
        std::array<uint8_t, 2> result = { 0, 0,  };
        for (unsigned int i = 0; i < 2; ++i)
        {
            uint8_t opcode = getByte(instruction);
            result[i] = opcode;
        }
        return result;
    }


    uint8_t
    X86Parser::getByte(Instruction& instruction)
    {
        uint8_t result = (*_output)[_cursor++];
        instruction.bytes.add(result);
        return result;
    }


    MemoryAddress32
    X86Parser::createMemoryAddress32(Instruction& instruction)
    {
        std::array<uint8_t, 4> opcode = getDoubleWord(instruction);
        return MemoryAddress32(opcode);
    }
}