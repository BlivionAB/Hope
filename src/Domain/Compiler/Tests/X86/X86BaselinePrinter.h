#ifndef ELET_X86BASELINEPRINTER_H
#define ELET_X86BASELINEPRINTER_H

#include <cstdint>
#include <variant>
#include "Foundation/List.h"
#include "X86BaselinePrinter.h"
#include "X86Types.h"
#include "X86Parser.h"
#include "Domain/Compiler/Instruction/TextWriter.h"

namespace elet::domain::compiler::test
{

using namespace elet::foundation;


class X86BaselinePrinter
{

public:

    X86BaselinePrinter();

    void
    print(List<Instruction*>& instructions);

private:

    X86Parser*
    _parser;

    instruction::TextWriter*
    _tw;

    std::uint32_t
    _address = 0;

    void
    writeInstruction(Instruction* instruction);

    void
    writeGeneralPurposeRegister(const Register _register);

    void
    writeOperand(Operand* operand);

    void
    writeByteDisplacement(ByteDisplacement* displacement);

    void
    writeLongDisplacement(LongDisplacement* displacement);

    void
    writeByteInstruction(Instruction* instruction);

    void
    writeAddress();

    void
    writeColumnHeader();

    void
    writeSizeSuffix(Instruction* instruction);
};

}

#endif //ELET_X86BASELINEPRINTER_H
