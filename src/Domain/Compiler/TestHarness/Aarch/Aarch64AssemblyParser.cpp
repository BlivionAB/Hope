#include "Aarch64AssemblyParser.h"
#include "Aarch64Instructions.h"
#include <Domain/Compiler/Instruction/Assembly/Aarch/AArch64Encodings.h>
#include <Foundation/Bit.h>

namespace elet::domain::compiler::test::aarch
{
    using namespace elet::foundation;
    using namespace elet::domain::compiler::instruction::output;


    void
    Aarch64AssemblyParser::parse(List<OneOfInstruction>& instructions, List<uint8_t>& output, size_t offset, size_t size)
    {
        _output = &output;
        _offset = offset;
        _cursor = offset;
        _size = size;

        while (_cursor - _offset < _size)
        {
            Instruction* instruction = reinterpret_cast<Instruction*>(instructions.emplace());
            uint32_t dw = getDoubleWord(instruction);
            if ((dw & Aarch64Instruction::BrMask) == Aarch64Instruction::Br)
            {
                parseBrInstruction(instruction, dw);
                continue;
            }
            if (tryParse26Instructions(instruction, dw))
            {
                continue;
            }
            if (tryParse25Instructions(instruction, dw))
            {
                continue;
            }
            if (tryParse24Instructions(instruction, dw))
            {
                continue;
            }
            if (tryParse23Instructions(instruction, dw))
            {
                continue;
            }
            if (tryParse22Instructions(instruction, dw))
            {
                continue;
            }
            if (tryParse21Instructions(instruction, dw))
            {
                continue;
            }
            if (Aarch64Instruction::Udf == (dw && Aarch64Instruction::Mask16))
            {
                parseUdfInstruction(instruction, dw);
                continue;
            }
            if (dw == Aarch64Instruction::Nop)
            {
                NopInstruction* instr = reinterpret_cast<NopInstruction*>(instruction);
                instr->kind = Aarch64Instruction::Nop;
                continue;
            }
            throw std::runtime_error("Could not find instruction when parsing.");
        }
    }


    void
    Aarch64AssemblyParser::parseBrInstruction(Instruction* instruction, uint32_t dw)
    {
        BrInstruction* instr = reinterpret_cast<BrInstruction*>(instruction);
        instr->kind = Aarch64Instruction::Br;
        instr->rn = Rn(dw);
    }


    bool
    Aarch64AssemblyParser::tryParse26Instructions(Instruction* instruction, uint32_t dw)
    {
        uint32_t kind26 = dw & Mask26;

        switch (kind26)
        {
            case Aarch64Instruction::B:
            {
                BInstruction* b = reinterpret_cast<BInstruction*>(instruction);
                b->kind = Aarch64Instruction::B;
                b->imm26 = imm26(dw);
                return true;
            }
            case Aarch64Instruction::Bl:
            {
                BlInstruction* bl = reinterpret_cast<BlInstruction*>(instruction);
                bl->kind = Aarch64Instruction::Bl;
                bl->imm26 = imm26(dw);
                return true;
            }
        }
        return false;
    }

    bool
    Aarch64AssemblyParser::tryParse25Instructions(Instruction* instruction, uint32_t dw)
    {
        uint32_t kind25 = dw & Mask25;

        if (Aarch64Instruction::UnconditionalBranchRegister == kind25)
        {
            BrInstruction* brexsysc = reinterpret_cast<BrInstruction*>(instruction);
            brexsysc->kind = Aarch64Instruction::Ret;
            brexsysc->rn = Rn(dw);
            return true;
        }
        return false;
    }




    bool
    Aarch64AssemblyParser::tryParse24Instructions(Instruction* instruction, uint32_t dw)
    {
        uint32_t kind24 = dw & Mask24;

        switch (kind24)
        {
            case Aarch64Instruction::Ldr64:
            {
                LdrInstruction* instr = reinterpret_cast<LdrInstruction*>(instruction);
                instr->kind = Aarch64Instruction::Ldr64;
                instr->rt = Rt(dw);
                return true;
            }
            case Aarch64Instruction::Ldr32:
            {
                LdrInstruction* instr = reinterpret_cast<LdrInstruction*>(instruction);
                instr->kind = Aarch64Instruction::Ldr32;
                instr->rt = Rt(dw);
                return true;
            }
            case Aarch64Instruction::Adr:
            {
                AdrInstruction* adrp = reinterpret_cast<AdrInstruction*>(instruction);
                adrp->kind = Aarch64Instruction::Adr;
                adrp->rd = Rd(dw);
                adrp->immhilo = immhilo(dw);
                return true;
            }
            case Aarch64Instruction::Adrp:
            {
                AdrInstruction* adrp = reinterpret_cast<AdrInstruction*>(instruction);
                adrp->kind = Aarch64Instruction::Adrp;
                adrp->rd = Rd(dw);
                adrp->immhilo = immhilo(dw);
                return true;
            }
            case Aarch64Instruction::Add_ShiftedRegister:
            {
                Add_ShiftedRegisterInstruction* add = reinterpret_cast<Add_ShiftedRegisterInstruction*>(instruction);
                add->kind = Aarch64Instruction::Add_ShiftedRegister;
                add->Rd = Rd(dw);
                add->Rn = Rn(dw);
                add->Rm = Rm(dw);
                add->imm6 = imm6(dw);
                add->shift = shift(dw);
                return true;
            }
        }
        return false;
    }


