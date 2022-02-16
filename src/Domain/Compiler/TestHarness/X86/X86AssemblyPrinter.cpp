#include <fstream>
#include "X86AssemblyPrinter.h"
#include "X86AssemblyParser.h"
#include "X86Types.h"


namespace elet::domain::compiler::test::x86
{
    X86AssemblyPrinter::X86AssemblyPrinter()
    {

    }


    void
    X86AssemblyPrinter::writeInstructions(const List<Instruction>& instructions)
    {
        for (const auto& instruction : instructions)
        {
            writeInstruction(instruction);
        }
    }


    void
    X86AssemblyPrinter::writeInstruction(const Instruction& instruction)
    {
        writeAddress();
        _tw.tab();
        writeByteInstruction(instruction);
        _tw.tab();
        switch (instruction.kind)
        {
            case InstructionKind::Add:
                writeInstructionWithName("add", instruction);
                break;
            case InstructionKind::Sub:
                writeInstructionWithName("sub", instruction);
                break;
            case InstructionKind::Imul:
                writeInstructionWithName("imul", instruction);
                break;
            case InstructionKind::Idiv:
                writeInstructionWithName("idiv", instruction);
                break;
            case InstructionKind::Div:
                writeInstructionWithName("div", instruction);
                break;
            case InstructionKind::Cdq:
                if (instruction.size == SizeKind::Quad)
                {
                    _tw.write("cdo");
                }
                else
                {
                    _tw.write("cdq");
                }
                break;
            case InstructionKind::Cwde:
                if (instruction.size == SizeKind::Quad)
                {
                    _tw.write("cdqe");
                }
                else
                {
                    _tw.write("cwde");
                }
                break;
            case InstructionKind::Nop:
                writeInstructionWithName("nop", instruction);
                break;
            case InstructionKind::Xor:
                writeInstructionWithName("xor", instruction);
                break;
            case InstructionKind::Lea:
                writeInstructionWithName("lea", instruction);
                break;
            case InstructionKind::Ret:
                _tw.write("ret");
                break;
            case InstructionKind::Call:
                writeInstructionWithName("call", instruction);
                break;
            case InstructionKind::Push:
                writeInstructionWithName("push", instruction);
                break;
            case InstructionKind::Pop:
                writeInstructionWithName("pop", instruction);
                break;
            case InstructionKind::Mov:
                writeInstructionWithName("mov", instruction);
                break;
            case InstructionKind::Movzx:
                writeMovzx(instruction);
                break;
            case InstructionKind::Movsx:
                writeMovsx(instruction);
                break;
            case InstructionKind::Jmp:
                writeInstructionWithName("jmp", instruction);
                break;
        }
        _tw.newline();
    }



    void
    X86AssemblyPrinter::writeInstructionWithName(const char* name, const Instruction& instruction)
    {
        _tw.write(name);
        writeSizeSuffix(instruction);
        if (!std::holds_alternative<std::monostate>(instruction.operand2))
        {
            writeOperand(&instruction.operand2, instruction);
            _tw.write(", ");
        }
        if (!std::holds_alternative<std::monostate>(instruction.operand1))
        {
            writeOperand(&instruction.operand1, instruction);
        }
    }


    void
    X86AssemblyPrinter::writeOperand(const Operand* operand, const Instruction& instruction)
    {
        writeOperand(operand, instruction, SizeKind::None);
    }


