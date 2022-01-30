#ifndef ELET_AARCH64ASSEMBLYPARSER_H
#define ELET_AARCH64ASSEMBLYPARSER_H


#include "Domain/Compiler/TestHarness/AssemblyParser.h"
#include "Aarch64Instructions.h"

namespace elet::domain::compiler::test::aarch
{


enum RegistrySize
{
   _64 = 64,
   _32 = 32,
};


class Aarch64AssemblyParser : public AssemblyParser
{

public:

    void
    parse(List<OneOfInstruction>& instructions, List<std::uint8_t>& output, size_t offset, size_t size);

    uint8_t
    getByte(Instruction* instruction);

    uint32_t
    getDoubleWord(Instruction* instruction);

private:

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
    uimm16(uint32_t dw);

    int32_t
    imm26(uint32_t dw);

    bool
    sf(uint32_t dw);

    uint32_t
    hw(uint32_t dw);

    uint64_t
    getLeftShiftFromHv(uint32_t dw);

    void
    parseMovzInstruction(MovzInstruction* instruction, uint32_t dw);

    bool
    tryParse21Instructions(Instruction* instruction, uint32_t dw);

    void
    parseLoadStorePairInstruction(Instruction* instruction, uint32_t dw, uint32_t kind22);

    void
    parseDataProcessImmediateInstruction(Instruction* instruction, uint32_t dw, uint32_t kind22);

    bool
    tryParse22Instructions(Instruction* instruction, uint32_t dw);

    bool
    tryParse25Instructions(Instruction* instruction, uint32_t dw);

    bool
    tryParse26Instructions(Instruction* instruction, uint32_t dw);

    void
    parseLoadStoreInstruction(Instruction* instruction, uint32_t dw, uint32_t kind22);

    bool
    tryParse24Instructions(Instruction* instruction, uint32_t dw);

    void
    parseAdrInstruction(Instruction* instruction, uint32_t dw);

    void
    parseAdrpInstruction(Instruction* instruction, uint32_t dw);

    uint32_t
    immhilo(uint32_t dw);

    void
    parseUdfInstruction(Instruction* instruction, uint32_t dw);

    void
    parseBrInstruction(Instruction* instruction, uint32_t dw);

    bool
    tryParse23Instructions(Instruction* instruction, uint32_t dw);

    void
    parseOrrInstruction(OrrImmediateInstruction* orrInstruction, uint32_t dw);

    uint64_t
    decodeBitmaskImmediate(uint32_t dw, RegistrySize registrySize);

    void
    parseMovnInstruction(MovnInstruction* movInstruction, uint32_t dw);

    void
    parseMovkInstruction(MovkInstruction* movk, uint32_t dw);

    uint8_t
    imm6(uint32_t dw);

    uint8_t
    shift(uint32_t dw);

    void
    parseLdrStrImmediateUnsignedOffsetInstruction(LdrStrUnsignedOffsetInstruction* instruction, uint32_t dw, uint32_t kind22);

    void
    parseAddShiftedRegister(AddShiftedRegisterInstruction* instruction, uint32_t dw);

    void
    parseSubShiftedRegister(SubShiftedRegisterInstruction* instruction, uint32_t dw);

    bool
    tryParse15Instructions(Instruction* instruction, uint32_t dw);

    void
    parseMaddInstruction(MaddInstruction* instruction, uint32_t dw);
};


}

#endif //ELET_AARCH64ASSEMBLYPARSER_H
