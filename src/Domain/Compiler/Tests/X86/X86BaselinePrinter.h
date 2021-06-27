#ifndef ELET_X86BASELINEPRINTER_H
#define ELET_X86BASELINEPRINTER_H

#include <cstdint>
#include <variant>
#include <map>
#include "Foundation/List.h"
#include "X86BaselinePrinter.h"
#include "X86Types.h"
#include "X86Parser.h"
#include "../BaselinePrinter.h"
#include "Domain/Compiler/Instruction/TextWriter.h"

namespace elet::domain::compiler::test
{

using namespace elet::foundation;


class X86BaselinePrinter : public BaselinePrinter
{

public:

    X86BaselinePrinter();

    Utf8String
    print() override;

    uint64_t
    vmOffset = 0x0000000100000000;

    List<Instruction*>
    instructions;

private:

    X86Parser*
    _parser;

    void
    writeInstruction(Instruction* instruction);

    void
    writeGeneralPurposeRegister(const Register _register, const Instruction* instruction);

    void
    writeOperand(const Operand* operand, const Instruction* instruction);

    void
    writeByteDisplacement(ByteDisplacement* byteDisp, const Instruction* instruction);

    void
    writeLongDisplacement(LongDisplacement* displacement, const Instruction* instruction);

    void
    writeRegisterDisplacement(RegisterDisplacement* displacement, const Instruction* instruction);

    void
    writeInstructionWithName(const char* name, const Instruction* instruction);

    void
    writeByteInstruction(Instruction* instruction);

    void
    writeAddress();

    void
    writeColumnHeader();

    void
    writeSizeSuffix(const Instruction* instruction);

    void
    writeMemoryAddress(const std::array<uint8_t, 4> mem);
};

}

#endif //ELET_X86BASELINEPRINTER_H
