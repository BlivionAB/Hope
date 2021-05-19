#ifndef ELET_X86BASELINEPRINTER_H
#define ELET_X86BASELINEPRINTER_H

#include <cstdint>
#include <variant>
#include "Foundation/List.h"
#include "X86BaselinePrinter.h"
#include "X86Types.h"
#include "X86BaselineParser.h"
#include "Domain/Compiler/Instruction/TextWriter.h"

namespace elet::domain::compiler::instruction::x86
{

using namespace elet::foundation;


class X86BaselinePrinter
{

public:

    X86BaselinePrinter();

    void
    print(List<std::uint8_t>* output);

private:

    X86BaselineParser*
    _parser;

    TextWriter
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
    writeByteDisplacement(ByteDisplacement* byteDisp);

    void
    writeByteInstruction(Instruction* instruction);

    void
    writeAddress();

    void
    writeColumnHeader();
};

}

#endif //ELET_X86BASELINEPRINTER_H
