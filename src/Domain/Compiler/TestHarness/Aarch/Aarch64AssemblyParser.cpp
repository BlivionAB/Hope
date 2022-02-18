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
            uint32_t opc = RootInstruction::Op0_Mask & dw;
            switch (opc)
            {
                case static_cast<uint32_t>(RootInstruction::Op0_DataProcessing_Immediate_0):
                case static_cast<uint32_t>(RootInstruction::Op0_DataProcessing_Immediate_1):
                    if (parseDataProcessingImmediateInstruction(instruction, dw))
                    {
                        continue;
                    }
                    break;
                case static_cast<uint32_t>(RootInstruction::Op0_BranchingExceptionSystem_0):
                case static_cast<uint32_t>(RootInstruction::Op0_BranchingExceptionSystem_1):
                    if (parseBranchingExceptionSystemInstruction(instruction, dw))
                    {
                        continue;
                    }
                    break;
                case static_cast<uint32_t>(RootInstruction::Op0_LoadAndStore_0):
                case static_cast<uint32_t>(RootInstruction::Op0_LoadAndStore_1):
                case static_cast<uint32_t>(RootInstruction::Op0_LoadAndStore_2):
                case static_cast<uint32_t>(RootInstruction::Op0_LoadAndStore_3):
                    if (parseLoadAndStoreInstruction(instruction, dw))
                    {
                        continue;
                    }
                    break;
                case static_cast<uint32_t>(RootInstruction::Op0_DataProcessing_Register_0):
                case static_cast<uint32_t>(RootInstruction::Op0_DataProcessing_Register_1):
                    if (parseDataProcessingRegisterInstruction(instruction, dw))
                    {
                        continue;
                    }
                    break;
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
            if (tryParse15Instructions(instruction, dw))
            {
                continue;
            }
            if (tryParse10Instructions(instruction, dw))
            {
                continue;
            }
            if (Aarch64Instruction::Adr == (dw & Aarch64Instruction::AdrMask))
            {
                parseAdrInstruction(instruction, dw);
                continue;;
            }
            if (Aarch64Instruction::Adrp == (dw & Aarch64Instruction::AdrpMask))
            {
                parseAdrpInstruction(instruction, dw);
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
        BrInstruction* br = reinterpret_cast<BrInstruction*>(instruction);
        br->kind = Aarch64Instruction::Br;
        br->rn = Rn(dw);
    }


    void
    Aarch64AssemblyParser::parseBInstruction(Instruction* instruction, uint32_t dw)
    {
        BInstruction* b = reinterpret_cast<BInstruction*>(instruction);
        b->kind = B;
        b->imm26 = imm26(dw);
    }


    void
    Aarch64AssemblyParser::parseBlInstruction(Instruction* instruction, uint32_t dw)
    {
        BlInstruction* bl = reinterpret_cast<BlInstruction*>(instruction);
        bl->kind = Bl;
        bl->imm26 = imm26(dw);
    }


    void
    Aarch64AssemblyParser::parseRetInstruction(Instruction* instruction, uint32_t dw)
    {
        RetInstruction* ret = reinterpret_cast<RetInstruction*>(instruction);
        ret->kind = Ret;
        ret->rn = Rn(dw);
    }


    void
    Aarch64AssemblyParser::parseLdrLiteralInstruction(Instruction* instruction, uint32_t dw)
    {
        LdrInstruction* ldr = reinterpret_cast<LdrInstruction*>(instruction);
        ldr->kind = Ldr32;
        ldr->is64Bit = opc(dw) == Aarch64Instruction::Opc1;
        ldr->rt = Rt(dw);
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
            pattern = Bit::rotateRight(pattern, elementSize);
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
            case Aarch64Instruction::LdrImmediateUnsignedOffset64:
            case Aarch64Instruction::StrImmediateUnsignedOffset64:
                parseLdrStrImmediateUnsignedOffsetInstruction(reinterpret_cast<LdrStrImmediateUnsignedOffsetInstruction*>(instruction), dw, kind22);
                return true;
            case Aarch64Instruction::LdrbImmediateUnsignedOffset:
            case Aarch64Instruction::StrbImmediateUnsignedOffset:
                parseLdrbStrbImmediateUnsignedOffsetInstruction(reinterpret_cast<LdrbStrbImmediateUnsignedOffsetInstruction*>(instruction), dw, static_cast<Aarch64Instruction>(kind22));
                return true;
            case Aarch64Instruction::LdrsbImmediateUnsignedOffset:
                parseLdrsbImmediateUnsignedOffsetInstruction(reinterpret_cast<LdrsbImmediateUnsignedOffsetInstruction*>(instruction), dw);
                return true;
            case Aarch64Instruction::LdrshImmediateUnsignedOffset:
                parseLdrshImmediateUnsignedOffsetInstruction(reinterpret_cast<LdrshImmediateUnsignedOffsetInstruction*>(instruction), dw);
                return true;
            case Aarch64Instruction::LdrhImmediateUnsignedOffset:
            case Aarch64Instruction::StrhImmediateUnsignedOffset:
                parseLdrbStrbImmediateUnsignedOffsetInstruction(reinterpret_cast<LdrbStrbImmediateUnsignedOffsetInstruction*>(instruction), dw, static_cast<Aarch64Instruction>(kind22));
                return true;
            case Aarch64Instruction::AndImmediate:
                parseAndImmediateInstruction(reinterpret_cast<AndImmediateInstruction*>(instruction), dw);
                return true;
        }
        return false;
    }


    void
    Aarch64AssemblyParser::parseLdrStrImmediateUnsignedOffsetInstruction(LdrStrImmediateUnsignedOffsetInstruction* instruction, uint32_t dw, uint32_t kind22)
    {
        instruction->kind = static_cast<Aarch64Instruction>(kind22);
        instruction->is64Bit = dw & (0b01 << 30);
        instruction->Rn = Rn(dw);
        instruction->Rt = Rt(dw);
        instruction->imm12 = imm12(dw);
    }


    void
    Aarch64AssemblyParser::parseLdrbStrbImmediateUnsignedOffsetInstruction(LdrbStrbImmediateUnsignedOffsetInstruction* instruction, uint32_t dw, Aarch64Instruction kind)
    {
        instruction->kind = static_cast<Aarch64Instruction>(kind);
        instruction->Rn = Rn(dw);
        instruction->Rt = Rt(dw);
        instruction->imm12 = imm12(dw);
    }


    void
    Aarch64AssemblyParser::parseLdrsbImmediateUnsignedOffsetInstruction(LdrsbImmediateUnsignedOffsetInstruction* instruction, uint32_t dw)
    {
        instruction->kind = Aarch64Instruction::LdrsbImmediateUnsignedOffset;
        instruction->Rn = Rn(dw);
        instruction->Rt = Rt(dw);
        instruction->imm12 = imm12(dw);
    }


    void
    Aarch64AssemblyParser::parseLdrhStrhImmediateUnsignedOffsetInstruction(LdrhStrhImmediateUnsignedOffsetInstruction* instruction, uint32_t dw, Aarch64Instruction kind)
    {
        instruction->kind = static_cast<Aarch64Instruction>(kind);
        instruction->Rn = Rn(dw);
        instruction->Rt = Rt(dw);
        instruction->imm12 = imm12(dw);
    }


    void
    Aarch64AssemblyParser::parseLdrshImmediateUnsignedOffsetInstruction(LdrshImmediateUnsignedOffsetInstruction* instruction, uint32_t dw)
    {
        instruction->kind = Aarch64Instruction::LdrshImmediateUnsignedOffset;
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
//            case Aarch64Instruction::StrImmediateBaseOffset64:
//            case Aarch64Instruction::LdrImmediateBaseOffset64:
//                loadStoreInstruction->addressMode = AddressMode::BaseOffset;
//                break;
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
            case Aarch64Instruction::SubShiftedRegister:
            case Aarch64Instruction::SubShiftedRegister64:
                parseSubShiftedRegister(reinterpret_cast<SubShiftedRegisterInstruction*>(instruction), dw);
                return true;
        }

        return false;
    }


    bool
    Aarch64AssemblyParser::tryParse15Instructions(Instruction* instruction, uint32_t dw)
    {
        if ((Aarch64Instruction::Msub & dw) == Aarch64Instruction::Msub)
        {
            parseMaddSubInstruction(reinterpret_cast<MsubInstruction*>(instruction), dw, Aarch64Instruction::Msub);
            return true;
        }
        if ((Aarch64Instruction::Madd & dw) == Aarch64Instruction::Madd)
        {
            parseMaddSubInstruction(reinterpret_cast<MaddInstruction*>(instruction), dw, Aarch64Instruction::Madd);
            return true;
        }
        return false;
    }


    bool
    Aarch64AssemblyParser::tryParse10Instructions(Instruction* instruction, uint32_t dw)
    {
        uint32_t kind = dw & Mask10 & ~(((1 << 5) - 1) << 16);
        switch (kind)
        {
            case Aarch64Instruction::Sdiv:
            case Aarch64Instruction::Udiv:
                parseDivInstruction(reinterpret_cast<DivInstruction*>(instruction), dw, static_cast<Aarch64Instruction>(kind));
                return true;
            case Aarch64Instruction::Sxtb:
                parseSxtbInstruction(reinterpret_cast<SxtbInstruction*>(instruction), dw);
                return true;
            case Aarch64Instruction::Sxth:
                parseSxthInstruction(reinterpret_cast<SxthInstruction*>(instruction), dw);
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
    Aarch64AssemblyParser::Ra(uint32_t dw)
    {
        return static_cast<Register>((dw & Aarch64Instruction::RaMask) >> 10);
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
    Aarch64AssemblyParser::parseSubShiftedRegister(SubShiftedRegisterInstruction* instruction, uint32_t dw)
    {
        instruction->kind = Aarch64Instruction::SubShiftedRegister;
        instruction->is64Bit = sf(dw);
        instruction->Rd = Rd(dw);
        instruction->Rn = Rn(dw);
        instruction->Rm = Rm(dw);
        instruction->shift = shift(dw);
        instruction->imm6 = imm6(dw);
    }


    void
    Aarch64AssemblyParser::parseAddShiftedRegister(AddShiftedRegisterInstruction* instruction, uint32_t dw)
    {
        instruction->kind = Aarch64Instruction::AddShiftedRegister;
        instruction->is64Bit = sf(dw);
        instruction->Rd = Rd(dw);
        instruction->Rn = Rn(dw);
        instruction->Rm = Rm(dw);
        instruction->shift = shift(dw);
        instruction->imm6 = imm6(dw);
    }


    void
    Aarch64AssemblyParser::parseMaddSubInstruction(MaddSubInstruction* instruction, uint32_t dw, Aarch64Instruction kind)
    {
        instruction->kind = kind;
        instruction->is64Bit = sf(dw);
        instruction->Rm = Rm(dw);
        instruction->Ra = Ra(dw);
        instruction->Rn = Rn(dw);
        instruction->Rd = Rd(dw);
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
    Aarch64AssemblyParser::parseDivInstruction(DivInstruction* instruction, uint32_t dw, Aarch64Instruction kind)
    {
        instruction->kind = kind;
        instruction->is64Bit = sf(dw);
        instruction->Rm = Rm(dw);
        instruction->Rn = Rn(dw);
        instruction->Rd = Rd(dw);
    }


    void
    Aarch64AssemblyParser::parseUdfInstruction(Instruction* instruction, uint32_t dw)
    {
        UdfInstruction* udf = reinterpret_cast<UdfInstruction*>(instruction);
        udf->kind = Aarch64Instruction::Udf;
        udf->imm16 = dw & MASK(16, 0);
    }


    bool
    Aarch64AssemblyParser::op(uint32_t dw)
    {
        return Aarch64Instruction::op & dw;
    }


    bool
    Aarch64AssemblyParser::S(uint32_t dw)
    {
        return Aarch64Instruction::S & dw;
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


    void
    Aarch64AssemblyParser::parseAndImmediateInstruction(AndImmediateInstruction* instruction, uint32_t dw)
    {
        instruction->kind = Aarch64Instruction::AndImmediate;
        instruction->is64Bit = sf(dw);
        instruction->Rd = Rd(dw);
        instruction->Rn = Rn(dw);
        if (instruction->is64Bit)
        {
            instruction->value = decodeBitmaskImmediate(dw, RegistrySize::_64);
        }
        else
        {
            instruction->value = decodeBitmaskImmediate(dw, RegistrySize::_32);
        }
    }


    void
    Aarch64AssemblyParser::parseSxtbInstruction(SxtbInstruction* instruction, uint32_t dw)
    {
        instruction->kind = Aarch64Instruction::Sxtb;
        instruction->Rn = Rn(dw);
        instruction->Rd = Rd(dw);
    }


    void
    Aarch64AssemblyParser::parseSxthInstruction(SxthInstruction* instruction, uint32_t dw)
    {
        instruction->kind = Aarch64Instruction::Sxth;
        instruction->Rn = Rn(dw);
        instruction->Rd = Rd(dw);
    }


    bool
    Aarch64AssemblyParser::parseDataProcessingImmediateInstruction(Instruction* instruction, uint32_t dw)
    {
        return false;
    }


    bool
    Aarch64AssemblyParser::parseBranchingExceptionSystemInstruction(Instruction* instruction, uint32_t dw)
    {
        uint32_t op0 = BranchingExceptionSystem::Op0_Mask & dw;
        uint32_t op1 = BranchingExceptionSystem::Op1_Mask & dw;
        switch (op0)
        {
            case static_cast<uint32_t>(BranchingExceptionSystem::Op0_Grp6):
                if (BranchingExceptionSystem::Op1_UnconditionalBranch_Register & op1)
                {
                    if (parseUnconditionalBranchRegister(instruction, dw))
                    {
                        return true;
                    }
                }
                break;
            case static_cast<uint32_t>(BranchingExceptionSystem::Op0_GrpUnconditionalBranchImmediate_0):
            case static_cast<uint32_t>(BranchingExceptionSystem::Op0_GrpUnconditionalBranchImmediate_1):
                if (parseUnconditionalBranchImmediate(instruction, dw))
                {
                    return true;
                }
                break;

        }
        return false;
    }


    bool
    Aarch64AssemblyParser::parseUnconditionalBranchRegister(Instruction* instruction, uint32_t dw)
    {
        uint32_t op0 = UnconditionalBranch_Register::Opc_Mask & dw;
        uint32_t op2 = UnconditionalBranch_Register::Op2_Mask & dw;
        uint32_t op3 = UnconditionalBranch_Register::Op3_Mask & dw;
        uint32_t op4 = UnconditionalBranch_Register::Op4_Mask & dw;
        switch (op0)
        {
            case static_cast<uint32_t>(UnconditionalBranch_Register::Opc_Grp0):
                if (UnconditionalBranch_Register::Op2_True == op2)
                {
                    if (parseUnconditionalBranchOpcOp2Grp0(instruction, dw, op3, op4))
                    {
                        return true;
                    }
                }
                break;
            case static_cast<uint32_t>(UnconditionalBranch_Register::Opc_Grp2):
                if (UnconditionalBranch_Register::Op2_True == op2)
                {
                    if (UnconditionalBranch_Register::Op3_0 == op3 && UnconditionalBranch_Register::Op4_0 == op4)
                    {
                        parseRetInstruction(instruction, dw);
                        return true;
                    }
                }
        }
        return false;
    }


    bool
    Aarch64AssemblyParser::parseUnconditionalBranchOpcOp2Grp0(Instruction* instruction, uint32_t dw, uint32_t op3, uint32_t op4)
    {
        switch (op3)
        {
            case static_cast<uint32_t>(UnconditionalBranch_Register::Op3_0):
                if (UnconditionalBranch_Register::Op4_0 == op4)
                {
                    parseBrInstruction(instruction, dw);
                    return true;
                }
        }
        return false;
    }


    bool
    Aarch64AssemblyParser::parseUnconditionalBranchImmediate(Instruction* instruction, uint32_t dw)
    {
        uint32_t op0 = UnconditionalBranch_Immediate::Op0_Mask & dw;
        if (UnconditionalBranch_Immediate::Op0_B & op0)
        {
            parseBInstruction(instruction, dw);
        }
        else
        {
            parseBlInstruction(instruction, dw);
        }
        return true;
    }


    bool
    Aarch64AssemblyParser::parseLoadAndStoreInstruction(Instruction* instruction, uint32_t dw)
    {
        uint32_t op0 = LoadAndStoreInstruction::Op0_Mask & dw;
        uint32_t op1 = LoadAndStoreInstruction::Op1_Mask & dw;
        uint32_t op2 = LoadAndStoreInstruction::Op2_Mask & dw;
        uint32_t op3 = LoadAndStoreInstruction::Op3_Mask & dw;
        uint32_t op4 = LoadAndStoreInstruction::Op4_Mask & dw;
        switch (op0)
        {
            case static_cast<uint32_t>(LoadAndStoreInstruction::Op0_Grp1_0):
            case static_cast<uint32_t>(LoadAndStoreInstruction::Op0_Grp1_1):
            case static_cast<uint32_t>(LoadAndStoreInstruction::Op0_Grp1_2):
            case static_cast<uint32_t>(LoadAndStoreInstruction::Op0_Grp1_3):
                if (op2 == LoadAndStoreInstruction::Op2_0 || op2 == LoadAndStoreInstruction::Op2_1)
                {
                    parseLdrLiteralInstruction(instruction, dw);
                    return true;
                }
        }
        return false;
    }


    bool
    Aarch64AssemblyParser::parseDataProcessingRegisterInstruction(Instruction* instruction, uint32_t dw)
    {
        uint32_t op0 = DataProcessingRegisterInstruction::Op0_Mask & dw;
        uint32_t op1 = DataProcessingRegisterInstruction::Op1_Mask & dw;
        uint32_t op2 = DataProcessingRegisterInstruction::Op2_Mask & dw;
        uint32_t op3 = DataProcessingRegisterInstruction::Op3_Mask & dw;
        if (op1 == DataProcessingRegisterInstruction::Op1_0)
        {
            switch (op2)
            {
                case static_cast<uint32_t>(DataProcessingRegisterInstruction::Op2_1xx0_0):
                case static_cast<uint32_t>(DataProcessingRegisterInstruction::Op2_1xx0_1):
                case static_cast<uint32_t>(DataProcessingRegisterInstruction::Op2_1xx0_2):
                case static_cast<uint32_t>(DataProcessingRegisterInstruction::Op2_1xx0_3):
                    if (parseAddSubtractShiftedRegister(instruction, dw))
                    {
                        return true;
                    }
                    break;
            }
        }
        return false;
    }


    bool
    Aarch64AssemblyParser::parseAddSubtractShiftedRegister(Instruction* instruction, uint32_t dw)
    {
        uint32_t sf_op_S = AddSubtractShiftedRegisterInstruction::sf_op_S_Mask & dw;
        switch (sf_op_S)
        {
            case static_cast<uint32_t>(AddSubtractShiftedRegisterInstruction::sf_op_S_Add64):
            case static_cast<uint32_t>(AddSubtractShiftedRegisterInstruction::sf_op_S_Add32):
                parseAddShiftedRegister(reinterpret_cast<AddShiftedRegisterInstruction*>(instruction), dw);
                return true;
        }
        return false;
    }


    uint32_t
    Aarch64AssemblyParser::opc(uint32_t dw)
    {
        return Aarch64Instruction::OpcMask & dw;
    }
}