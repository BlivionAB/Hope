#include "Aarch64Parser.h"
#include "Aarch64Instructions.h"
#include <Domain/Compiler/Instruction/Assembly/Aarch/AArch64Encodings.h>
#include <Foundation/Bit.h>

namespace elet::domain::compiler::test::aarch
{
    using namespace elet::foundation;
    using namespace elet::domain::compiler::instruction::output;


    void
    Aarch64Parser::parse(List<OneOfInstruction>& instructions, List<uint8_t>& output, size_t offset, size_t size)
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
                case static_cast<uint32_t>(RootInstruction::Reserved):
                    if (parseReservedInstruction(instruction, dw))
                    {
                        continue;
                    }
                    break;
                case static_cast<uint32_t>(RootInstruction::DataProcessing_Immediate_0):
                case static_cast<uint32_t>(RootInstruction::DataProcessing_Immediate_1):
                    if (parseDataProcessingImmediateInstruction(instruction, dw))
                    {
                        continue;
                    }
                    break;
                case static_cast<uint32_t>(RootInstruction::BranchingExceptionSystem_0):
                case static_cast<uint32_t>(RootInstruction::BranchingExceptionSystem_1):
                    if (parseBranchingExceptionSystemInstruction(instruction, dw))
                    {
                        continue;
                    }
                    break;
                case static_cast<uint32_t>(RootInstruction::LoadAndStore_0):
                case static_cast<uint32_t>(RootInstruction::LoadAndStore_1):
                case static_cast<uint32_t>(RootInstruction::LoadAndStore_2):
                case static_cast<uint32_t>(RootInstruction::LoadAndStore_3):
                    if (parseLoadAndStoreInstruction(instruction, dw))
                    {
                        continue;
                    }
                    break;
                case static_cast<uint32_t>(RootInstruction::DataProcessing_Register_0):
                case static_cast<uint32_t>(RootInstruction::DataProcessing_Register_1):
                    if (parseDataProcessingRegisterInstruction(instruction, dw))
                    {
                        continue;
                    }
                    break;
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
    Aarch64Parser::parseBrInstruction(Instruction* instruction, uint32_t dw)
    {
        BrInstruction* br = reinterpret_cast<BrInstruction*>(instruction);
        br->kind = Aarch64Instruction::Br;
        br->rn = Rn(dw);
    }


    void
    Aarch64Parser::parseBInstruction(Instruction* instruction, uint32_t dw)
    {
        BInstruction* b = reinterpret_cast<BInstruction*>(instruction);
        b->kind = B;
        b->imm26 = imm26(dw);
    }


    void
    Aarch64Parser::parseBlInstruction(Instruction* instruction, uint32_t dw)
    {
        BlInstruction* bl = reinterpret_cast<BlInstruction*>(instruction);
        bl->kind = Bl;
        bl->imm26 = imm26(dw);
    }


    void
    Aarch64Parser::parseRetInstruction(Instruction* instruction, uint32_t dw)
    {
        RetInstruction* ret = reinterpret_cast<RetInstruction*>(instruction);
        ret->kind = Ret;
        ret->rn = Rn(dw);
    }


    void
    Aarch64Parser::parseLdrLiteralInstruction(Instruction* instruction, uint32_t dw)
    {
        LdrInstruction* ldr = reinterpret_cast<LdrInstruction*>(instruction);
        ldr->kind = Ldr32;
        ldr->is64Bit = opc(dw) == Aarch64Instruction::Opc1;
        ldr->rt = Rt(dw);
    }


    void
    Aarch64Parser::parseOrrInstruction(OrrImmediateInstruction* instruction, uint32_t dw)
    {
        instruction->kind = Aarch64Instruction::OrrImmediate;
        instruction->is64Bit = sf(dw);
        instruction->rd = Rd(dw);
        instruction->rn = Rn(dw);
        if (sf(dw))
        {
            instruction->immediateValue = decodeBitmaskImmediate(dw, RegistrySize::_64);
        }
        else
        {
            instruction->immediateValue = decodeBitmaskImmediate(dw, RegistrySize::_32);
        }
    }


    uint64_t
    Aarch64Parser::decodeBitmaskImmediate(uint32_t dw, RegistrySize registrySize)
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


    void
    Aarch64Parser::parseLdrStrImmediateUnsignedOffsetInstruction(LdrStrImmediateUnsignedOffsetInstruction* instruction, uint32_t dw, uint32_t kind22)
    {
        instruction->kind = static_cast<Aarch64Instruction>(kind22);
        instruction->is64Bit = dw & (0b01 << 30);
        instruction->Rn = Rn(dw);
        instruction->Rt = Rt(dw);
        instruction->imm12 = imm12(dw);
    }


