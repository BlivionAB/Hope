#include "Aarch64BaselinePrinter.h"


namespace elet::domain::compiler::test::aarch
{


Utf8String
Aarch64BaselinePrinter::print()
{
    _tw.addColumn(18);
    _tw.addColumn(40);
    writeColumnHeader();
    for (OneOfInstruction& oneOfInstruction : instructions)
    {
        const Instruction* instruction = reinterpret_cast<Instruction*>(&oneOfInstruction);
        writeAddress();
        _tw.tab();
        writeByteInstruction(instruction);
        _tw.tab();
        switch (instruction->kind)
        {
            case Aarch64Instruction::AdrpImmediate64:
                writeAdrpInstruction(reinterpret_cast<const AdrpInstruction*>(instruction));
                break;
            case Aarch64Instruction::StrImmediateBaseOffset64:
            case Aarch64Instruction::LdrImmediateBaseOffset64:
                writeLoadStoreInstruction(reinterpret_cast<const LoadStoreInstruction*>(instruction));
                break;
            case Aarch64Instruction::StpPreIndex64:
            case Aarch64Instruction::StpBaseOffset64:
            case Aarch64Instruction::LdpPostIndex64:
            case Aarch64Instruction::LdpBaseOffset64:
                writeLoadStorePairInstruction(reinterpret_cast<const LoadStorePairInstruction*>(instruction));
                break;
            case Aarch64Instruction::AddImmediate64:
            case Aarch64Instruction::SubImmediate64:
                writeDataProcessImmediateInstruction(reinterpret_cast<const DataProcessImmediateInstruction*>(instruction));
                break;
            case Aarch64Instruction::Movz64:
                writeMovImmediate(reinterpret_cast<const MovWideImmediateInstruction*>(instruction));
                break;
            case Aarch64Instruction::Mov64:
                writeMov(reinterpret_cast<const MovInstruction*>(instruction));
                break;
            case Aarch64Instruction::Bl:
                writeBl(reinterpret_cast<const UnconditionalBranchImmediateInstruction*>(instruction));
                break;
            case Aarch64Instruction::Ret:
                writeBranchExceptionSyscallInstruction(reinterpret_cast<const BranchExceptionSyscallInstruction*>(instruction));
                break;
            default:
                throw std::runtime_error("Unknown instruction.");
        }
        _tw.newline();
    }
    return _tw.toString();
}


void
Aarch64BaselinePrinter::writeAdrpInstruction(const AdrpInstruction* instruction)
{
    _tw.write("adrp ");
    writeGeneralPurposeRegister64(instruction->rd);
    _tw.writeDisplacement(instruction->immhilo);
}


void
Aarch64BaselinePrinter::writeMov(const MovInstruction* instruction)
{
    _tw.write("mov ");
    writeGeneralPurposeRegister64(instruction->rd);
    _tw.write(", ");
    writeGeneralPurposeRegister64(instruction->rm);
}


void
Aarch64BaselinePrinter::writeMovImmediate(const MovWideImmediateInstruction* instruction)
{
    _tw.write("movz ");
    writeGeneralPurposeRegister32(instruction->rd);
    _tw.write(", #");
    _tw.writeDisplacement(instruction->imm16);
}


void
Aarch64BaselinePrinter::writeLoadStoreInstruction(const LoadStoreInstruction* instruction)
{
    switch (instruction->kind)
    {
        case Aarch64Instruction::StrImmediateBaseOffset64:
            _tw.write("str ");
            break;
        case Aarch64Instruction::LdrImmediateBaseOffset64:
            _tw.write("ldr ");
            break;
        default:
            throw std::runtime_error("Unknown load store instruction.");
    }
    writeGeneralPurposeRegister64(instruction->rt);
    _tw.write(", [");
    writeGeneralPurposeRegister64(instruction->rn);
    writeIndexedAddressSuffix(instruction->addressMode, instruction->imm12);
}

void
Aarch64BaselinePrinter::writeIndexedAddressSuffix(AddressMode addressMode, int16_t offset)
{
    switch (addressMode)
    {
        case AddressMode::PreIndex:
            _tw.write(", #");
            _tw.writeDisplacement(offset);
            _tw.write("]!");
            break;
        case AddressMode::BaseOffset:
            _tw.write(", #");
            _tw.writeDisplacement(offset);
            _tw.write("]");
            break;
        case AddressMode::PostIndex:
            _tw.write("], #");
            _tw.writeDisplacement(offset);
            break;
        default:
            throw std::runtime_error("Unknown address mode.");
    }
}


void
Aarch64BaselinePrinter::writeLoadStorePairInstruction(const LoadStorePairInstruction* instruction)
{
    switch (instruction->kind)
    {
        case Aarch64Instruction::StrImmediateBaseOffset64:
            _tw.write("str ");
            break;
        case Aarch64Instruction::LdpBaseOffset64:
        case Aarch64Instruction::LdpPostIndex64:
            _tw.write("ldp ");
            break;
        case Aarch64Instruction::StpPreIndex64:
        case Aarch64Instruction::StpBaseOffset64:
            _tw.write("stp ");
            break;
        default:
            throw std::runtime_error("Unknown load store instruction.");
    }
    writeGeneralPurposeRegister64(instruction->rt);
    _tw.write(", ");
    writeGeneralPurposeRegister64(instruction->rt2);
    _tw.write(", ");
    _tw.write("[");
    writeGeneralPurposeRegister64(instruction->rn);
    writeIndexedAddressSuffix(instruction->addressMode, instruction->imm7);
}


void
Aarch64BaselinePrinter::writeGeneralPurposeRegister32(Register reg)
{
    switch (reg)
    {
        case Register::r0:
            _tw.write("w0");
            break;
        case Register::r1:
            _tw.write("w1");
            break;
        case Register::r2:
            _tw.write("w2");
            break;
        case Register::r3:
            _tw.write("w3");
            break;
        case Register::r4:
            _tw.write("w4");
            break;
        case Register::r5:
            _tw.write("w5");
            break;
        case Register::r6:
            _tw.write("w6");
            break;
        case Register::r7:
            _tw.write("w7");
            break;
        case Register::r8:
            _tw.write("w8");
            break;
        case Register::fp:
            _tw.write("fp");
            break;
        case Register::lr:
            _tw.write("lr");
            break;
        case Register::sp:
            _tw.write("sp");
            break;
        default:
            throw std::runtime_error("Unknown general purpose register.");
    }
}


void
Aarch64BaselinePrinter::writeGeneralPurposeRegister64(Register reg)
{
    switch (reg)
    {
        case Register::r0:
            _tw.write("x0");
            break;
        case Register::r1:
            _tw.write("x1");
            break;
        case Register::r2:
            _tw.write("x2");
            break;
        case Register::r3:
            _tw.write("x3");
            break;
        case Register::r4:
            _tw.write("x4");
            break;
        case Register::r5:
            _tw.write("x5");
            break;
        case Register::r6:
            _tw.write("x6");
            break;
        case Register::r7:
            _tw.write("x7");
            break;
        case Register::r8:
            _tw.write("x8");
            break;
        case Register::fp:
            _tw.write("fp");
            break;
        case Register::lr:
            _tw.write("lr");
            break;
        case Register::sp:
            _tw.write("sp");
            break;
        default:
            throw std::runtime_error("Unknown general purpose register.");
    }
}


void
Aarch64BaselinePrinter::writeByteInstruction(const Instruction* instruction)
{
    for (const auto byte : instruction->bytes)
    {
        _tw.writeByteHex(byte);
        ++address;
    }
}

void
Aarch64BaselinePrinter::writeBranchExceptionSyscallInstruction(const BranchExceptionSyscallInstruction* instruction)
{
    if (instruction->kind == Aarch64Instruction::Ret)
    {
        _tw.write("ret ");
        writeGeneralPurposeRegister64(instruction->rn);
    }
}


void
Aarch64BaselinePrinter::writeBl(const UnconditionalBranchImmediateInstruction* instruction)
{
    _tw.write("bl ");

    uint64_t vmAddress = instruction->imm26 * 4 + address - 4 /* We wrote the byte instruction above*/;
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
Aarch64BaselinePrinter::writeDataProcessImmediateInstruction(const DataProcessImmediateInstruction* instruction)
{
    switch (instruction->kind)
    {
        case Aarch64Instruction::AddImmediate64:
            _tw.write("add ");
            break;
        case Aarch64Instruction::SubImmediate64:
            _tw.write("sub ");
            break;
        default:
            throw std::runtime_error("Unknown instruction");
    }
    writeGeneralPurposeRegister64(instruction->rd);
    _tw.write(", ");
    writeGeneralPurposeRegister64(instruction->rn);
    _tw.write(", ");
    _tw.writeDisplacement(instruction->imm12);
}


}