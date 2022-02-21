#ifndef ELET_AARCH64PARSER_H
#define ELET_AARCH64PARSER_H


#include "Domain/Compiler/TestHarness/AssemblyParser.h"
#include "Aarch64Instructions.h"
#include <array>

namespace elet::domain::compiler::test::aarch
{
    enum RegistrySize
    {
       _64 = 64,
       _32 = 32,
    };


    class Aarch64Parser : public AssemblyParser
    {

    public:

        void
        parse(List<OneOfInstruction>& instructions, List<std::uint8_t>& output, size_t offset, size_t size);

        uint8_t
        getByte(Instruction* instruction);

        uint32_t
        getDoubleWord(Instruction* instruction);

    private:

        std::array<uint8_t, 4>
        _currentDw;

        Register
        Rn(uint32_t dw);

        Register
        Rm(uint32_t dw);

        Register
        Ra(uint32_t dw);

        Register
        Rt(uint32_t dw);

        Register
        Rt2(uint32_t dw);

        Register
        Rd(uint32_t dw);

        int8_t
        imm7(uint32_t dw);

        int16_t
        imm12(uint32_t dw);

        uint16_t
        imm16(uint32_t dw);

        int32_t
        imm26(uint32_t dw);

        uint32_t
        opc(uint32_t dw);

        bool
        sf(uint32_t dw);

        bool
        S(uint32_t dw);

        Hw
        hw(uint32_t dw);

        uint32_t
        immhilo(uint32_t dw);

        uint64_t
        getLeftShiftFromHv(uint32_t dw);

        template<typename T>
        T*
        emplaceInstruction(T instruction, List<OneOfInstruction>& instructions);

        void
        parseMovzInstruction(List<OneOfInstruction>& instructions, uint32_t dw);

        void
        parseLoadStorePairInstruction(List<OneOfInstruction>& instructions, uint32_t dw, Aarch64Instruction kind);

        void
        parseBInstruction(List<OneOfInstruction>& instructions, uint32_t dw);

        void
        parseBlInstruction(List<OneOfInstruction>& instructions, uint32_t dw);

        void
        parseRetInstruction(List<OneOfInstruction>& instructions, uint32_t dw);

        void
        parseAdrInstruction(List<OneOfInstruction>& instructions, uint32_t dw);

        void
        parseAdrpInstruction(List<OneOfInstruction>& instruction, uint32_t dw);

        void
        parseAddSubtractImmediateInstruction(List<OneOfInstruction>& instructions, uint32_t dw, Aarch64Instruction kind);

        void
        parseUdfInstruction(List<OneOfInstruction>& instructions, uint32_t dw);

        void
        parseBrInstruction(List<OneOfInstruction>& instructions, uint32_t dw);

        void
        parseOrrInstruction(List<OneOfInstruction>& instructions, uint32_t dw);

        uint64_t
        decodeBitmaskImmediate(uint32_t dw, RegistrySize registrySize);

        void
        parseMovnInstruction(List<OneOfInstruction>& instructions, uint32_t dw);

        void
        parseMovkInstruction(List<OneOfInstruction>& instructions, uint32_t dw);

        uint8_t
        imm6(uint32_t dw);

        uint8_t
        shift(uint32_t dw);

        void
        parseLdrStrImmediateUnsignedOffsetInstruction(List <OneOfInstruction>& instructions, uint32_t dw, Aarch64Instruction kind);

        void
        parseLdrbStrbImmediateUnsignedOffsetInstruction(List <OneOfInstruction>& instructions, uint32_t dw, Aarch64Instruction kind);

        void
        parseLdrsbImmediateUnsignedOffsetInstruction(List <OneOfInstruction>& instructions, uint32_t dw);

        void
        parseLdrhStrhImmediateUnsignedOffsetInstruction(List <OneOfInstruction>& instructions, uint32_t dw, Aarch64Instruction kind);

        void
        parseLdrshImmediateUnsignedOffsetInstruction(List <OneOfInstruction>& instructions, uint32_t dw);

        void
        parseAddShiftedRegister(List <OneOfInstruction>& instructions, uint32_t dw);

        void
        parseSubShiftedRegister(List <OneOfInstruction>& instructions, uint32_t dw);

        void
        parseMaddSubInstruction(List <OneOfInstruction>& instructions, uint32_t dw, Aarch64Instruction kind);

        void
        parseDivInstruction(List <OneOfInstruction>& instructions, uint32_t dw, Aarch64Instruction kind);

        void
        parseAndImmediateInstruction(List<OneOfInstruction>& instructions, uint32_t dw);

        void
        parseSxtbInstruction(List<OneOfInstruction>& instructions, uint32_t dw);

        void
        parseSxthInstruction(List<OneOfInstruction>& instructions, uint32_t dw);

        bool
        parseDataProcessingImmediateInstruction(List <OneOfInstruction>& instructions, uint32_t dw);

        bool
        parseBranchingExceptionSystemInstruction(List<OneOfInstruction>& instructions, uint32_t dw);

        bool
        parseUnconditionalBranchRegister(List<OneOfInstruction>& instructions, uint32_t dw);

        bool
        parseUnconditionalBranchOpcOp2Grp0(List<OneOfInstruction>& instructions, uint32_t dw, uint32_t op3, uint32_t op4);

        bool
        parseUnconditionalBranchImmediate(List<OneOfInstruction>& instructions, uint32_t dw);

        void
        parseLdrLiteralInstruction(List<OneOfInstruction>& instructions, uint32_t dw);

        bool
        parseLoadAndStoreInstruction(List<OneOfInstruction>& instructions, uint32_t dw);

        bool
        parseDataProcessingRegisterInstruction(List <OneOfInstruction>& instructions, uint32_t dw);

        bool
        parseAddSubtractShiftedRegister(List <OneOfInstruction>& instructions, uint32_t dw);

        bool
        parseLogicalImmediateInstruction(List<OneOfInstruction>& instructions, uint32_t dw);

        bool
        parseMoveWideImmediateInstruction(List<OneOfInstruction>& instructions, uint32_t dw);

        bool
        parseLoadStorePair_PreIndexed_Instruction(List<OneOfInstruction>& instruction, uint32_t dw);

        bool
        parseLoadStorePair_Offset_Instruction(List<OneOfInstruction>& instructions, uint32_t dw);

        bool
        parseLoadStorePair_PostIndexed_Instruction(List<OneOfInstruction>& instructions, uint32_t dw);

        bool
        parseAddSubtractImmediateInstruction(List<OneOfInstruction>& instructions, uint32_t dw);

        bool
        parse_Op0_xx10(uint32_t op2, List<OneOfInstruction>& instructions, uint32_t dw);

        bool
        parseLoadStore_UnsignedImmediate(List <OneOfInstruction>& instructions, uint32_t dw);

        bool
        parseDataProcessing3Source(List <OneOfInstruction>& instructions, uint32_t dw);

        bool
        parseDataProcessing2Source(List <OneOfInstruction>& instructions, uint32_t dw);

        bool
        parseBitfieldInstruction(List<OneOfInstruction>& instructions, uint32_t dw);

        bool
        parsePcRelAddressingInstruction(List<OneOfInstruction>& instructions, uint32_t dw);

        bool
        parseReservedInstruction(List<OneOfInstruction>& instructions, uint32_t dw);

        void
        resetToLastInstructionCursor(List<OneOfInstruction>& instructions) const;
    };
}

#endif //ELET_AARCH64PARSER_H