    void
    Aarch64Parser::parseLdrbStrbImmediateUnsignedOffsetInstruction(LdrbStrbImmediateUnsignedOffsetInstruction* instruction, uint32_t dw, Aarch64Instruction kind)
    {
        assert(kind == Aarch64Instruction::StrbImmediateUnsignedOffset || kind == Aarch64Instruction::LdrbImmediateUnsignedOffset && "Must be Strb or Ldrb");
        instruction->kind = static_cast<Aarch64Instruction>(kind);
        instruction->Rn = Rn(dw);
        instruction->Rt = Rt(dw);
        instruction->imm12 = imm12(dw);
    }


    void
    Aarch64Parser::parseLdrsbImmediateUnsignedOffsetInstruction(LdrsbImmediateUnsignedOffsetInstruction* instruction, uint32_t dw)
    {
        instruction->kind = Aarch64Instruction::LdrsbImmediateUnsignedOffset;
        instruction->Rn = Rn(dw);
        instruction->Rt = Rt(dw);
        instruction->imm12 = imm12(dw);
    }


    void
    Aarch64Parser::parseLdrhStrhImmediateUnsignedOffsetInstruction(LdrhStrhImmediateUnsignedOffsetInstruction* instruction, uint32_t dw, Aarch64Instruction kind)
    {
        instruction->kind = static_cast<Aarch64Instruction>(kind);
        instruction->Rn = Rn(dw);
        instruction->Rt = Rt(dw);
        instruction->imm12 = imm12(dw);
    }


    void
    Aarch64Parser::parseLdrshImmediateUnsignedOffsetInstruction(LdrshImmediateUnsignedOffsetInstruction* instruction, uint32_t dw)
    {
        instruction->kind = Aarch64Instruction::LdrshImmediateUnsignedOffset;
        instruction->Rn = Rn(dw);
        instruction->Rt = Rt(dw);
        instruction->imm12 = imm12(dw);
    }


    void
    Aarch64Parser::parseLoadStorePairInstruction(Instruction* instruction, uint32_t dw, Aarch64Instruction kind)
    {
        assert(kind == Aarch64Instruction::StpOffset64 ||
            kind == Aarch64Instruction::LdpOffset64 ||
            kind == Aarch64Instruction::LdpPostIndex64 ||
            kind == Aarch64Instruction::StpPreIndex64 && "Unknown kind");

        LoadStorePairInstruction* loadStoreInstruction = reinterpret_cast<LoadStorePairInstruction*>(instruction);
        loadStoreInstruction->kind = kind;
        loadStoreInstruction->rt = Rt(dw);
        loadStoreInstruction->rn = Rn(dw);
        loadStoreInstruction->rt2 = Rt2(dw);
        loadStoreInstruction->imm7 = imm7(dw);

        switch (loadStoreInstruction->kind)
        {
            case Aarch64Instruction::StpPreIndex64:
                loadStoreInstruction->is64Bit = true;
                loadStoreInstruction->addressMode = AddressMode::PreIndex;
                break;
            case Aarch64Instruction::StpOffset64:
            case Aarch64Instruction::LdpOffset64:
                loadStoreInstruction->is64Bit = true;
                loadStoreInstruction->addressMode = AddressMode::BaseOffset;
                break;
            case Aarch64Instruction::LdpPostIndex64:
                loadStoreInstruction->is64Bit = true;
                loadStoreInstruction->addressMode = AddressMode::PostIndex;
                break;
            default:
                throw std::runtime_error("Unknown load store pair instruction.");
        }
    }


    void
    Aarch64Parser::parseMovzInstruction(MovzInstruction* movz, uint32_t dw)
    {
        movz->kind = Aarch64Instruction::Movz;
        movz->is64Bit = sf(dw);
        movz->rd = Rd(dw);
        movz->imm16 = uimm16(dw);
        movz->immediateValue = static_cast<uint64_t>(uimm16(dw)) << getLeftShiftFromHv(dw);
    }


    void
    Aarch64Parser::parseMovnInstruction(MovnInstruction* movn, uint32_t dw)
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
    Aarch64Parser::parseMovkInstruction(MovkInstruction* movk, uint32_t dw)
    {
        movk->kind = Aarch64Instruction::Movk;
        movk->is64Bit = sf(dw);
        movk->rd = Rd(dw);
        movk->hw = static_cast<Hw>(hw(dw));
        movk->imm16 = uimm16(dw);
        movk->immediateValue = static_cast<uint64_t>(uimm16(dw));
    }


