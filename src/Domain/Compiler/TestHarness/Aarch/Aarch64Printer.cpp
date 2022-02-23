#include "Aarch64Printer.h"


namespace elet::domain::compiler::test::aarch
{
    void
    Aarch64Printer::writeInstructions(const List<OneOfInstruction>& instructions)
    {
        for (OneOfInstruction& oneOfInstruction : instructions)
        {
            const Instruction* instruction = reinterpret_cast<Instruction*>(&oneOfInstruction);
            writeAddress();
            _tw.tab();
            writeByteInstruction(instruction);
            _tw.tab();
            switch (instruction->kind)
            {
                case InstructionKind::AndImmediate:
                    writeAndImmediateInstruction(reinterpret_cast<const AndImmediateInstruction*>(instruction));
                    break;
                case InstructionKind::Adr:
                    writeAdrInstruction(reinterpret_cast<const AdrInstruction*>(instruction));
                    break;
                case InstructionKind::Adrp:
                    writeAdrpInstruction(reinterpret_cast<const AdrpInstruction*>(instruction));
                    break;
                case InstructionKind::AddShiftedRegister:
                case InstructionKind::SubShiftedRegister:
                    writeShiftedRegisterInstruction(reinterpret_cast<const ShiftedRegisterInstruction*>(instruction));
                    break;
                case InstructionKind::Madd:
                case InstructionKind::Msub:
                    writeMaddSubInstruction(reinterpret_cast<const MaddSubInstruction*>(instruction));
                    break;
                case InstructionKind::Sdiv:
                case InstructionKind::Udiv:
                    writeDivInstruction(reinterpret_cast<const DivInstruction*>(instruction));
                    break;
                case InstructionKind::Ldr:
                    writeLdr(reinterpret_cast<const LdrInstruction*>(instruction));
                    break;
                case InstructionKind::StpPreIndex:
                case InstructionKind::StpOffset:
                case InstructionKind::LdpPostIndex:
                case InstructionKind::LdpOffset:
                    writeLoadStorePairInstruction(reinterpret_cast<const LoadStorePairInstruction*>(instruction));
                    break;
                case InstructionKind::StrImmediateUnsignedOffset:
                case InstructionKind::LdrImmediateUnsignedOffset:
                    writeLdrStrImmediateUnsignedOffsetInstruction(reinterpret_cast<const StrUnsignedOffsetInstruction*>(instruction));
                    break;
                case InstructionKind::LdrbImmediateUnsignedOffset:
                case InstructionKind::StrbImmediateUnsignedOffset:
                    writeLdrbStrbImmediateUnsignedOffsetInstruction(reinterpret_cast<const LdrbStrbImmediateUnsignedOffsetInstruction*>(instruction));
                    break;
                case InstructionKind::LdrhImmediateUnsignedOffset:
                case InstructionKind::StrhImmediateUnsignedOffset:
                    writeLdrhStrhImmediateUnsignedOffsetInstruction(reinterpret_cast<const LdrhStrhImmediateUnsignedOffsetInstruction*>(instruction));
                    break;
                case InstructionKind::LdrsbImmediateUnsignedOffset:
                    writeLdrsbImmediateUnsignedOffsetInstruction(reinterpret_cast<const LdrsbImmediateUnsignedOffsetInstruction*>(instruction));
                    break;
                case InstructionKind::LdrshImmediateUnsignedOffset:
                    writeLdrshImmediateUnsignedOffsetInstruction(reinterpret_cast<const LdrshImmediateUnsignedOffsetInstruction*>(instruction));
                    break;
                case InstructionKind::AddImmediate:
                case InstructionKind::SubImmediate:
                    writeDataProcessImmediateInstruction(reinterpret_cast<const DataProcessImmediateInstruction*>(instruction));
                    break;
                case InstructionKind::OrrImmediate:
                    writeOrrImmediate(reinterpret_cast<const OrrImmediateInstruction*>(instruction));
                    break;
                case InstructionKind::Movz:
                    writeMovz(reinterpret_cast<const MovzInstruction*>(instruction));
                    break;
                case InstructionKind::Movn:
                    writeMovn(reinterpret_cast<const MovnInstruction*>(instruction));
                    break;
                case InstructionKind::Movk:
                    writeMovk(reinterpret_cast<const MovkInstruction*>(instruction));
                    break;
                case InstructionKind::Sxtb:
                    writeSxtb(reinterpret_cast<const SxtbInstruction*>(instruction));
                    break;
                case InstructionKind::Sxth:
                    writeSxth(reinterpret_cast<const SxthInstruction*>(instruction));
                    break;
                case InstructionKind::B:
                    writeB(reinterpret_cast<const BInstruction*>(instruction));
                    break;
                case InstructionKind::Bl:
                    writeBl(reinterpret_cast<const BlInstruction*>(instruction));
                    break;
                case InstructionKind::Br:
                    writeBr(reinterpret_cast<const BrInstruction*>(instruction));
                    break;
                case InstructionKind::Ret:
                    writeBranchExceptionSyscallInstruction(reinterpret_cast<const BrInstruction*>(instruction));
                    break;
                case InstructionKind::Nop:
                    writeNopInstruction();
                    break;
                case InstructionKind::Udf:
                    writeUdf(reinterpret_cast<const UdfInstruction*>(instruction));
                    break;
                default:
                    throw std::runtime_error("Unknown instruction.");
            }
            _tw.newline();
        }
    }