    void
    X86AssemblyPrinter::writeOperand(const Operand* operand, const Instruction& instruction, SizeKind overrideSize)
    {
        SizeKind size = overrideSize != SizeKind::None ? overrideSize : instruction.size;
        if (auto ev = std::get_if<Ev*>(operand))
        {
            if (auto reg = std::get_if<Register>(*ev))
            {
                writeGeneralPurposeRegister(*reg, size);
            }
            else if (auto disp = std::get_if<ByteDisplacement>(*ev))
            {
                writeByteDisplacement(disp, instruction);
            }
            else if (auto disp = std::get_if<LongDisplacement>(*ev))
            {
                writeLongDisplacement(disp, instruction);
            }
            else if (auto disp = std::get_if<RegisterDisplacement>(*ev))
            {
                writeRegisterDisplacement(disp, instruction);
            }
            else if (auto memoryAddress = std::get_if<MemoryAddress32>(*ev))
            {
                writeMemoryAddress(memoryAddress->value);
            }
        }
        else if (auto iz = std::get_if<Iz>(operand))
        {
            _tw.writeSignedImmediateValue(iz->offset);
        }
        else if (auto jv = std::get_if<Jv>(operand))
        {
            writeMemoryAddress((*jv).offset);
        }
        else if (auto gv = std::get_if<Gv*>(operand))
        {
            auto s = std::get<Register>(**gv);
            writeGeneralPurposeRegister(s, size);
        }
        else if (auto ib = std::get_if<Ib>(operand))
        {
            _tw.writeSignedImmediateValue(ib->offset);
        }
        else if (auto mem = std::get_if<MemoryAddress32>(operand))
        {
            writeMemoryAddress(mem->value);
        }
        else if (auto reg = std::get_if<Register>(operand))
        {
            writeGeneralPurposeRegister(*reg, size);
        }
        else
        {
            throw std::runtime_error("Could not resolve destination.");
        }
    }


    void
    X86AssemblyPrinter::writeMemoryAddress(const std::array<uint8_t, 4> mem)
    {
        size_t offset = 0;
        for (int i = 0; i < 4; ++i)
        {
            offset += mem[i] << 8 * i;
        }
        size_t vmAddress = offset + textSectionStartOffset;
        if (vmAddress >= cstringSectionOffset && vmAddress <= cstringSectionOffset + cstringSectionSize)
        {
            _tw.write("\"");
            _tw.writeAddress64(vmAddress + vmOffset);
            _tw.write(":\"");
            _tw.writeCString(reinterpret_cast<const char*>(&(*list)[vmAddress]));
            _tw.write("\"");
            _tw.write("\"");
            return;
        }
        auto result = symbols->find(vmAddress + vmOffset);
        if (result == symbols->end())
        {
            _tw.writeAddress64(vmAddress + vmOffset);
        }
        else
        {
            _tw.write("\"");
            _tw.writeAddress64(vmAddress + vmOffset);
            _tw.space();
            _tw.write("(");
            _tw.writeCString(result->second);
            _tw.write(")");
            _tw.write("\"");
        }
    }


    void
    X86AssemblyPrinter::writeGeneralPurposeRegister(const Register _register, SizeKind size)
    {
        _tw.write("%");
        switch (size)
        {
            case SizeKind::Quad:
                _tw.write("r");
                break;
            case SizeKind::Long:
                _tw.write("e");
                break;
        }
        if (size >= SizeKind::Word)
        {
            switch (_register)
            {
                case Register::rAX:
                    _tw.write("ax");
                    break;
                case Register::rCX:
                    _tw.write("cx");
                    break;
                case Register::rDX:
                    _tw.write("dx");
                    break;
                case Register::rBX:
                    _tw.write("bx");
                    break;
                case Register::rSP:
                    _tw.write("sp");
                    break;
                case Register::rBP:
                    _tw.write("bp");
                    break;
                case Register::rSI:
                    _tw.write("si");
                    break;
                case Register::rDI:
                    _tw.write("di");
                    break;
                case Register::r11:
                    _tw.write("11");
                    break;
                default:
                    throw std::runtime_error("Unknown register.");
            }
        }
        else
        {
            switch (_register)
            {
                case Register::rAX:
                    _tw.write("al");
                    break;
                case Register::rCX:
                    _tw.write("cl");
                    break;
                case Register::rDX:
                    _tw.write("dl");
                    break;
                case Register::rBX:
                    _tw.write("bl");
                    break;
                case Register::rSP:
                    _tw.write("ah");
                    break;
                case Register::rBP:
                    _tw.write("ch");
                    break;
                case Register::rSI:
                    _tw.write("dh");
                    break;
                case Register::rDI:
                    _tw.write("bh");
                    break;
                default:
                    throw std::runtime_error("Unknown register.");
            }
        }
    }


    void
    X86AssemblyPrinter::writeRegisterDisplacement(RegisterDisplacement* displacement, const Instruction& instruction)
    {
        _tw.write("(");
        writeGeneralPurposeRegister(displacement->base, instruction.size);
        _tw.write(")");
    }