    bool
    Aarch64AssemblyParser::tryParse23Instructions(Instruction* instruction, uint32_t dw)
    {
        uint32_t kind23 = dw & Aarch64Instruction::Mask23;

        switch (kind23)
        {
            case Aarch64Instruction::OrrImmediate:
                parseOrrInstruction(reinterpret_cast<OrrImmediateInstruction*>(instruction), dw);
                return true;
            case Aarch64Instruction::Movz:
                parseMovzInstruction(reinterpret_cast<MovzInstruction*>(instruction), dw);
                return true;
            case Aarch64Instruction::Movn:
                parseMovnInstruction(reinterpret_cast<MovnInstruction*>(instruction), dw);
                return true;
            case Aarch64Instruction::Movk:
                parseMovkInstruction(reinterpret_cast<MovkInstruction*>(instruction), dw);
                return true;
        }
        return false;
    }


    void
    Aarch64AssemblyParser::parseOrrInstruction(OrrImmediateInstruction* orrInstruction, uint32_t dw)
    {
        orrInstruction->kind = Aarch64Instruction::OrrImmediate;
        orrInstruction->rd = Rd(dw);
        orrInstruction->rn = Rn(dw);
        if (sf(dw))
        {
            orrInstruction->immediateValue = decodeBitmaskImmediate(dw, RegistrySize::_64);
        }
        else
        {
            orrInstruction->immediateValue = decodeBitmaskImmediate(dw, RegistrySize::_32);
        }
    }


    uint64_t
    Aarch64AssemblyParser::decodeBitmaskImmediate(uint32_t dw, RegistrySize registrySize)
    {
        uint32_t N = (dw >> 22) & 1;
        uint32_t immr = (dw >> 16) & 0x3f;
        uint32_t imms = (dw >> 10) & 0x3f;

        int bitIndex = 31 - Bit::countLeadingZeroes((N << 6) | (~imms & 0x3f));
        unsigned int elementSize = (1 << bitIndex);
        unsigned int R = immr & (elementSize - 1);
        unsigned int S = imms & (elementSize - 1);

        uint64_t pattern = (1ULL << (S + 1)) - 1;
        for (unsigned int i = 0; i < R; ++i)
        {
            pattern |= Bit::rotateRight(pattern, elementSize);
        }

        while (elementSize != registrySize)
        {
            pattern |= (pattern << elementSize);
            elementSize *= 2;
        }

        return pattern;
    }


    bool
    Aarch64AssemblyParser::tryParse22Instructions(Instruction* instruction, uint32_t dw)
    {
        uint32_t kind22 = dw & Mask22;

        switch (kind22)
        {
            case Aarch64Instruction::StrImmediateBaseOffset64:
            case Aarch64Instruction::LdrImmediateBaseOffset64:
                parseLoadStoreInstruction(instruction, dw, kind22);
                return true;
            case Aarch64Instruction::StpPreIndex64:
            case Aarch64Instruction::StpBaseOffset64:
            case Aarch64Instruction::LdpPostIndex64:
            case Aarch64Instruction::LdpBaseOffset64:
                parseLoadStorePairInstruction(instruction, dw, kind22);
                return true;
            case Aarch64Instruction::AddImmediate64:
            case Aarch64Instruction::SubImmediate64:
                parseDataProcessImmediateInstruction(instruction, dw, kind22);
                return true;
            case Aarch64Instruction::LdrImmediateUnsignedOffset:
            case Aarch64Instruction::StrImmediateUnsignedOffset:
                parseStrLdrImmediateUnsignedOffsetInstruction(reinterpret_cast<StrUnsignedOffsetInstruction*>(instruction), dw, kind22);
                return true;

        }
        return false;
    }


    void
    Aarch64AssemblyParser::parseStrLdrImmediateUnsignedOffsetInstruction(StrUnsignedOffsetInstruction* instruction, uint32_t dw, uint32_t kind22)
    {
        instruction->kind = static_cast<Aarch64Instruction>(kind22);
        instruction->Rn = Rn(dw);
        instruction->Rt = Rt(dw);
        instruction->imm12 = imm12(dw);
    }