    void
    Aarch64Printer::writeAdrInstruction(const AdrInstruction* instruction)
    {
        _tw.write("adr ");
        writeGeneralPurposeRegister(instruction->rd, instruction);
        _tw.write(", ");
        _tw.writeSignedHexValue(instruction->immhilo);
    }


    void
    Aarch64Printer::writeAdrpInstruction(const AdrpInstruction* instruction)
    {
        _tw.write("adrp ");
        writeGeneralPurposeRegister(instruction->rd, instruction);
        _tw.write(", ");
        _tw.writeSignedHexValue(instruction->immhilo);
    }


    void
    Aarch64Printer::writeMovz(const MovzInstruction* instruction)
    {
        if (!(instruction->imm16 == 0 && instruction->hw != Hw::_0))
        {
            _tw.write("mov ");
        }
        else
        {
            _tw.write("movz ");
        }
        writeGeneralPurposeRegister(instruction->Rd, instruction);
        _tw.write(", #");
        _tw.writeSignedHexValue(instruction->immediateValue);
    }


    void
    Aarch64Printer::writeLdrStrImmediateUnsignedOffsetInstruction(const LdrStrImmediateUnsignedOffsetInstruction* instruction)
    {
        if (instruction->kind == InstructionKind::LdrImmediateUnsignedOffset)
        {
            _tw.write("ldr ");
        }
        else
        {
            _tw.write("str ");
        }
        writeGeneralPurposeRegister(instruction->Rt, instruction);
        _tw.write(", [");
        writeGeneralPurposeRegister(instruction->Rn, instruction);
        _tw.write(", #");
        if (instruction->is64Bit)
        {
            _tw.writeUnsignedHexValue(instruction->imm12 * 8);
        }
        else
        {
            _tw.writeUnsignedHexValue(instruction->imm12 * 4);
        }
        _tw.write("]");
    }


    void
    Aarch64Printer::writeLdrbStrbImmediateUnsignedOffsetInstruction(const LdrbStrbImmediateUnsignedOffsetInstruction* instruction)
    {
        if (instruction->kind == InstructionKind::LdrbImmediateUnsignedOffset)
        {
            _tw.write("ldrb ");
        }
        else
        {
            _tw.write("strb ");
        }
        writeGeneralPurposeRegister(instruction->Rt, instruction);
        _tw.write(", [");
        writeGeneralPurposeRegister(instruction->Rn, instruction);
        _tw.write(", #");
        _tw.writeUnsignedHexValue(instruction->imm12);
        _tw.write("]");
    }


