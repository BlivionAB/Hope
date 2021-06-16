#include <fstream>
#include "X86BaselinePrinter.h"
#include "X86Parser.h"
#include "X86Types.h"
#include "Foundation/FilePath.h"


namespace elet::domain::compiler::test
{


X86BaselinePrinter::X86BaselinePrinter()
{
    _parser = new X86Parser();
    _tw = new instruction::TextWriter();
}


void
X86BaselinePrinter::print(List<Instruction*>& instructions)
{
    _tw->addColumn(10);
    _tw->addColumn(40);
    _tw->addColumn(56);
    writeColumnHeader();
    for (const auto& instruction : instructions)
    {
        writeInstruction(instruction);
    }
    std::cout << _tw->toString() << std::endl;
}


void
X86BaselinePrinter::writeColumnHeader()
{
    _tw->write("Address");
    _tw->tab();
    _tw->write("Instruction");
    _tw->tab();
    _tw->write("Bytes");
    _tw->tab();
    _tw->write("Symbol");
    _tw->newline();
    _tw->write("============================================================");
    _tw->newline();
}


void
X86BaselinePrinter::writeInstruction(Instruction* instruction)
{
    writeAddress();
    _tw->tab();
    switch (instruction->kind)
    {
        case InstructionKind::Nop:
            _tw->write("nop");
            writeSizeSuffix(instruction);
            writeOperand(&instruction->operand2);
            _tw->write(",");
            writeOperand(&instruction->operand1);
            break;
        case InstructionKind::Pop:
            _tw->write("pop");
            writeSizeSuffix(instruction);
            writeOperand(&instruction->operand1);
            break;
        case InstructionKind::Xor:
            _tw->write("xor");
            writeSizeSuffix(instruction);
            writeOperand(&instruction->operand2);
            _tw->write(",");
            writeOperand(&instruction->operand1);
            break;
        case InstructionKind::Lea:
            _tw->write("lea");
            writeSizeSuffix(instruction);
            writeOperand(&instruction->operand2);
            _tw->write(",");
            writeOperand(&instruction->operand1);
            break;
        case InstructionKind::Ret:
            _tw->write("ret");
            break;
        case InstructionKind::Call:
            _tw->write("call");
            writeSizeSuffix(instruction);
            writeOperand(&instruction->operand1);
            break;
        case InstructionKind::Push:
            _tw->write("push");
            writeSizeSuffix(instruction);
            writeGeneralPurposeRegister(std::get<Register>(instruction->operand1));
            break;
        case InstructionKind::Mov:
            _tw->write("mov");
            writeSizeSuffix(instruction);
            writeOperand(&instruction->operand2);
            _tw->write(",");
            writeOperand(&instruction->operand1);
            break;
    }
    _tw->tab();
    writeByteInstruction(instruction);
    _tw->newline();
}


void
X86BaselinePrinter::writeByteInstruction(Instruction* instruction)
{
    for (const auto byte : instruction->bytes)
    {
        _tw->writeByteHex(byte);
        ++_address;
    }
}


void
X86BaselinePrinter::writeOperand(Operand* operand)
{
    if (auto ev = std::get_if<Ev*>(operand))
    {
        if (auto reg = std::get_if<Register>(*ev))
        {
            writeGeneralPurposeRegister(*reg);
        }
        else if (auto byteDisp = std::get_if<ByteDisplacement>(*ev))
        {
            writeByteDisplacement(byteDisp);
        }
        else if (auto byteDisp = std::get_if<LongDisplacement>(*ev))
        {
            writeLongDisplacement(byteDisp);
        }
    }
    else if (auto jv = std::get_if<Jv>(operand))
    {
        for (int i = 3; i >= 0; --i)
        {
            _tw->writeByteHex(jv->offset[i]);
        }
    }
    else if (auto gv = std::get_if<Gv*>(operand))
    {
        auto s = std::get<Register>(**gv);
        writeGeneralPurposeRegister(s);
    }
    else if (auto mem = std::get_if<MemoryAddress32*>(operand))
    {
        for (int i = 3; i >= 0; --i)
        {
            _tw->writeByteHex((*mem)->value[i]);
        }
    }
    else if (auto reg = std::get_if<Register>(operand))
    {
        writeGeneralPurposeRegister(*reg);
    }
    else
    {
        throw std::runtime_error("Could not resolve operand.");
    }
}


void
X86BaselinePrinter::writeGeneralPurposeRegister(const Register _register)
{
    switch (_register)
    {
        case Register::rAX:
            _tw->write("%rax");
            break;
        case Register::rCX:
            _tw->write("%rcx");
            break;
        case Register::rDX:
            _tw->write("%rdx");
            break;
        case Register::rBX:
            _tw->write("%rbx");
            break;
        case Register::rSP:
            _tw->write("%rsp");
            break;
        case Register::rBP:
            _tw->write("%rbp");
            break;
        case Register::rSI:
            _tw->write("%rsi");
            break;
        case Register::rDI:
            _tw->write("%rdi");
            break;
        default:
            throw std::runtime_error("Unknown register.");
    }
}


void
X86BaselinePrinter::writeLongDisplacement(LongDisplacement* displacement)
{
    if (auto sibDisplacement = std::get_if<SibDisplacement*>(&displacement->base))
    {
        _tw->writeSignedHex(displacement->displacement);
        _tw->write("(");
        writeGeneralPurposeRegister((*sibDisplacement)->base);
        _tw->write(",");
        writeGeneralPurposeRegister((*sibDisplacement)->index);
        _tw->write(",");
        switch ((*sibDisplacement)->scale)
        {
            case 1:
                _tw->write("1");
                break;
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
        _tw->write(")");
    }
    else
    {
        throw std::runtime_error("Not implemented four byte displacement on register");
    }
}


void
X86BaselinePrinter::writeByteDisplacement(ByteDisplacement* byteDisp)
{
    _tw->write("[");
    writeGeneralPurposeRegister(byteDisp->base);
    if (byteDisp->displacement > 0)
    {
        _tw->write("+");
    }
    else
    {
        _tw->write("-");
    }
    _tw->writeByteWithHexPrefix(byteDisp->displacement);
    _tw->write("]");
}


void
X86BaselinePrinter::writeAddress()
{
    _tw->writeByteHex((_address >> 24) & 0xff);
    _tw->writeByteHex((_address >> 16) & 0xff);
    _tw->writeByteHex((_address >> 8) & 0xff);
    _tw->writeByteHex(_address & 0xff);
    _tw->write("  ");
}


void
X86BaselinePrinter::writeSizeSuffix(Instruction* instruction)
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

