#include <fstream>
#include "X86BaselinePrinter.h"
#include "X86Parser.h"
#include "X86Types.h"


namespace elet::domain::compiler::test
{


X86BaselinePrinter::X86BaselinePrinter()
{
    _parser = new X86Parser();
    _tw = new instruction::TextWriter();
}


Utf8String
X86BaselinePrinter::print()
{
    _tw->addColumn(18);
    _tw->addColumn(40);
    writeColumnHeader();
    for (const auto& instruction : instructions)
    {
        writeInstruction(instruction);
    }
    return _tw->toString();
}


void
X86BaselinePrinter::writeColumnHeader()
{
    _tw->write("Address");
    _tw->tab();
    _tw->write("Bytes");
    _tw->tab();
    _tw->write("Instruction");
    _tw->newline();
    _tw->write("============================================================");
    _tw->newline();
}


void
X86BaselinePrinter::writeInstruction(Instruction* instruction)
{
    writeAddress();
    _tw->tab();
    writeByteInstruction(instruction);
    _tw->tab();
    switch (instruction->kind)
    {
        case InstructionKind::Add:
            writeInstructionWithName("add", instruction);
            break;
        case InstructionKind::Sub:
            writeInstructionWithName("sub", instruction);
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
            _tw->write("ret");
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
    }
    _tw->newline();
}



void
X86BaselinePrinter::writeInstructionWithName(const char* name, const Instruction* instruction)
{
    _tw->write(name);
    writeSizeSuffix(instruction);
    if (!std::holds_alternative<std::monostate>(instruction->operand2))
    {
        writeOperand(&instruction->operand2, instruction);
        _tw->write(", ");
    }
    if (!std::holds_alternative<std::monostate>(instruction->operand1))
    {
        writeOperand(&instruction->operand1, instruction);
    }
}


void
X86BaselinePrinter::writeByteInstruction(Instruction* instruction)
{
    for (const auto byte : instruction->bytes)
    {
        _tw->writeByteHex(byte);
        ++address;
    }
}


void
X86BaselinePrinter::writeOperand(const Operand* operand, const Instruction* instruction)
{
    if (auto ev = std::get_if<Ev*>(operand))
    {
        if (auto reg = std::get_if<Register>(*ev))
        {
            writeGeneralPurposeRegister(*reg, instruction);
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
    }
    else if (auto jv = std::get_if<Jv>(operand))
    {
        writeMemoryAddress((*jv).offset);
    }
    else if (auto gv = std::get_if<Gv*>(operand))
    {
        auto s = std::get<Register>(**gv);
        writeGeneralPurposeRegister(s, instruction);
    }
    else if (auto ib = std::get_if<Ib*>(operand))
    {
        _tw->writeImmediateValue((*ib)->offset);
    }
    else if (auto mem = std::get_if<MemoryAddress32*>(operand))
    {
        writeMemoryAddress((*mem)->value);
    }
    else if (auto reg = std::get_if<Register>(operand))
    {
        writeGeneralPurposeRegister(*reg, instruction);
    }
    else
    {
        throw std::runtime_error("Could not resolve operand.");
    }
}

void
X86BaselinePrinter::writeMemoryAddress(const std::array<uint8_t, 4> mem)
{
    size_t offset = 0;
    for (int i = 0; i < 4; ++i)
    {
        offset += mem[i] << 8 * i;
    }
    size_t vmAddress = offset + address;
    if (vmAddress >= cstringSectionOffset && vmAddress <= cstringSectionOffset + cstringSectionSize)
    {
        _tw->write("\"");
        _tw->writeAddress64(vmAddress + vmOffset);
        _tw->write(":\"");
        _tw->writeCString(reinterpret_cast<const char*>(&(*list)[vmAddress]));
        _tw->write("\"");
        _tw->write("\"");
        return;
    }
    auto result = symbols->find(vmAddress + vmOffset);
    if (result == symbols->end())
    {
        _tw->writeAddress64(vmAddress + vmOffset);
    }
    else
    {
        _tw->write("\"");
        _tw->writeAddress64(vmAddress + vmOffset);
        _tw->space();
        _tw->write("(");
        _tw->writeCString(result->second);
        _tw->write(")");
        _tw->write("\"");
    }
}


void
X86BaselinePrinter::writeGeneralPurposeRegister(const Register _register, const Instruction* instruction)
{
    _tw->write("%");
    switch (instruction->size)
    {
        case SizeKind::Quad:
            _tw->write("r");
            break;
        case SizeKind::Long:
            _tw->write("e");
            break;
    }
    switch (_register)
    {
        case Register::rAX:
            _tw->write("ax");
            break;
        case Register::rCX:
            _tw->write("cx");
            break;
        case Register::rDX:
            _tw->write("dx");
            break;
        case Register::rBX:
            _tw->write("bx");
            break;
        case Register::rSP:
            _tw->write("sp");
            break;
        case Register::rBP:
            _tw->write("bp");
            break;
        case Register::rSI:
            _tw->write("si");
            break;
        case Register::rDI:
            _tw->write("di");
            break;
        default:
            throw std::runtime_error("Unknown register.");
    }
}


void
X86BaselinePrinter::writeRegisterDisplacement(RegisterDisplacement* displacement, const Instruction* instruction)
{
    _tw->write("(");
    writeGeneralPurposeRegister(displacement->base, instruction);
    _tw->write(")");
}


void
X86BaselinePrinter::writeLongDisplacement(LongDisplacement* displacement, const Instruction* instruction)
{
    if (auto sibDisplacement = std::get_if<SibDisplacement*>(&displacement->base))
    {
        auto disp = displacement->displacement;
        auto allZero = disp[0] == 0 && disp[1] == 0 && disp[2] == 0 && disp[3] == 0;
        if (!allZero)
        {
            _tw->writeSignedHex(displacement->displacement);
        }
        auto sib = (*sibDisplacement);
        _tw->write("(");
        writeGeneralPurposeRegister(sib->base, instruction);
        _tw->write(",");
        writeGeneralPurposeRegister(sib->index, instruction);
        if (sib->scale != 1)
        {
            _tw->write(",");
            switch (sib->scale)
            {
                case 2:
                    _tw->write("2");
                    break;
                case 4:
                    _tw->write("4");
                    break;
                case 8:
                    _tw->write("8");
                    break;
                default:
                    throw std::runtime_error("scale must be 1,2,4,8");
            }
        }
        _tw->write(")");
    }
    else
    {
        throw std::runtime_error("Not implemented four byte displacement on register");
    }
}


void
X86BaselinePrinter::writeByteDisplacement(ByteDisplacement* displacement, const Instruction* instruction)
{
    auto disp = displacement->displacement;
    if (disp)
    {
        _tw->writeDisplacement(displacement->displacement);
    }
    _tw->write("(");
    if (auto sibDisplacement = std::get_if<SibDisplacement*>(&displacement->base))
    {
        auto sib = (*sibDisplacement);
        writeGeneralPurposeRegister(sib->base, instruction);
        _tw->write(",");
        writeGeneralPurposeRegister(sib->index, instruction);
        if (sib->scale != 1)
        {
            _tw->write(",");
            switch (sib->scale)
            {
                case 2:
                    _tw->write("2");
                    break;
                case 4:
                    _tw->write("4");
                    break;
                case 8:
                    _tw->write("8");
                    break;
                default:
                    throw std::runtime_error("scale must be 1,2,4,8");
            }
        }
    }
    else if (auto reg = std::get_if<Register>(&displacement->base))
    {
        writeGeneralPurposeRegister(*reg, instruction);

    }
    _tw->write(")");
}


void
X86BaselinePrinter::writeAddress()
{
    auto vmAddress = address + vmOffset;
    auto result = symbols->find(vmAddress);
    if (result != symbols->end())
    {
        _tw->newline();
        _tw->write(result->second);
        _tw->write(":");
        _tw->newline();
    }
    _tw->writeByteHex((vmAddress >> 56) & 0xff);
    _tw->writeByteHex((vmAddress >> 48) & 0xff);
    _tw->writeByteHex((vmAddress >> 40) & 0xff);
    _tw->writeByteHex((vmAddress >> 32) & 0xff);
    _tw->writeByteHex((vmAddress >> 24) & 0xff);
    _tw->writeByteHex((vmAddress >> 16) & 0xff);
    _tw->writeByteHex((vmAddress >> 8) & 0xff);
    _tw->writeByteHex(vmAddress & 0xff);
}


void
X86BaselinePrinter::writeSizeSuffix(const Instruction* instruction)
{
    switch (instruction->size)
    {
        case SizeKind::Quad:
            _tw->write("q ");
            break;
        case SizeKind::Long:
            _tw->write("l ");
            break;
        case SizeKind::Word:
            _tw->write("w ");
            break;
        case SizeKind::Byte:
            _tw->write("b ");
            break;
        default:
            throw std::runtime_error("Unknown instruction length.");
    }
}


}