    void
    Aarch64Printer::writeLdrsbImmediateUnsignedOffsetInstruction(const LdrsbImmediateUnsignedOffsetInstruction* instruction)
    {
        _tw.write("ldrsb ");
        writeGeneralPurposeRegister(instruction->Rt, instruction);
        _tw.write(", [");
        writeGeneralPurposeRegister(instruction->Rn, instruction);
        _tw.write(", #");
        _tw.writeUnsignedHexValue(instruction->imm12);
        _tw.write("]");
    }


    void
    Aarch64Printer::writeLdrhStrhImmediateUnsignedOffsetInstruction(const LdrhStrhImmediateUnsignedOffsetInstruction* instruction)
    {
        if (instruction->kind == InstructionKind::LdrhImmediateUnsignedOffset)
        {
            _tw.write("ldrh ");
        }
        else
        {
            _tw.write("strh ");
        }
        writeGeneralPurposeRegister(instruction->Rt, instruction);
        _tw.write(", [");
        writeGeneralPurposeRegister(instruction->Rn, instruction);
        _tw.write(", #");
        _tw.writeUnsignedHexValue(instruction->imm12);
        _tw.write("]");
    }


    void
    Aarch64Printer::writeLdrshImmediateUnsignedOffsetInstruction(const LdrshImmediateUnsignedOffsetInstruction* instruction)
    {
        _tw.write("ldrsh ");
        writeGeneralPurposeRegister(instruction->Rt, instruction);
        _tw.write(", [");
        writeGeneralPurposeRegister(instruction->Rn, instruction);
        _tw.write(", #");
        _tw.writeUnsignedHexValue(instruction->imm12);
        _tw.write("]");
    }


    void
    Aarch64Printer::writeIndexedAddressSuffix(AddressMode addressMode, int16_t offset)
    {
        switch (addressMode)
        {
            case AddressMode::PreIndex:
                _tw.write(", #");
                _tw.writeSignedHexValue(offset);
                _tw.write("]!");
                break;
            case AddressMode::BaseOffset:
                _tw.write(", #");
                _tw.writeSignedHexValue(offset);
                _tw.write("]");
                break;
            case AddressMode::PostIndex:
                _tw.write("], #");
                _tw.writeSignedHexValue(offset);
                break;
            case AddressMode::UnsignedOffset:
                break;
            default:
                throw std::runtime_error("Unknown address mode.");
        }
    }


    void
    Aarch64Printer::writeLoadStorePairInstruction(const LoadStorePairInstruction* instruction)
    {
        switch (instruction->kind)
        {
            case InstructionKind::LdpOffset:
            case InstructionKind::LdpPostIndex:
                _tw.write("ldp ");
                break;
            case InstructionKind::StpPreIndex:
            case InstructionKind::StpOffset:
                _tw.write("stp ");
                break;
            default:
                throw std::runtime_error("Unknown load store instruction.");
        }
        writeGeneralPurposeRegister(instruction->Rt, instruction);
        _tw.write(", ");
        writeGeneralPurposeRegister(instruction->Rt2, instruction);
        _tw.write(", ");
        _tw.write("[");
        writeGeneralPurposeRegister(instruction->Rn, instruction);
        writeIndexedAddressSuffix(instruction->addressMode, instruction->imm7);
    }