    void
    Aarch64AssemblyParser::parseDataProcessImmediateInstruction(Instruction* instruction, uint32_t dw, uint32_t kind22)
    {
        DataProcessImmediateInstruction* dataProcessImmediateInstruction = reinterpret_cast<DataProcessImmediateInstruction*>(instruction);
        dataProcessImmediateInstruction->kind = static_cast<Aarch64Instruction>(kind22);
        dataProcessImmediateInstruction->rd = Rd(dw);
        dataProcessImmediateInstruction->rn = Rn(dw);
        dataProcessImmediateInstruction->imm12 = imm12(dw);
    }



    void
    Aarch64AssemblyParser::parseLoadStoreInstruction(Instruction* instruction, uint32_t dw, uint32_t kind22)
    {
        LoadStoreInstruction* loadStoreInstruction = reinterpret_cast<LoadStoreInstruction*>(instruction);
        loadStoreInstruction->kind = static_cast<Aarch64Instruction>(kind22);
        loadStoreInstruction->rt = Rt(dw);
        loadStoreInstruction->rn = Rn(dw);
        loadStoreInstruction->imm12 = imm12(dw) * 8;

        switch (loadStoreInstruction->kind)
        {
            case Aarch64Instruction::StrImmediateBaseOffset64:
            case Aarch64Instruction::LdrImmediateBaseOffset64:
                loadStoreInstruction->addressMode = AddressMode::BaseOffset;
                break;
            default:
                throw std::runtime_error("Unknown load store instruction.");
        }
    }


    void
    Aarch64AssemblyParser::parseLoadStorePairInstruction(Instruction* instruction, uint32_t dw, uint32_t kind22)
    {
        LoadStorePairInstruction* loadStoreInstruction = reinterpret_cast<LoadStorePairInstruction*>(instruction);
        loadStoreInstruction->kind = static_cast<Aarch64Instruction>(kind22);
        loadStoreInstruction->rt = Rt(dw);
        loadStoreInstruction->rn = Rn(dw);
        loadStoreInstruction->rt2 = Rt2(dw);
        loadStoreInstruction->imm7 = imm7(dw);

        switch (loadStoreInstruction->kind)
        {
            case Aarch64Instruction::StpPreIndex64:
                loadStoreInstruction->addressMode = AddressMode::PreIndex;
                break;
            case Aarch64Instruction::StrImmediateBaseOffset64:
            case Aarch64Instruction::StpBaseOffset64:
            case Aarch64Instruction::LdpBaseOffset64:
                loadStoreInstruction->addressMode = AddressMode::BaseOffset;
                break;
            case Aarch64Instruction::LdpPostIndex64:
                loadStoreInstruction->addressMode = AddressMode::PostIndex;
                break;
            default:
                throw std::runtime_error("Unknown load store pair instruction.");
        }
    }


    bool
    Aarch64AssemblyParser::tryParse21Instructions(Instruction* instruction, uint32_t dw)
    {
        uint32_t kind21 = dw & Mask21;

        switch (kind21)
        {
            case Aarch64Instruction::Movz:
                parseMovzInstruction(reinterpret_cast<MovzInstruction*>(instruction), dw);
                return true;
        }

        return false;
    }


    void
    Aarch64AssemblyParser::parseMovzInstruction(MovzInstruction* movz, uint32_t dw)
    {
        movz->kind = Aarch64Instruction::Movz;
        movz->is64Bit = sf(dw);
        movz->rd = Rd(dw);
        movz->imm16 = uimm16(dw);
        movz->immediateValue = static_cast<uint64_t>(uimm16(dw)) << getLeftShiftFromHv(dw);
    }


    void
    Aarch64AssemblyParser::parseMovnInstruction(MovnInstruction* movn, uint32_t dw)
    {
        movn->kind = Aarch64Instruction::Movn;
        movn->is64Bit = sf(dw);
        movn->rd = Rd(dw);
        movn->hw = static_cast<Hw>(hw(dw));
        movn->imm16 = uimm16(dw);
        uint16_t s = ~uimm16(dw);
        int64_t imm16 = static_cast<int64_t>(s);
        uint64_t lsh = getLeftShiftFromHv(dw);
        uint64_t result = (imm16 << lsh);
        if (imm16 >= 0)
        {
            // For some reason when lsh + 16 == 64. The left shift yields -1 instead of 0. Let's guard against that.
            if (lsh != 48)
            {
                result |= -1i64 << (lsh + 16);
            }
            result |= (1ui64 << lsh) - 1ui64;
        }
        movn->immediateValue = result;
    }


