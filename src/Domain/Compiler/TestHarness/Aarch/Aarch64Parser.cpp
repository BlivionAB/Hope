#include "Aarch64Parser.h"
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
            uint32_t dw = getDoubleWord();
            uint32_t opc = RootInstruction::Op0_Mask & dw;

            switch (opc)
            {
                case static_cast<uint32_t>(RootInstruction::Reserved):
                    if (parseReservedInstruction(instructions, dw))
                    {
                        continue;
                    }
                    break;
                case static_cast<uint32_t>(RootInstruction::DataProcessing_Immediate_0):
                case static_cast<uint32_t>(RootInstruction::DataProcessing_Immediate_1):
                    if (parseDataProcessingImmediateInstruction(instructions, dw))
                    {
                        continue;
                    }
                    break;
                case static_cast<uint32_t>(RootInstruction::BranchingExceptionSystem_0):
                case static_cast<uint32_t>(RootInstruction::BranchingExceptionSystem_1):
                    if (parseBranchingExceptionSystemInstruction(instructions, dw))
                    {
                        continue;
                    }
                    break;
                case static_cast<uint32_t>(RootInstruction::LoadAndStore_0):
                case static_cast<uint32_t>(RootInstruction::LoadAndStore_1):
                case static_cast<uint32_t>(RootInstruction::LoadAndStore_2):
                case static_cast<uint32_t>(RootInstruction::LoadAndStore_3):
                    if (parseLoadAndStoreInstruction(instructions, dw))
                    {
                        continue;
                    }
                    break;
                case static_cast<uint32_t>(RootInstruction::DataProcessing_Register_0):
                case static_cast<uint32_t>(RootInstruction::DataProcessing_Register_1):
                    if (parseDataProcessingRegisterInstruction(instructions, dw))
                    {
                        continue;
                    }
                    break;
            }
            if (dw == Aarch64Instruction::Nop)
            {
                emplaceInstruction(NopInstruction(), instructions);
                continue;
            }
            throw std::runtime_error("Could not find instruction when parsing.");
        }
    }


    void
    Aarch64Parser::parseBrInstruction(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        emplaceInstruction(BrInstruction(Rn(dw)), instructions);
    }


    void
    Aarch64Parser::parseBInstruction(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        emplaceInstruction(BInstruction(imm26(dw)), instructions);
    }


    void
    Aarch64Parser::parseBlInstruction(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        emplaceInstruction(BlInstruction(imm26(dw)), instructions);
    }


    void
    Aarch64Parser::parseRetInstruction(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        emplaceInstruction(RetInstruction(Rn(dw)), instructions);
    }


    void
    Aarch64Parser::parseLdrLiteralInstruction(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        emplaceInstruction(LdrInstruction(Rt(dw), 0, opc(dw) == Aarch64Instruction::Opc1), instructions);
    }


    void
    Aarch64Parser::parseOrrInstruction(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        bool is64Bit = sf(dw);
        uint64_t value;
        if (is64Bit)
        {
            value = decodeBitmaskImmediate(dw, RegistrySize::_64);
        }
        else
        {
            value = decodeBitmaskImmediate(dw, RegistrySize::_32);
        }
        emplaceInstruction(OrrImmediateInstruction(Rd(dw), Rn(dw), value, is64Bit), instructions);
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
    Aarch64Parser::parseLdrStrImmediateUnsignedOffsetInstruction(List<OneOfInstruction>& instructions, uint32_t dw, InstructionKind kind)
    {
        assert(kind == InstructionKind::LdrImmediateUnsignedOffset ||
            kind == InstructionKind::StrImmediateUnsignedOffset && "Unsupported kind");
        emplaceInstruction(LdrStrImmediateUnsignedOffsetInstruction(kind, Rt(dw), Rn(dw), imm12(dw), dw & (0b01 << 30)), instructions);
    }


    void
    Aarch64Parser::parseLdrbStrbImmediateUnsignedOffsetInstruction(List<OneOfInstruction>& instructions, uint32_t dw, InstructionKind kind)
    {
        assert(kind == InstructionKind::StrbImmediateUnsignedOffset ||
            kind == InstructionKind::LdrbImmediateUnsignedOffset && "Must be Strb or Ldrb");
        emplaceInstruction(LdrbStrbImmediateUnsignedOffsetInstruction(kind, Rt(dw), Rn(dw), imm12(dw)), instructions);
    }


    void
    Aarch64Parser::parseLdrsbImmediateUnsignedOffsetInstruction(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        emplaceInstruction(LdrsbImmediateUnsignedOffsetInstruction(Rt(dw), Rn(dw), imm12(dw)), instructions);
    }


    void
    Aarch64Parser::parseLdrhStrhImmediateUnsignedOffsetInstruction(List<OneOfInstruction>& instructions, uint32_t dw, InstructionKind kind)
    {
        assert(kind == InstructionKind::LdrhImmediateUnsignedOffset ||
            kind == InstructionKind::StrhImmediateUnsignedOffset && "Unsupported kind");
        emplaceInstruction(LdrhStrhImmediateUnsignedOffsetInstruction(kind, Rt(dw), Rn(dw), imm12(dw)), instructions);
    }


    void
    Aarch64Parser::parseLdrshImmediateUnsignedOffsetInstruction(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        emplaceInstruction(LdrshImmediateUnsignedOffsetInstruction(Rt(dw), Rn(dw), imm12(dw)), instructions);
    }


    void
    Aarch64Parser::parseLoadStorePairInstruction(List<OneOfInstruction>& instructions, uint32_t dw, InstructionKind kind)
    {
        LoadStorePairInstruction* instruction = emplaceInstruction(LoadStorePairInstruction(
            kind,
            Rt(dw),
            Rt2(dw),
            Rn(dw),
            imm7(dw),
            AddressMode::Unknown,
            false), instructions);

        switch (kind)
        {
            case InstructionKind::StpPreIndex:
                instruction->is64Bit = true;
                instruction->addressMode = AddressMode::PreIndex;
                break;
            case InstructionKind::StpOffset:
            case InstructionKind::LdpOffset:
                instruction->is64Bit = true;
                instruction->addressMode = AddressMode::BaseOffset;
                break;
            case InstructionKind::LdpPostIndex:
                instruction->is64Bit = true;
                instruction->addressMode = AddressMode::PostIndex;
                break;
            default:
                throw std::runtime_error("Unknown load store pair instruction.");
        }
    }


    void
    Aarch64Parser::parseMovzInstruction(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        emplaceInstruction(MovzInstruction(Rd(dw), imm16(dw), hw(dw), sf(dw)), instructions);
    }


    void
    Aarch64Parser::parseMovnInstruction(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        emplaceInstruction(MovnInstruction(Rd(dw), imm16(dw), hw(dw), sf(dw)), instructions);
    }


    void
    Aarch64Parser::parseMovkInstruction(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        emplaceInstruction(MovkInstruction(Rd(dw), imm16(dw), hw(dw), sf(dw)), instructions);
    }


    uint32_t
    Aarch64Parser::getDoubleWord()
    {
        uint32_t result = 0;
        for (int i = 0; i < 4; ++i)
        {
            uint8_t byte = getByte();
            result |= (byte << 8*i);
            _currentDw[i] = byte;
        }
        return result;
    }


    uint8_t
    Aarch64Parser::getByte()
    {
        return (*_output)[_cursor++];
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
    Aarch64Parser::imm16(uint32_t dw)
    {
        return (dw & Aarch64Instruction::Imm16Mask) >> 5;
    }


    void
    Aarch64Parser::parseSubShiftedRegister(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        emplaceInstruction(SubShiftedRegisterInstruction(Rd(dw), Rn(dw), Rm(dw), shift(dw), imm6(dw), sf(dw)), instructions);
    }


    void
    Aarch64Parser::parseAddShiftedRegister(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        emplaceInstruction(AddShiftedRegisterInstruction(Rd(dw), Rn(dw), Rm(dw), shift(dw), imm6(dw), sf(dw)), instructions);
    }


    void
    Aarch64Parser::parseMaddSubInstruction(List <OneOfInstruction>& instructions, uint32_t dw, InstructionKind kind)
    {
        assert(kind == InstructionKind::Madd || kind == InstructionKind::Msub && "Unsupported kind");
        emplaceInstruction(MaddSubInstruction(kind, Rm(dw), Ra(dw), Rn(dw), Rd(dw), sf(dw)), instructions);
    }


    void
    Aarch64Parser::parseAdrpInstruction(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        emplaceInstruction(AdrpInstruction(Rd(dw), immhilo(dw)), instructions);
    }


    void
    Aarch64Parser::parseAdrInstruction(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        emplaceInstruction(AdrInstruction(Rd(dw), immhilo(dw)), instructions);
    }


    template<typename T>
    T*
    Aarch64Parser::emplaceInstruction(T instruction, List<OneOfInstruction>& instructions)
    {
        for (int i = 0; i < 4; i++)
        {
            instruction.bytes.add(_currentDw[i]);
        }
        return reinterpret_cast<T*>(instructions.emplace(instruction));
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
    Aarch64Parser::parseDivInstruction(List <OneOfInstruction>& instructions, uint32_t dw, InstructionKind kind)
    {
        assert(kind == InstructionKind::Sdiv || kind == InstructionKind::Udiv && "Unsupported kind");
        emplaceInstruction(DivInstruction(kind, Rm(dw), Rn(dw), Rd(dw), sf(dw)), instructions);
    }


    void
    Aarch64Parser::parseUdfInstruction(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        instructions.emplace(UdfInstruction(imm16(dw)));
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


    Hw
    Aarch64Parser::hw(uint32_t dw)
    {
        return static_cast<Hw>((Aarch64Instruction::HwMask & dw) >> 21);
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
    Aarch64Parser::parseAndImmediateInstruction(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        uint64_t value;
        bool is64Bit = sf(dw);
        if (is64Bit)
        {
            value = decodeBitmaskImmediate(dw, RegistrySize::_64);
        }
        else
        {
            value = decodeBitmaskImmediate(dw, RegistrySize::_32);
        }
        emplaceInstruction(AndImmediateInstruction(value, Rd(dw), Rn(dw), is64Bit), instructions);
    }


    void
    Aarch64Parser::parseSxtbInstruction(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        emplaceInstruction(SxtbInstruction(Rd(dw), Rn(dw)), instructions);
    }


    void
    Aarch64Parser::parseSxthInstruction(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        emplaceInstruction(SxthInstruction(Rd(dw), Rn(dw)), instructions);
    }


    bool
    Aarch64Parser::parseDataProcessingImmediateInstruction(List <OneOfInstruction>& instructions, uint32_t dw)
    {
        uint32_t op0 = DataProcessingImmediateEncoding::Op0_Mask & dw;
        switch (op0)
        {
            case static_cast<uint32_t>(DataProcessingImmediateEncoding::PcRelAddressing_0):
            case static_cast<uint32_t>(DataProcessingImmediateEncoding::PcRelAddressing_1):
                if (parsePcRelAddressingInstruction(instructions, dw))
                {
                    return true;
                }
                break;
            case static_cast<uint32_t>(DataProcessingImmediateEncoding::AddSubtractImmediate32):
                if (parseAddSubtractImmediateInstruction(instructions, dw))
                {
                    return true;
                }
                break;
            case static_cast<uint32_t>(DataProcessingImmediateEncoding::LogicalImmediate):
                if (parseLogicalImmediateInstruction(instructions, dw))
                {
                    return true;
                }
                break;
            case static_cast<uint32_t>(DataProcessingImmediateEncoding::MoveWideImmediate):
                if (parseMoveWideImmediateInstruction(instructions, dw))
                {
                    return true;
                }
                break;
            case static_cast<uint32_t>(DataProcessingImmediateEncoding::Bitfield):
                if (parseBitfieldInstruction(instructions, dw))
                {
                    return true;
                }
                break;
        }
        return false;
    }


    bool
    Aarch64Parser::parseBranchingExceptionSystemInstruction(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        uint32_t op0 = BranchingExceptionSystemEncoding::Op0_Mask & dw;
        uint32_t op1 = BranchingExceptionSystemEncoding::Op1_Mask & dw;
        switch (op0)
        {
            case static_cast<uint32_t>(BranchingExceptionSystemEncoding::Op0_Grp6):
                if (BranchingExceptionSystemEncoding::Op1_UnconditionalBranch_Register & op1)
                {
                    if (parseUnconditionalBranchRegister(instructions, dw))
                    {
                        return true;
                    }
                }
                break;
            case static_cast<uint32_t>(BranchingExceptionSystemEncoding::Op0_GrpUnconditionalBranchImmediate_0):
            case static_cast<uint32_t>(BranchingExceptionSystemEncoding::Op0_GrpUnconditionalBranchImmediate_1):
                if (parseUnconditionalBranchImmediate(instructions, dw))
                {
                    return true;
                }
                break;

        }
        return false;
    }


    bool
    Aarch64Parser::parseUnconditionalBranchRegister(List<OneOfInstruction>& instructions, uint32_t dw)
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
                    if (parseUnconditionalBranchOpcOp2Grp0(instructions, dw, op3, op4))
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
                        parseRetInstruction(instructions, dw);
                        return true;
                    }
                }
        }
        return false;
    }


    bool
    Aarch64Parser::parseUnconditionalBranchOpcOp2Grp0(List<OneOfInstruction>& instructions, uint32_t dw, uint32_t op3, uint32_t op4)
    {
        switch (op3)
        {
            case static_cast<uint32_t>(UnconditionalBranch_Register::Op3_0):
                if (UnconditionalBranch_Register::Op4_0 == op4)
                {
                    parseBrInstruction(instructions, dw);
                    return true;
                }
        }
        return false;
    }


    bool
    Aarch64Parser::parseUnconditionalBranchImmediate(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        uint32_t op0 = UnconditionalBranch_Immediate::Op0_Mask & dw;
        if (UnconditionalBranch_Immediate::Op0_B & op0)
        {
            parseBInstruction(instructions, dw);
        }
        else
        {
            parseBlInstruction(instructions, dw);
        }
        return true;
    }


    bool
    Aarch64Parser::parseLoadAndStoreInstruction(List<OneOfInstruction>& instructions, uint32_t dw)
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
                    parseLdrLiteralInstruction(instructions, dw);
                    return true;
                }
                break;
            case static_cast<uint32_t>(LoadAndStoreEncoding::Op0_xx10_0):
            case static_cast<uint32_t>(LoadAndStoreEncoding::Op0_xx10_1):
            case static_cast<uint32_t>(LoadAndStoreEncoding::Op0_xx10_2):
            case static_cast<uint32_t>(LoadAndStoreEncoding::Op0_xx10_3):
                if (parse_Op0_xx10(op2, instructions, dw))
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
                    if (parseLoadStore_UnsignedImmediate(instructions, dw))
                    {
                        return true;
                    }
                }
                break;
        }
        return false;
    }


    bool
    Aarch64Parser::parse_Op0_xx10(uint32_t op2, List<OneOfInstruction>& instructions, uint32_t dw)
    {
        switch (op2)
        {
            case static_cast<uint32_t>(LoadAndStoreEncoding::Op2_1):
                if (parseLoadStorePair_PostIndexed_Instruction(instructions, dw))
                {
                    return true;
                }
                break;
            case static_cast<uint32_t>(LoadAndStoreEncoding::Op2_2):
                if (parseLoadStorePair_Offset_Instruction(instructions, dw))
                {
                    return true;
                }
                break;
            case static_cast<uint32_t>(LoadAndStoreEncoding::Op2_3):
                if (parseLoadStorePair_PreIndexed_Instruction(instructions, dw))
                {
                    return true;
                }
                break;
        }
        return false;
    }


    bool
    Aarch64Parser::parseDataProcessingRegisterInstruction(List<OneOfInstruction>& instructions, uint32_t dw)
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
                    if (parseAddSubtractShiftedRegister(instructions, dw))
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
                    if (parseDataProcessing3Source(instructions, dw))
                    {
                        return true;
                    }
                    break;
                default:
                    if (op0 == DataProcessingRegisterInstruction::Op0_0)
                    {
                        if (op2 == DataProcessingRegisterInstruction::Op2_0110)
                        {
                            if (parseDataProcessing2Source(instructions, dw))
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
    Aarch64Parser::parseAddSubtractShiftedRegister(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        uint32_t sf_op_S = AddSubtractShiftedRegisterInstruction::sf_op_S_Mask & dw;
        switch (sf_op_S)
        {
            case static_cast<uint32_t>(AddSubtractShiftedRegisterInstruction::Add32):
            case static_cast<uint32_t>(AddSubtractShiftedRegisterInstruction::Add64):
                parseAddShiftedRegister(instructions, dw);
                return true;
            case static_cast<uint32_t>(AddSubtractShiftedRegisterInstruction::Sub32):
            case static_cast<uint32_t>(AddSubtractShiftedRegisterInstruction::Sub64):
                parseSubShiftedRegister(instructions, dw);
                return true;
        }
        return false;
    }


    bool
    Aarch64Parser::parseLogicalImmediateInstruction(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        uint32_t sf_opc = LogicalImmediateEncoding::sf_opc_Mask & dw;
        switch (sf_opc)
        {
            case static_cast<uint32_t>(LogicalImmediateEncoding::AndImmediate32):
            case static_cast<uint32_t>(LogicalImmediateEncoding::AndImmediate64):
                parseAndImmediateInstruction(instructions, dw);
                return true;
            case static_cast<uint32_t>(LogicalImmediateEncoding::OrrImmediate32):
            case static_cast<uint32_t>(LogicalImmediateEncoding::OrrImmediate64):
                parseOrrInstruction(instructions, dw);
                return true;
        }
        return false;
    }


    bool
    Aarch64Parser::parseMoveWideImmediateInstruction(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        uint32_t sf_opc = MoveWideImmediateEncoding::sf_opc_Mask & dw;
        switch (sf_opc)
        {
            case static_cast<uint32_t>(MoveWideImmediateEncoding::Movn32):
            case static_cast<uint32_t>(MoveWideImmediateEncoding::Movn64):
                parseMovnInstruction(instructions, dw);
                return true;
            case static_cast<uint32_t>(MoveWideImmediateEncoding::Movz32):
            case static_cast<uint32_t>(MoveWideImmediateEncoding::Movz64):
                parseMovzInstruction(instructions, dw);
                return true;
            case static_cast<uint32_t>(MoveWideImmediateEncoding::Movk32):
            case static_cast<uint32_t>(MoveWideImmediateEncoding::Movk64):
                parseMovkInstruction(instructions, dw);
                return true;
        }
        return false;
    }


    bool
    Aarch64Parser::parseLoadStorePair_PostIndexed_Instruction(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        uint32_t opc_V_L = LoadStoreEncoding::opc_V_L_Mask & dw;
        switch (opc_V_L)
        {
            case static_cast<uint32_t>(LoadStoreEncoding::Ldp64):
                parseLoadStorePairInstruction(instructions, dw, InstructionKind::LdpPostIndex);
                return true;
        }
        return false;
    }


    bool
    Aarch64Parser::parseLoadStorePair_Offset_Instruction(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        uint32_t opc_V_L = LoadStoreEncoding::opc_V_L_Mask & dw;
        switch (opc_V_L)
        {
            case static_cast<uint32_t>(LoadStoreEncoding::Stp64):
                parseLoadStorePairInstruction(instructions, dw, InstructionKind::StpOffset);
                return true;
            case static_cast<uint32_t>(LoadStoreEncoding::Ldp64):
                parseLoadStorePairInstruction(instructions, dw, InstructionKind::LdpOffset);
                return true;
        }
        return false;
    }


    bool
    Aarch64Parser::parseLoadStorePair_PreIndexed_Instruction(List<OneOfInstruction>& instruction, uint32_t dw)
    {
        uint32_t opc_V_L = LoadStoreEncoding::opc_V_L_Mask & dw;
        switch (opc_V_L)
        {
            case static_cast<uint32_t>(LoadStoreEncoding::Stp64):
                parseLoadStorePairInstruction(instruction, dw, InstructionKind::StpPreIndex);
                return true;
        }
        return false;
    }


    bool
    Aarch64Parser::parseAddSubtractImmediateInstruction(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        uint32_t sf_op_S = AddSubtract_Immediate_Encoding::sf_op_S_Mask & dw;
        switch (sf_op_S)
        {
            case static_cast<uint32_t>(AddSubtract_Immediate_Encoding::Add_Immediate32):
                parseAddSubtractImmediateInstruction(instructions, dw, InstructionKind::AddImmediate);
                return true;
            case static_cast<uint32_t>(AddSubtract_Immediate_Encoding::Sub_Immediate32):
                parseAddSubtractImmediateInstruction(instructions, dw, InstructionKind::SubImmediate);
                return true;
            case static_cast<uint32_t>(AddSubtract_Immediate_Encoding::Add_Immediate64):
                parseAddSubtractImmediateInstruction(instructions, dw, InstructionKind::AddImmediate);
                return true;
            case static_cast<uint32_t>(AddSubtract_Immediate_Encoding::Sub_Immediate64):
                parseAddSubtractImmediateInstruction(instructions, dw, InstructionKind::SubImmediate);
                return true;
        }
        return false;
    }


    void
    Aarch64Parser::parseAddSubtractImmediateInstruction(List<OneOfInstruction>& instructions, uint32_t dw, InstructionKind kind)
    {
        emplaceInstruction(AddSubImmediateInstruction(kind, Rd(dw), Rn(dw), imm12(dw), sf(dw)), instructions);
    }


    bool
    Aarch64Parser::parseLoadStore_UnsignedImmediate(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        uint32_t size_V_opc = LoadStoreRegister_UnsignedImmediate::size_V_opc_Mask & dw;
        switch (size_V_opc)
        {
            case static_cast<uint32_t>(LoadStoreRegister_UnsignedImmediate::Strb_Immediate):
                parseLdrbStrbImmediateUnsignedOffsetInstruction(instructions, dw, InstructionKind::StrbImmediateUnsignedOffset);
                return true;
            case static_cast<uint32_t>(LoadStoreRegister_UnsignedImmediate::Ldrb_Immediate):
                parseLdrbStrbImmediateUnsignedOffsetInstruction(instructions, dw, InstructionKind::LdrbImmediateUnsignedOffset );
                return true;
            case static_cast<uint32_t>(LoadStoreRegister_UnsignedImmediate::Ldrsb_Immediate32):
                parseLdrsbImmediateUnsignedOffsetInstruction(instructions, dw);
                return true;
            case static_cast<uint32_t>(LoadStoreRegister_UnsignedImmediate::Ldrsh_Immediate32):
                parseLdrshImmediateUnsignedOffsetInstruction(instructions, dw);
                return true;
            case static_cast<uint32_t>(LoadStoreRegister_UnsignedImmediate::Ldr_Immediate_32):
            case static_cast<uint32_t>(LoadStoreRegister_UnsignedImmediate::Ldr_Immediate_64):
                parseLdrStrImmediateUnsignedOffsetInstruction(instructions, dw, InstructionKind::LdrImmediateUnsignedOffset);
                return true;
            case static_cast<uint32_t>(LoadStoreRegister_UnsignedImmediate::Str_Immediate_32):
            case static_cast<uint32_t>(LoadStoreRegister_UnsignedImmediate::Str_Immediate_64):
                parseLdrStrImmediateUnsignedOffsetInstruction(instructions, dw, InstructionKind::StrImmediateUnsignedOffset);
                return true;
            case static_cast<uint32_t>(LoadStoreRegister_UnsignedImmediate::Strh_Immediate):
                parseLdrhStrhImmediateUnsignedOffsetInstruction(instructions, dw, InstructionKind::StrhImmediateUnsignedOffset);
                return true;
            case static_cast<uint32_t>(LoadStoreRegister_UnsignedImmediate::Ldrh_Immediate):
                parseLdrhStrhImmediateUnsignedOffsetInstruction(instructions, dw, InstructionKind::LdrhImmediateUnsignedOffset);
                return true;
        }
        return false;
    }


    bool
    Aarch64Parser::parseDataProcessing3Source(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        uint32_t sf_op53_op31_o0 = DataProcessing3Source::sf_op54_op32_o0_Mask & dw;
        switch (sf_op53_op31_o0)
        {
            case static_cast<uint32_t>(DataProcessing3Source::Madd32):
            case static_cast<uint32_t>(DataProcessing3Source::Madd64):
                parseMaddSubInstruction(instructions, dw, InstructionKind::Madd);
                return true;
            case static_cast<uint32_t>(DataProcessing3Source::Msub32):
            case static_cast<uint32_t>(DataProcessing3Source::Msub64):
                parseMaddSubInstruction(instructions, dw, InstructionKind::Msub);
                return true;
        }
        return false;
    }


    bool
    Aarch64Parser::parseDataProcessing2Source(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        uint32_t opcode = DataProcessing2Source::opcode_Mask & dw;
        switch (opcode)
        {
            case static_cast<uint32_t>(DataProcessing2Source::opcode_Sdiv):
                if (S(dw) == 0)
                {
                    parseDivInstruction(instructions, dw, InstructionKind::Sdiv);
                    return true;
                }
                break;
            case static_cast<uint32_t>(DataProcessing2Source::opcode_Udiv):
                if (S(dw) == 0)
                {
                    parseDivInstruction(instructions, dw, InstructionKind::Udiv);
                    return true;
                }
                break;
        }
        return false;
    }


    bool
    Aarch64Parser::parseBitfieldInstruction(List<OneOfInstruction>& instructions, uint32_t dw)
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
                    parseSxtbInstruction(instructions, dw);
                    return true;
                }
                else if (immr == BitfieldEncoding::immr_Sxth && imms == BitfieldEncoding::imms_Sxth)
                {
                    parseSxthInstruction(instructions, dw);
                    return true;
                }
                break;
        }
        return false;
    }


    bool
    Aarch64Parser::parsePcRelAddressingInstruction(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        if (DataProcessingImmediateEncoding::op & dw)
        {
            parseAdrpInstruction(instructions, dw);
        }
        else
        {
            parseAdrInstruction(instructions, dw);
        }
        return true;
    }


    bool
    Aarch64Parser::parseReservedInstruction(List<OneOfInstruction>& instructions, uint32_t dw)
    {
        uint32_t op0 = ReservedEncoding::Op0_Mask & dw;
        uint32_t op1 = ReservedEncoding::Op1_Mask & dw;
        if (op0 == ReservedEncoding::Op0_Udf && op1 == ReservedEncoding::Op1_Udf)
        {
            parseUdfInstruction(instructions, dw);
            return true;
        }
        return false;
    }
}