    void
    Aarch64Printer::writeGeneralPurposeRegister(Register reg, const Instruction* instruction)
    {
        if (reg >= Register::r0 && reg <= Register::r28)
        {
            if (instruction->is64Bit)
            {
                _tw.write("x");
            }
            else
            {
                _tw.write("w");
            }
            switch (reg)
            {
                case Register::r0:
                    _tw.write("0");
                    break;
                case Register::r1:
                    _tw.write("1");
                    break;
                case Register::r2:
                    _tw.write("2");
                    break;
                case Register::r3:
                    _tw.write("3");
                    break;
                case Register::r4:
                    _tw.write("4");
                    break;
                case Register::r5:
                    _tw.write("5");
                    break;
                case Register::r6:
                    _tw.write("6");
                    break;
                case Register::r7:
                    _tw.write("7");
                    break;
                case Register::r8:
                    _tw.write("8");
                    break;
                case Register::r16:
                    _tw.write("16");
                    break;
                case Register::r17:
                    _tw.write("17");
                    break;
                default:
                    throw std::runtime_error("Unknown general purpose register.");
            }
        }
        else
        {
            switch (reg)
            {
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
    }


    void
    Aarch64Printer::writeByteInstruction(const Instruction* instruction)
    {
        // Aarch is little endian. So we have to reverse the bytes.
        for (auto it = instruction->bytes.rbegin(); it != instruction->bytes.rend(); ++it)
        {
            _tw.writeByteHex(*it);
            ++textSectionStartOffset;
        }
    }

    void
    Aarch64Printer::writeBranchExceptionSyscallInstruction(const BrInstruction* instruction)
    {
        if (instruction->kind == InstructionKind::Ret)
        {
            _tw.write("ret ");
            writeGeneralPurposeRegister(instruction->Rn, instruction);
        }
    }


    void
    Aarch64Printer::writeBl(const BlInstruction* instruction)
    {
        _tw.write("bl ");

        uint64_t vmAddress = instruction->imm26 * 4 + textSectionStartOffset - 4 /* We wrote the byte instruction above*/;
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
    Aarch64Printer::writeDataProcessImmediateInstruction(const DataProcessImmediateInstruction* instruction)
    {
        switch (instruction->kind)
        {
            case InstructionKind::AddImmediate:
                _tw.write("add ");
                break;
            case InstructionKind::SubImmediate:
                _tw.write("sub ");
                break;
            default:
                throw std::runtime_error("Unknown instruction");
        }
        writeGeneralPurposeRegister(instruction->rd, instruction);
        _tw.write(", ");
        writeGeneralPurposeRegister(instruction->rn, instruction);
        _tw.write(", ");
        _tw.writeSignedImmediateValue(instruction->imm12);
    }


    void
    Aarch64Printer::writeNopInstruction()
    {
        _tw.write("nop");
    }


    void
    Aarch64Printer::writeLdr(const LdrInstruction* instruction)
    {
        _tw.write("ldr ");
        writeGeneralPurposeRegister(instruction->Rt, instruction);
        _tw.write(", ");
        _tw.writeSignedImmediateValue(instruction->imm19);
    }


    void
    Aarch64Printer::writeB(const BInstruction* instruction)
    {
        _tw.write("b ");

        uint64_t vmAddress = instruction->imm26 * 4 + textSectionStartOffset - 4 /* We wrote the byte instruction above*/;
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
    Aarch64Printer::writeUdf(const UdfInstruction* instruction)
    {
        _tw.write("udf ");
        _tw.writeSignedImmediateValue(instruction->imm16);
    }


    void
    Aarch64Printer::writeBr(const BrInstruction* instruction)
    {
        _tw.write("br ");
        writeGeneralPurposeRegister(instruction->Rn, instruction);
    }

    void
    Aarch64Printer::writeOrrImmediate(const OrrImmediateInstruction* instruction)
    {
        if (instruction->Rn == Aarch64Register::r31)
        {
            _tw.write("mov ");
            writeGeneralPurposeRegister(instruction->Rd, instruction);
            _tw.write(", ");
            _tw.writeSignedImmediateValue(instruction->value);
        }
        else
        {
            throw std::runtime_error("Have not implemented instruction instruction yet.");
        }
    }


    void
    Aarch64Printer::writeMovn(const MovnInstruction* instruction)
    {
        _tw.write("movn ");
        writeGeneralPurposeRegister(instruction->Rd, instruction);
        _tw.write(", ");
        _tw.writeUnsignedHexValue(instruction->immediateValue);
    }


    void
    Aarch64Printer::writeMovk(const MovkInstruction* instruction)
    {
        _tw.write("movk ");
        writeGeneralPurposeRegister(instruction->Rd, instruction);
        _tw.write(", ");
        _tw.writeSignedImmediateValue(instruction->immediateValue);
        writeHw(instruction);
    }


    void
    Aarch64Printer::writeHw(const MovkInstruction* movInstruction)
    {
        if (movInstruction->hw == Hw::_0)
        {
            return;
        }
        _tw.write(", lsl #");
        switch (movInstruction->hw)
        {
            case Hw::_16:
                _tw.write("16");
                break;
            case Hw::_32:
                _tw.write("32");
                break;
            case Hw::_48:
                _tw.write("48");
                break;
            default:
                throw std::runtime_error("Unknown hw case.");
        }
    }


    void
    Aarch64Printer::writeShiftedRegisterInstruction(const ShiftedRegisterInstruction* instruction)
    {
        if (instruction->kind == InstructionKind::AddShiftedRegister)
        {
            _tw.write("add ");
        }
        else
        {
            _tw.write("sub ");
        }
        writeGeneralPurposeRegister(instruction->Rd, instruction);
        _tw.write(", ");
        writeGeneralPurposeRegister(instruction->Rn, instruction);
        _tw.write(", ");
        writeGeneralPurposeRegister(instruction->Rm, instruction);
        if (instruction->imm6)
        {
            _tw.write(", ");
            _tw.writeUnsignedHexValue(instruction->imm6);
        }
    }


    void
    Aarch64Printer::writeMaddSubInstruction(const MaddSubInstruction* instruction)
    {
        if (instruction->kind == InstructionKind::Madd)
        {
            if (instruction->Ra == Register::Zero)
            {
                _tw.write("mul ");
            }
            else
            {
                _tw.write("madd ");
            }
        }
        else
        {
            if (instruction->Ra == Register::Zero)
            {
                _tw.write("mneg ");
            }
            else
            {
                _tw.write("msub ");
            }
        }
        writeGeneralPurposeRegister(instruction->Rd, instruction);
        _tw.write(", ");
        writeGeneralPurposeRegister(instruction->Rn, instruction);
        _tw.write(", ");
        writeGeneralPurposeRegister(instruction->Rm, instruction);
        if (instruction->Ra != Register::Zero)
        {
            _tw.write(", ");
            writeGeneralPurposeRegister(instruction->Ra, instruction);
        }
    }

    void
    Aarch64Printer::writeDivInstruction(const DivInstruction* instruction)
    {
        if (instruction->kind == InstructionKind::Sdiv)
        {
            _tw.write("sdiv ");
        }
        else
        {
            _tw.write("udiv ");
        }
        writeGeneralPurposeRegister(instruction->Rd, instruction);
        _tw.write(", ");
        writeGeneralPurposeRegister(instruction->Rn, instruction);
        _tw.write(", ");
        writeGeneralPurposeRegister(instruction->Rm, instruction);
    }


    void
    Aarch64Printer::writeAndImmediateInstruction(const AndImmediateInstruction* instruction)
    {
        _tw.write("and ");
        writeGeneralPurposeRegister(instruction->Rd, instruction);
        _tw.write(", ");
        writeGeneralPurposeRegister(instruction->Rn, instruction);
        _tw.write(", ");
        _tw.writeUnsignedHexValue(instruction->value);
    }


    void
    Aarch64Printer::writeSxtb(const SxtbInstruction* instruction)
    {
        _tw.write("sxtb ");
        writeGeneralPurposeRegister(instruction->Rd, instruction);
        _tw.write(", ");
        writeGeneralPurposeRegister(instruction->Rn, instruction);
    }


    void
    Aarch64Printer::writeSxth(const SxthInstruction* instruction)
    {
        _tw.write("sxth ");
        writeGeneralPurposeRegister(instruction->Rd, instruction);
        _tw.write(", ");
        writeGeneralPurposeRegister(instruction->Rn, instruction);
    }
}