    void
    Aarch64AssemblyParser::parseMovkInstruction(MovkInstruction* movk, uint32_t dw)
    {
        movk->kind = Aarch64Instruction::Movk;
        movk->is64Bit = sf(dw);
        movk->rd = Rd(dw);
        movk->hw = static_cast<Hw>(hw(dw));
        movk->imm16 = uimm16(dw);
        movk->immediateValue = static_cast<uint64_t>(uimm16(dw));
    }


    uint32_t
    Aarch64AssemblyParser::getDoubleWord(Instruction* instruction)
    {
        uint32_t result = 0;
        for (int i = 0; i < 4; ++i)
        {
            uint8_t byte = getByte(instruction);
            result |= (byte << 8*i);
        }
        return result;
    }


    uint8_t
    Aarch64AssemblyParser::getByte(Instruction* instruction)
    {
        uint8_t result = (*_output)[_cursor++];
        instruction->bytes.add(result);
        return result;
    }


    Register
    Aarch64AssemblyParser::Rn(uint32_t dw)
    {
        return static_cast<Register>((dw & Aarch64Instruction::RnMask) >> 5);
    }


    Register
    Aarch64AssemblyParser::Rm(uint32_t dw)
    {
        return static_cast<Register>((dw & Aarch64Instruction::RmMask) >> 16);
    }


    Register
    Aarch64AssemblyParser::Rt(uint32_t dw)
    {
        return static_cast<Register>((dw & Aarch64Instruction::RtMask) >> 0);
    }


    Register
    Aarch64AssemblyParser::Rd(uint32_t dw)
    {
        return static_cast<Register>((dw & Aarch64Instruction::RdMask) >> 0);
    }


    Register
    Aarch64AssemblyParser::Rt2(uint32_t dw)
    {
        return static_cast<Register>((dw & Aarch64Instruction::Rt2Mask) >> 10);
    }


    int8_t
    Aarch64AssemblyParser::imm7(uint32_t dw)
    {
        return (((dw & Aarch64Instruction::Imm7Mask) >> 15) | 0x80) * 8;
    }


    int16_t
    Aarch64AssemblyParser::imm12(uint32_t dw)
    {
        return (dw & Aarch64Instruction::Imm12Mask) >> 10;
    }


    int32_t
    Aarch64AssemblyParser::imm26(uint32_t dw)
    {
        return (dw & Aarch64Instruction::Imm26Mask);
    }


    uint16_t
    Aarch64AssemblyParser::uimm16(uint32_t dw)
    {
        return (dw & Aarch64Instruction::Imm16Mask) >> 5;
    }


    void
    Aarch64AssemblyParser::parseAdrpInstruction(Instruction* instruction, uint32_t dw)
    {
        AdrInstruction* adrp = reinterpret_cast<AdrInstruction*>(instruction);
        adrp->kind = Aarch64Instruction::Adrp;
        adrp->rd = Rd(dw);
        adrp->immhilo = immhilo(dw);
    }



    void
    Aarch64AssemblyParser::parseAdrInstruction(Instruction* instruction, uint32_t dw)
    {
        AdrInstruction* adrp = reinterpret_cast<AdrInstruction*>(instruction);
        adrp->kind = Aarch64Instruction::Adr;
        adrp->rd = Rd(dw);
        adrp->immhilo = immhilo(dw);
    }


    uint32_t
    Aarch64AssemblyParser::immhilo(uint32_t dw)
    {
        uint32_t result = 0;
        result |= (dw & MASK(12, 5)) >> 3;
        result |= (dw & (0b11 << 29)) >> 29;
        return result;

    }


    void
    Aarch64AssemblyParser::parseUdfInstruction(Instruction* instruction, uint32_t dw)
    {
        UdfInstruction* udf = reinterpret_cast<UdfInstruction*>(instruction);
        udf->kind = Aarch64Instruction::Udf;
        udf->imm16 = dw & MASK(16, 0);
    }


    bool
    Aarch64AssemblyParser::sf(uint32_t dw)
    {
        return Aarch64Instruction::sf & dw;
    }


    uint32_t
    Aarch64AssemblyParser::hw(uint32_t dw)
    {
        return (Aarch64Instruction::HwMask & dw) >> 21;
    }

    uint8_t
    Aarch64AssemblyParser::imm6(uint32_t dw)
    {
        return (Aarch64Instruction::Imm6Mask & dw) >> 10;
    }


    uint8_t
    Aarch64AssemblyParser::shift(uint32_t dw)
    {
        return (Aarch64Instruction::ShiftMask & dw) >> 22;
    }


    uint64_t
    Aarch64AssemblyParser::getLeftShiftFromHv(uint32_t dw)
    {
        return (((0b11 << 21) & dw) >> 21) * 16;
    }
}