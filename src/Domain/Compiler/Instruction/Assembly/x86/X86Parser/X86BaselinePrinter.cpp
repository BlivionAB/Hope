#include "X86BaselinePrinter.h"
#include "X86BaselineParser.h"
#include "Domain/Compiler/Instruction/Assembly/x86/OpCode/GeneralOpCodes.h"
#include "X86Types.h"


namespace elet::domain::compiler::instruction::x86
{


X86BaselinePrinter::X86BaselinePrinter()
{
    _parser = new X86BaselineParser();
}


void
X86BaselinePrinter::print(List<uint8_t>* output)
{
    auto instructionList = _parser->parse(output);
    for (const auto& instruction : *instructionList)
    {
        writeInstruction(instruction);
    }
    std::cout << _tw.toString() << std::endl;
}


void
X86BaselinePrinter::writeInstruction(Instruction* instruction)
{
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
            writeOperand(&instruction->operand2);
            break;
    }
    _tw.newline();
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

    }
    else if (auto jv = std::get_if<Jv>(operand))
    {
        _tw.writeDoubleWordHexInteger(jv->offset);
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
        case Register::RBP:
            _tw.write("rbp");
            break;
        case Register::EBP:
            _tw.write("ebp");
            break;
        case Register::ESP:
            _tw.write("esp");
            break;
        case Register::ESI:
            _tw.write("esi");
            break;
        case Register::EDI:
            _tw.write("edi");
            break;
        default:
            throw std::exception();
    }
}


}