    uint32_t
    Aarch64Parser::getDoubleWord(Instruction* instruction)
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
    Aarch64Parser::getByte(Instruction* instruction)
    {
        uint8_t result = (*_output)[_cursor++];
        instruction->bytes.add(result);
        return result;
    }


    Register
    Aarch64Parser::Ra(uint32_t dw)
    {
        return static_cast<Register>((dw & Aarch64Instruction::RaMask) >> 10);
    }


    Register
    Aarch64Parser::Rn(uint32_t dw)
    {
        return static_cast<Register>((dw & Aarch64Instruction::RnMask) >> 5);
    }


    Register
    Aarch64Parser::Rm(uint32_t dw)
    {
        return static_cast<Register>((dw & Aarch64Instruction::RmMask) >> 16);
    }


    Register
    Aarch64Parser::Rt(uint32_t dw)
    {
        return static_cast<Register>((dw & Aarch64Instruction::RtMask) >> 0);
    }


    Register
    Aarch64Parser::Rd(uint32_t dw)
    {
        return static_cast<Register>((dw & Aarch64Instruction::RdMask) >> 0);
    }


    Register
    Aarch64Parser::Rt2(uint32_t dw)
    {
        return static_cast<Register>((dw & Aarch64Instruction::Rt2Mask) >> 10);
    }


    int8_t
    Aarch64Parser::imm7(uint32_t dw)
    {
        return (((dw & Aarch64Instruction::Imm7Mask) >> 15) | 0x80) * 8;
    }


    int16_t
    Aarch64Parser::imm12(uint32_t dw)
    {
        return (dw & Aarch64Instruction::Imm12Mask) >> 10;
    }


    int32_t
    Aarch64Parser::imm26(uint32_t dw)
    {
        return (dw & Aarch64Instruction::Imm26Mask);
    }


    uint16_t
    Aarch64Parser::uimm16(uint32_t dw)
    {
        return (dw & Aarch64Instruction::Imm16Mask) >> 5;
    }


    void
    Aarch64Parser::parseSubShiftedRegister(SubShiftedRegisterInstruction* instruction, uint32_t dw)
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
    Aarch64Parser::parseAddShiftedRegister(AddShiftedRegisterInstruction* instruction, uint32_t dw)
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
    Aarch64Parser::parseMaddSubInstruction(MaddSubInstruction* instruction, uint32_t dw, Aarch64Instruction kind)
    {
        instruction->kind = kind;
        instruction->is64Bit = sf(dw);
        instruction->Rm = Rm(dw);
        instruction->Ra = Ra(dw);
        instruction->Rn = Rn(dw);
        instruction->Rd = Rd(dw);
    }


    void
    Aarch64Parser::parseAdrpInstruction(Instruction* instruction, uint32_t dw)
    {
        AdrInstruction* adrp = reinterpret_cast<AdrInstruction*>(instruction);
        adrp->kind = Aarch64Instruction::Adrp;
        adrp->rd = Rd(dw);
        adrp->immhilo = immhilo(dw);
    }



    void
    Aarch64Parser::parseAdrInstruction(Instruction* instruction, uint32_t dw)
    {
        AdrInstruction* adrp = reinterpret_cast<AdrInstruction*>(instruction);
        adrp->kind = Aarch64Instruction::Adr;
        adrp->rd = Rd(dw);
        adrp->immhilo = immhilo(dw);
    }


    uint32_t
    Aarch64Parser::immhilo(uint32_t dw)
    {
        uint32_t result = 0;
        result |= (dw & MASK(12, 5)) >> 3;
        result |= (dw & (0b11 << 29)) >> 29;
        return result;

    }


    void
    Aarch64Parser::parseDivInstruction(DivInstruction* instruction, uint32_t dw, Aarch64Instruction kind)
    {
        instruction->kind = kind;
        instruction->is64Bit = sf(dw);
        instruction->Rm = Rm(dw);
        instruction->Rn = Rn(dw);
        instruction->Rd = Rd(dw);
    }


    void
    Aarch64Parser::parseUdfInstruction(Instruction* instruction, uint32_t dw)
    {
        UdfInstruction* udf = reinterpret_cast<UdfInstruction*>(instruction);
        udf->kind = Aarch64Instruction::Udf;
        udf->imm16 = dw & MASK(16, 0);
    }


    bool
    Aarch64Parser::sf(uint32_t dw)
    {
        return Aarch64Instruction::sf & dw;
    }


