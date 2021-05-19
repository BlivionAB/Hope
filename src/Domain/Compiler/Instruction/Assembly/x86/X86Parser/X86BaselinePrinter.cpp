#include <fstream>
#include "X86BaselinePrinter.h"
#include "X86BaselineParser.h"
#include "X86Types.h"
#include "Foundation/Path.h"


namespace elet::domain::compiler::instruction::x86
{


X86BaselinePrinter::X86BaselinePrinter()
{
    _parser = new X86BaselineParser();
}


void
X86BaselinePrinter::print(List<uint8_t>* output)
{
//    auto instructionList = _parser->parse(output);
//    _tw.addColumn(10);
//    _tw.addColumn(30);
//    _tw.addColumn(42);
//    writeColumnHeader();
//    for (const auto& instruction : *instructionList)
//    {
//        writeInstruction(instruction);
//    }
//    std::cout << _tw.toString() << std::endl;
}


void
X86BaselinePrinter::writeColumnHeader()
{
    _tw.write("Address");
    _tw.tab();
    _tw.write("Instruction");
    _tw.tab();
    _tw.write("Bytes");
    _tw.tab();
    _tw.write("Symbol");
    _tw.newline();
    _tw.write("=================================================");
    _tw.newline();
}


void
X86BaselinePrinter::writeInstruction(Instruction* instruction)
{
    writeAddress();
    _tw.tab();
    switch (instruction->kind)
    {
        case InstructionKind::Ret:
            _tw.write("ret");
            break;
        case InstructionKind::Call:
            _tw.write("call ");
            writeOperand(&instruction->operand1);
            break;
        case InstructionKind::Push:
            _tw.write("push ");
            writeGeneralPurposeRegister(std::get<Register>(instruction->operand1));
            break;
        case InstructionKind::Mov:
            _tw.write("mov ");
            writeOperand(&instruction->operand1);
            _tw.write(",");
            writeOperand(&instruction->operand2);
            break;
    }
    _tw.tab();
    writeByteInstruction(instruction);
    _tw.newline();
}


void
X86BaselinePrinter::writeByteInstruction(Instruction* instruction)
{
    for (const auto byte : instruction->bytes)
    {
        _tw.writeByte(byte);
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

    }
    else if (auto jv = std::get_if<Jv>(operand))
    {
        for (int i = 3; i >= 0; --i)
        {
            _tw.writeByte(jv->offset[i]);
        }
    }
    else if (auto gv = std::get_if<Gv*>(operand))
    {
        auto s = std::get<Register>(**gv);
        writeGeneralPurposeRegister(s);
    }
}


void
X86BaselinePrinter::writeGeneralPurposeRegister(const Register _register)
{
    switch (_register)
    {
        case Register::EAX:
            _tw.write("eax");
            break;
        case Register::ECX:
            _tw.write("ecx");
            break;
        case Register::EDX:
            _tw.write("edx");
            break;
        case Register::EBX:
            _tw.write("ebx");
            break;
        case Register::ESP:
            _tw.write("esp");
            break;
        case Register::EBP:
            _tw.write("ebp");
            break;
        case Register::ESI:
            _tw.write("esi");
            break;
        case Register::EDI:
            _tw.write("edi");
            break;


        case Register::RAX:
            _tw.write("rax");
            break;
        case Register::RCX:
            _tw.write("rcx");
            break;
        case Register::RDX:
            _tw.write("rdx");
            break;
        case Register::RBX:
            _tw.write("rbx");
            break;
        case Register::RSP:
            _tw.write("rsp");
            break;
        case Register::RBP:
            _tw.write("rbp");
            break;
        case Register::RSI:
            _tw.write("rsi");
            break;
        case Register::RDI:
            _tw.write("rdi");
            break;
        default:
            throw std::exception();
    }
}

void
X86BaselinePrinter::writeByteDisplacement(ByteDisplacement* byteDisp)
{
    _tw.write("[");
    writeGeneralPurposeRegister(byteDisp->base);
    if (byteDisp->displacement > 0)
    {
        _tw.write("+");
    }
    else
    {
        _tw.write("-");
    }
    _tw.writeByteWithHexPrefix(byteDisp->displacement);
    _tw.write("]");
}

void
X86BaselinePrinter::writeAddress()
{
    _tw.writeByte((_address >> 24) & 0xff);
    _tw.writeByte((_address >> 16) & 0xff);
    _tw.writeByte((_address >> 8) & 0xff);
    _tw.writeByte(_address & 0xff);
    _tw.write("  ");
}


}

