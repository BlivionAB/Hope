#ifndef ELET_X86ASSEMBLYPRINTER_H
#define ELET_X86ASSEMBLYPRINTER_H

#include <cstdint>
#include <variant>
#include <map>
#include "Foundation/List.h"
#include "X86AssemblyPrinter.h"
#include "X86Types.h"
#include "X86AssemblyParser.h"
#include "Domain/Compiler/TestHarness/BaselinePrinter.h"
#include "Foundation/TextWriter.h"

namespace elet::domain::compiler::test::x86
{

using namespace elet::foundation;


class X86AssemblyPrinter : public BaselinePrinter<x86::Instruction>
{

public:

    X86AssemblyPrinter();

private:

    void
    writeInstructions(const List<Instruction>& instructions) override;

    void
    writeInstruction(const Instruction& instruction);

    void
    writeGeneralPurposeRegister(const Register _register, SizeKind size);

    void
    writeOperand(const Operand* operand, const Instruction& instruction);

    void
    writeOperand(const Operand* operand, const Instruction& instruction, SizeKind overrideSize);

    void
    writeByteDisplacement(ByteDisplacement* byteDisp, const Instruction& instruction);

    void
    writeLongDisplacement(LongDisplacement* displacement, const Instruction& instruction);

    void
    writeRegisterDisplacement(RegisterDisplacement* displacement, const Instruction& instruction);

    void
    writeInstructionWithName(const char* name, const Instruction& instruction);

    void
    writeByteInstruction(const Instruction& instruction);

    void
    writeSizeSuffix(const Instruction& instruction);

    void
    writeMemoryAddress(const std::array<uint8_t, 4> mem);

    void
    writeMovzx(const Instruction& instruction);

    void
    writeMovsx(const Instruction& instruction);

    void
    writeTargetOperandSizeOverridableOperands(const Instruction& instruction);
};

}

#endif //ELET_X86ASSEMBLYPRINTER_H