    bool
    Aarch64Parser::S(uint32_t dw)
    {
        return Aarch64Instruction::S & dw >> 29;
    }


    uint32_t
    Aarch64Parser::opc(uint32_t dw)
    {
        return Aarch64Instruction::OpcMask & dw;
    }


    uint32_t
    Aarch64Parser::hw(uint32_t dw)
    {
        return (Aarch64Instruction::HwMask & dw) >> 21;
    }


    uint8_t
    Aarch64Parser::imm6(uint32_t dw)
    {
        return (Aarch64Instruction::Imm6Mask & dw) >> 10;
    }


    uint8_t
    Aarch64Parser::shift(uint32_t dw)
    {
        return (Aarch64Instruction::ShiftMask & dw) >> 22;
    }


    uint64_t
    Aarch64Parser::getLeftShiftFromHv(uint32_t dw)
    {
        return (((0b11 << 21) & dw) >> 21) * 16;
    }


    void
    Aarch64Parser::parseAndImmediateInstruction(AndImmediateInstruction* instruction, uint32_t dw)
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
    Aarch64Parser::parseSxtbInstruction(SxtbInstruction* instruction, uint32_t dw)
    {
        instruction->kind = Aarch64Instruction::Sxtb;
        instruction->Rn = Rn(dw);
        instruction->Rd = Rd(dw);
    }


    void
    Aarch64Parser::parseSxthInstruction(SxthInstruction* instruction, uint32_t dw)
    {
        instruction->kind = Aarch64Instruction::Sxth;
        instruction->Rn = Rn(dw);
        instruction->Rd = Rd(dw);
    }


    bool
    Aarch64Parser::parseDataProcessingImmediateInstruction(Instruction* instruction, uint32_t dw)
    {
        uint32_t op0 = DataProcessingImmediateEncoding::Op0_Mask & dw;
        switch (op0)
        {
            case static_cast<uint32_t>(DataProcessingImmediateEncoding::PcRelAddressing_0):
            case static_cast<uint32_t>(DataProcessingImmediateEncoding::PcRelAddressing_1):
                if (parsePcRelAddressingInstruction(instruction, dw))
                {
                    return true;
                }
                break;
            case static_cast<uint32_t>(DataProcessingImmediateEncoding::AddSubtractImmediate32):
                if (parseAddSubtractImmediateInstruction(instruction, dw))
                {
                    return true;
                }
                break;
            case static_cast<uint32_t>(DataProcessingImmediateEncoding::LogicalImmediate):
                if (parseLogicalImmediateInstruction(instruction, dw))
                {
                    return true;
                }
                break;
            case static_cast<uint32_t>(DataProcessingImmediateEncoding::MoveWideImmediate):
                if (parseMoveWideImmediateInstruction(instruction, dw))
                {
                    return true;
                }
                break;
            case static_cast<uint32_t>(DataProcessingImmediateEncoding::Bitfield):
                if (parseBitfieldInstruction(instruction, dw))
                {
                    return true;
                }
                break;
        }
        return false;
    }


    bool
    Aarch64Parser::parseBranchingExceptionSystemInstruction(Instruction* instruction, uint32_t dw)
    {
        uint32_t op0 = BranchingExceptionSystemEncoding::Op0_Mask & dw;
        uint32_t op1 = BranchingExceptionSystemEncoding::Op1_Mask & dw;
        switch (op0)
        {
            case static_cast<uint32_t>(BranchingExceptionSystemEncoding::Op0_Grp6):
                if (BranchingExceptionSystemEncoding::Op1_UnconditionalBranch_Register & op1)
                {
                    if (parseUnconditionalBranchRegister(instruction, dw))
                    {
                        return true;
                    }
                }
                break;
            case static_cast<uint32_t>(BranchingExceptionSystemEncoding::Op0_GrpUnconditionalBranchImmediate_0):
            case static_cast<uint32_t>(BranchingExceptionSystemEncoding::Op0_GrpUnconditionalBranchImmediate_1):
                if (parseUnconditionalBranchImmediate(instruction, dw))
                {
                    return true;
                }
                break;

        }
        return false;
    }