    void
    X86AssemblyPrinter::writeLongDisplacement(LongDisplacement* displacement, const Instruction& instruction)
    {
        if (auto sibDisplacement = std::get_if<SibDisplacement*>(&displacement->base))
        {
            auto disp = displacement->displacement;
            auto allZero = disp[0] == 0 && disp[1] == 0 && disp[2] == 0 && disp[3] == 0;
            if (!allZero)
            {
                _tw.writeSignedHexValue(displacement->displacement);
            }
            auto sib = (*sibDisplacement);
            _tw.write("(");
            writeGeneralPurposeRegister(sib->base, instruction.size);
            _tw.write(",");
            writeGeneralPurposeRegister(sib->index, instruction.size);
            if (sib->scale != 1)
            {
                _tw.write(",");
                switch (sib->scale)
                {
                    case 2:
                        _tw.write("2");
                        break;
                    case 4:
                        _tw.write("4");
                        break;
                    case 8:
                        _tw.write("8");
                        break;
                    default:
                        throw std::runtime_error("scale must be 1,2,4,8");
                }
            }
            _tw.write(")");
        }
        else if (auto reg = std::get_if<Register>(&displacement->base))
        {
            _tw.writeSignedHexValue(displacement->displacement);
            _tw.write("(");
            writeGeneralPurposeRegister(*reg, instruction.size);
            _tw.write(")");
        }
    }


    void
    X86AssemblyPrinter::writeByteDisplacement(ByteDisplacement* displacement, const Instruction& instruction)
    {
        auto disp = displacement->displacement;
        if (disp)
        {
            _tw.writeSignedHexValue(displacement->displacement);
        }
        _tw.write("(");
        if (auto sibDisplacement = std::get_if<SibDisplacement*>(&displacement->base))
        {
            auto sib = *sibDisplacement;
            writeGeneralPurposeRegister(sib->base, instruction.size);
            _tw.write(",");
            writeGeneralPurposeRegister(sib->index, instruction.size);
            if (sib->scale != 1)
            {
                _tw.write(",");
                switch (sib->scale)
                {
                    case 2:
                        _tw.write("2");
                        break;
                    case 4:
                        _tw.write("4");
                        break;
                    case 8:
                        _tw.write("8");
                        break;
                    default:
                        throw std::runtime_error("scale must be 1,2,4,8");
                }
            }
        }
        else if (auto reg = std::get_if<Register>(&displacement->base))
        {
            writeGeneralPurposeRegister(*reg, instruction.size);

        }
        _tw.write(")");
    }


    void
    X86AssemblyPrinter::writeSizeSuffix(const Instruction& instruction)
    {
        switch (instruction.size)
        {
            case SizeKind::Quad:
                _tw.write("q ");
                break;
            case SizeKind::Long:
                _tw.write("l ");
                break;
            case SizeKind::Word:
                _tw.write("w ");
                break;
            case SizeKind::Byte:
                _tw.write("b ");
                break;
            default:
                throw std::runtime_error("Unknown instruction length.");
        }
    }


    void
    X86AssemblyPrinter::writeByteInstruction(const Instruction& instruction)
    {
        for (const auto byte : instruction.bytes)
        {
            _tw.writeByteHex(byte);
            ++textSectionStartOffset;
        }
    }

    void
    X86AssemblyPrinter::writeMovzx(const Instruction& instruction)
    {
        _tw.write("movzx");
        writeTargetOperandSizeOverridableOperands(instruction);
    }


    void
    X86AssemblyPrinter::writeMovsx(const Instruction& instruction)
    {
        _tw.write("movsx");
        writeTargetOperandSizeOverridableOperands(instruction);
    }


    void
    X86AssemblyPrinter::writeTargetOperandSizeOverridableOperands(const Instruction& instruction)
    {
        writeSizeSuffix(instruction);
        if (!std::holds_alternative<std::monostate>(instruction.operand2))
        {
            writeOperand(&instruction.operand2, instruction, instruction.targetOperandSize);
            _tw.write(", ");
        }
        if (!std::holds_alternative<std::monostate>(instruction.operand1))
        {
            writeOperand(&instruction.operand1, instruction);
        }
    }
}