    bool
    Aarch64Parser::parseUnconditionalBranchRegister(Instruction* instruction, uint32_t dw)
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
    Aarch64Parser::parseUnconditionalBranchOpcOp2Grp0(Instruction* instruction, uint32_t dw, uint32_t op3, uint32_t op4)
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
    Aarch64Parser::parseUnconditionalBranchImmediate(Instruction* instruction, uint32_t dw)
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
    Aarch64Parser::parseLoadAndStoreInstruction(Instruction* instruction, uint32_t dw)
    {
        uint32_t op0 = LoadAndStoreEncoding::Op0_Mask & dw;
        uint32_t op1 = LoadAndStoreEncoding::Op1_Mask & dw;
        uint32_t op2 = LoadAndStoreEncoding::Op2_Mask & dw;
        uint32_t op3 = LoadAndStoreEncoding::Op3_Mask & dw;
        uint32_t op4 = LoadAndStoreEncoding::Op4_Mask & dw;
        switch (op0)
        {
            case static_cast<uint32_t>(LoadAndStoreEncoding::Op0_xx01_0):
            case static_cast<uint32_t>(LoadAndStoreEncoding::Op0_xx01_1):
            case static_cast<uint32_t>(LoadAndStoreEncoding::Op0_xx01_2):
            case static_cast<uint32_t>(LoadAndStoreEncoding::Op0_xx01_3):
                if (op2 == LoadAndStoreEncoding::Op2_0 || op2 == LoadAndStoreEncoding::Op2_1)
                {
                    parseLdrLiteralInstruction(instruction, dw);
                    return true;
                }
                break;
            case static_cast<uint32_t>(LoadAndStoreEncoding::Op0_xx10_0):
            case static_cast<uint32_t>(LoadAndStoreEncoding::Op0_xx10_1):
            case static_cast<uint32_t>(LoadAndStoreEncoding::Op0_xx10_2):
            case static_cast<uint32_t>(LoadAndStoreEncoding::Op0_xx10_3):
                if (parse_Op0_xx10(op2, instruction, dw))
                {
                    return true;
                }
                break;
            case static_cast<uint32_t>(LoadAndStoreEncoding::Op0_xx11_0):
            case static_cast<uint32_t>(LoadAndStoreEncoding::Op0_xx11_1):
            case static_cast<uint32_t>(LoadAndStoreEncoding::Op0_xx11_2):
            case static_cast<uint32_t>(LoadAndStoreEncoding::Op0_xx11_3):
                if (op2 == LoadAndStoreEncoding::Op2_2 || op2 == LoadAndStoreEncoding::Op2_3)
                {
                    if (parseLoadStore_UnsignedImmediate(instruction, dw))
                    {
                        return true;
                    }
                }
                break;
        }
        return false;
    }


    bool
    Aarch64Parser::parse_Op0_xx10(uint32_t op2, Instruction* instruction, uint32_t dw)
    {
        switch (op2)
        {
            case static_cast<uint32_t>(LoadAndStoreEncoding::Op2_1):
                if (parseLoadStorePair_PostIndexed_Instruction(instruction, dw))
                {
                    return true;
                }
                break;
            case static_cast<uint32_t>(LoadAndStoreEncoding::Op2_2):
                if (parseLoadStorePair_Offset_Instruction(instruction, dw))
                {
                    return true;
                }
                break;
            case static_cast<uint32_t>(LoadAndStoreEncoding::Op2_3):
                if (parseLoadStorePair_PreIndexed_Instruction(instruction, dw))
                {
                    return true;
                }
                break;
        }
        return false;
    }


    bool
    Aarch64Parser::parseDataProcessingRegisterInstruction(Instruction* instruction, uint32_t dw)
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
        else
        {
            switch (op2)
            {
                case static_cast<uint32_t>(DataProcessingRegisterInstruction::Op2_1xxx_0):
                case static_cast<uint32_t>(DataProcessingRegisterInstruction::Op2_1xxx_1):
                case static_cast<uint32_t>(DataProcessingRegisterInstruction::Op2_1xxx_2):
                case static_cast<uint32_t>(DataProcessingRegisterInstruction::Op2_1xxx_3):
                case static_cast<uint32_t>(DataProcessingRegisterInstruction::Op2_1xxx_4):
                case static_cast<uint32_t>(DataProcessingRegisterInstruction::Op2_1xxx_5):
                case static_cast<uint32_t>(DataProcessingRegisterInstruction::Op2_1xxx_6):
                case static_cast<uint32_t>(DataProcessingRegisterInstruction::Op2_1xxx_7):
                    if (parseDataProcessing3Source(instruction, dw))
                    {
                        return true;
                    }
                    break;
                default:
                    if (op0 == DataProcessingRegisterInstruction::Op0_0)
                    {
                        if (op2 == DataProcessingRegisterInstruction::Op2_0110)
                        {
                            if (parseDataProcessing2Source(instruction, dw))
                            {
                                return true;
                            }
                        }
                    }
            }
        }
        return false;
    }


    bool
    Aarch64Parser::parseAddSubtractShiftedRegister(Instruction* instruction, uint32_t dw)
    {
        uint32_t sf_op_S = AddSubtractShiftedRegisterInstruction::sf_op_S_Mask & dw;
        switch (sf_op_S)
        {
            case static_cast<uint32_t>(AddSubtractShiftedRegisterInstruction::Add32):
            case static_cast<uint32_t>(AddSubtractShiftedRegisterInstruction::Add64):
                parseAddShiftedRegister(reinterpret_cast<AddShiftedRegisterInstruction*>(instruction), dw);
                return true;
            case static_cast<uint32_t>(AddSubtractShiftedRegisterInstruction::Sub32):
            case static_cast<uint32_t>(AddSubtractShiftedRegisterInstruction::Sub64):
                parseSubShiftedRegister(reinterpret_cast<SubShiftedRegisterInstruction*>(instruction), dw);
                return true;
        }
        return false;
    }


    bool
    Aarch64Parser::parseLogicalImmediateInstruction(Instruction* instruction, uint32_t dw)
    {
        uint32_t sf_opc = LogicalImmediateEncoding::sf_opc_Mask & dw;
        switch (sf_opc)
        {
            case static_cast<uint32_t>(LogicalImmediateEncoding::AndImmediate32):
            case static_cast<uint32_t>(LogicalImmediateEncoding::AndImmediate64):
                parseAndImmediateInstruction(reinterpret_cast<AndImmediateInstruction*>(instruction), dw);
                return true;
            case static_cast<uint32_t>(LogicalImmediateEncoding::OrrImmediate32):
            case static_cast<uint32_t>(LogicalImmediateEncoding::OrrImmediate64):
                parseOrrInstruction(reinterpret_cast<OrrImmediateInstruction*>(instruction), dw);
                return true;
        }
        return false;
    }


    bool
    Aarch64Parser::parseMoveWideImmediateInstruction(Instruction* instruction, uint32_t dw)
    {
        uint32_t sf_opc = MoveWideImmediateEncoding::sf_opc_Mask & dw;
        switch (sf_opc)
        {
            case static_cast<uint32_t>(MoveWideImmediateEncoding::Movn32):
            case static_cast<uint32_t>(MoveWideImmediateEncoding::Movn64):
                parseMovnInstruction(reinterpret_cast<MovnInstruction*>(instruction), dw);
                return true;
            case static_cast<uint32_t>(MoveWideImmediateEncoding::Movz32):
            case static_cast<uint32_t>(MoveWideImmediateEncoding::Movz64):
                parseMovzInstruction(reinterpret_cast<MovzInstruction*>(instruction), dw);
                return true;
            case static_cast<uint32_t>(MoveWideImmediateEncoding::Movk32):
            case static_cast<uint32_t>(MoveWideImmediateEncoding::Movk64):
                parseMovkInstruction(reinterpret_cast<MovkInstruction*>(instruction), dw);
                return true;
        }
        return false;
    }


    bool
    Aarch64Parser::parseLoadStorePair_PostIndexed_Instruction(Instruction* instruction, uint32_t dw)
    {
        uint32_t opc_V_L = LoadStoreEncoding::opc_V_L_Mask & dw;
        switch (opc_V_L)
        {
            case static_cast<uint32_t>(LoadStoreEncoding::Ldp64):
                parseLoadStorePairInstruction(instruction, dw, Aarch64Instruction::LdpPostIndex64);
                return true;
        }
        return false;
    }


    bool
    Aarch64Parser::parseLoadStorePair_Offset_Instruction(Instruction* instruction, uint32_t dw)
    {
        uint32_t opc_V_L = LoadStoreEncoding::opc_V_L_Mask & dw;
        switch (opc_V_L)
        {
            case static_cast<uint32_t>(LoadStoreEncoding::Stp64):
                parseLoadStorePairInstruction(instruction, dw, Aarch64Instruction::StpOffset64);
                return true;
            case static_cast<uint32_t>(LoadStoreEncoding::Ldp64):
                parseLoadStorePairInstruction(instruction, dw, Aarch64Instruction::LdpOffset64);
                return true;
        }
        return false;
    }


    bool
    Aarch64Parser::parseLoadStorePair_PreIndexed_Instruction(Instruction* instruction, uint32_t dw)
    {
        uint32_t opc_V_L = LoadStoreEncoding::opc_V_L_Mask & dw;
        switch (opc_V_L)
        {
            case static_cast<uint32_t>(LoadStoreEncoding::Stp64):
                parseLoadStorePairInstruction(instruction, dw, Aarch64Instruction::StpPreIndex64);
                return true;
        }
        return false;
    }


    bool
    Aarch64Parser::parseAddSubtractImmediateInstruction(Instruction* instruction, uint32_t dw)
    {
        uint32_t sf_op_S = AddSubtract_Immediate_Encoding::sf_op_S_Mask & dw;
        switch (sf_op_S)
        {
            case static_cast<uint32_t>(AddSubtract_Immediate_Encoding::Add_Immediate32):
                parseAddSubtractImmediateInstruction(instruction, dw, Aarch64Instruction::AddImmediate32);
                return true;
            case static_cast<uint32_t>(AddSubtract_Immediate_Encoding::Sub_Immediate32):
                parseAddSubtractImmediateInstruction(instruction, dw, Aarch64Instruction::SubImmediate32);
                return true;
            case static_cast<uint32_t>(AddSubtract_Immediate_Encoding::Add_Immediate64):
                parseAddSubtractImmediateInstruction(instruction, dw, Aarch64Instruction::AddImmediate64);
                return true;
            case static_cast<uint32_t>(AddSubtract_Immediate_Encoding::Sub_Immediate64):
                parseAddSubtractImmediateInstruction(instruction, dw, Aarch64Instruction::SubImmediate64);
                return true;
        }
        return false;
    }


    void
    Aarch64Parser::parseAddSubtractImmediateInstruction(Instruction* instruction, uint32_t dw, Aarch64Instruction kind)
    {
        AddSubImmediateInstruction* _instruction = reinterpret_cast<AddSubImmediateInstruction*>(instruction);
        _instruction->kind = kind;
        _instruction->is64Bit = sf(dw);
        _instruction->rd = Rd(dw);
        _instruction->rn = Rn(dw);
        _instruction->imm12 = imm12(dw);
    }


    bool
    Aarch64Parser::parseLoadStore_UnsignedImmediate(Instruction* instruction, uint32_t dw)
    {
        uint32_t size_V_opc = LoadStoreRegister_UnsignedImmediate::size_V_opc_Mask & dw;
        switch (size_V_opc)
        {
            case static_cast<uint32_t>(LoadStoreRegister_UnsignedImmediate::Strb_Immediate):
                parseLdrbStrbImmediateUnsignedOffsetInstruction(reinterpret_cast<LdrbStrbImmediateUnsignedOffsetInstruction*>(instruction), dw, Aarch64Instruction::StrbImmediateUnsignedOffset);
                return true;
            case static_cast<uint32_t>(LoadStoreRegister_UnsignedImmediate::Ldrb_Immediate):
                parseLdrbStrbImmediateUnsignedOffsetInstruction(reinterpret_cast<LdrbStrbImmediateUnsignedOffsetInstruction*>(instruction), dw, Aarch64Instruction::LdrbImmediateUnsignedOffset );
                return true;
            case static_cast<uint32_t>(LoadStoreRegister_UnsignedImmediate::Ldrsb_Immediate32):
                parseLdrsbImmediateUnsignedOffsetInstruction(reinterpret_cast<LdrsbImmediateUnsignedOffsetInstruction*>(instruction), dw);
                return true;
            case static_cast<uint32_t>(LoadStoreRegister_UnsignedImmediate::Ldrsh_Immediate32):
                parseLdrshImmediateUnsignedOffsetInstruction(reinterpret_cast<LdrshImmediateUnsignedOffsetInstruction*>(instruction), dw);
                return true;
            case static_cast<uint32_t>(LoadStoreRegister_UnsignedImmediate::Ldr_Immediate_32):
            case static_cast<uint32_t>(LoadStoreRegister_UnsignedImmediate::Ldr_Immediate_64):
                parseLdrStrImmediateUnsignedOffsetInstruction(reinterpret_cast<LdrStrImmediateUnsignedOffsetInstruction*>(instruction), dw, Aarch64Instruction::LdrImmediateUnsignedOffset);
                return true;
            case static_cast<uint32_t>(LoadStoreRegister_UnsignedImmediate::Str_Immediate_32):
            case static_cast<uint32_t>(LoadStoreRegister_UnsignedImmediate::Str_Immediate_64):
                parseLdrStrImmediateUnsignedOffsetInstruction(reinterpret_cast<LdrStrImmediateUnsignedOffsetInstruction*>(instruction), dw, Aarch64Instruction::StrImmediateUnsignedOffset);
                return true;
            case static_cast<uint32_t>(LoadStoreRegister_UnsignedImmediate::Strh_Immediate):
                parseLdrhStrhImmediateUnsignedOffsetInstruction(reinterpret_cast<LdrhStrhImmediateUnsignedOffsetInstruction*>(instruction), dw, Aarch64Instruction::StrhImmediateUnsignedOffset);
                return true;
            case static_cast<uint32_t>(LoadStoreRegister_UnsignedImmediate::Ldrh_Immediate):
                parseLdrhStrhImmediateUnsignedOffsetInstruction(reinterpret_cast<LdrhStrhImmediateUnsignedOffsetInstruction*>(instruction), dw, Aarch64Instruction::LdrhImmediateUnsignedOffset);
                return true;
        }
        return false;
    }


    bool
    Aarch64Parser::parseDataProcessing3Source(Instruction* instruction, uint32_t dw)
    {
        uint32_t sf_op53_op31_o0 = DataProcessing3Source::sf_op54_op32_o0_Mask & dw;
        switch (sf_op53_op31_o0)
        {
            case static_cast<uint32_t>(DataProcessing3Source::Madd32):
            case static_cast<uint32_t>(DataProcessing3Source::Madd64):
                parseMaddSubInstruction(reinterpret_cast<MaddSubInstruction*>(instruction), dw, Aarch64Instruction::Madd);
                return true;
            case static_cast<uint32_t>(DataProcessing3Source::Msub32):
            case static_cast<uint32_t>(DataProcessing3Source::Msub64):
                parseMaddSubInstruction(reinterpret_cast<MaddSubInstruction*>(instruction), dw, Aarch64Instruction::Msub);
                return true;
        }
        return false;
    }


    bool
    Aarch64Parser::parseDataProcessing2Source(Instruction* instruction, uint32_t dw)
    {
        uint32_t opcode = DataProcessing2Source::opcode_Mask & dw;
        switch (opcode)
        {
            case static_cast<uint32_t>(DataProcessing2Source::opcode_Sdiv):
                if (S(dw) == 0)
                {
                    parseDivInstruction(reinterpret_cast<DivInstruction*>(instruction), dw, Aarch64Instruction::Sdiv);
                    return true;
                }
                break;
            case static_cast<uint32_t>(DataProcessing2Source::opcode_Udiv):
                if (S(dw) == 0)
                {
                    parseDivInstruction(reinterpret_cast<DivInstruction*>(instruction), dw, Aarch64Instruction::Udiv);
                    return true;
                }
                break;
        }
        return false;
    }


    bool
    Aarch64Parser::parseBitfieldInstruction(Instruction* instruction, uint32_t dw)
    {
        uint32_t sf_opc_N = BitfieldEncoding::sf_opc_N_Mask & dw;
        switch (sf_opc_N)
        {
            case static_cast<uint32_t>(BitfieldEncoding::Sbfm32):
            case static_cast<uint32_t>(BitfieldEncoding::Sbfm64):
                uint32_t immr = BitfieldEncoding::immr_Mask & dw;
                uint32_t imms = BitfieldEncoding::imms_Mask & dw;
                if (immr == BitfieldEncoding::immr_Sxtb && imms == BitfieldEncoding::imms_Sxtb)
                {
                    parseSxtbInstruction(reinterpret_cast<SxtbInstruction*>(instruction), dw);
                    return true;
                }
                else if (immr == BitfieldEncoding::immr_Sxth && imms == BitfieldEncoding::imms_Sxth)
                {
                    parseSxthInstruction(reinterpret_cast<SxthInstruction*>(instruction), dw);
                    return true;
                }
                break;
        }
        return false;
    }


    bool
    Aarch64Parser::parsePcRelAddressingInstruction(Instruction* instruction, uint32_t dw)
    {
        if (DataProcessingImmediateEncoding::op & dw)
        {
            parseAdrInstruction(instruction, dw);
        }
        else
        {
            parseAdrpInstruction(instruction, dw);
        }
        return true;
    }


    bool
    Aarch64Parser::parseReservedInstruction(Instruction* instruction, uint32_t dw)
    {
        uint32_t op0 = ReservedEncoding::Op0_Mask & dw;
        uint32_t op1 = ReservedEncoding::Op1_Mask & dw;
        if (op0 == ReservedEncoding::Op0_Udf && op1 == ReservedEncoding::Op1_Udf)
        {
            parseUdfInstruction(instruction, dw);
            return true;
        }
        return false;
    }
}