#ifndef ELET_AARCH64BASELINEPRINTER_H
#define ELET_AARCH64BASELINEPRINTER_H


#include "../BaselinePrinter.h"
#include "./AarchTypes.h"


namespace elet::domain::compiler::test::aarch
{

class Aarch64BaselinePrinter : public BaselinePrinter
{
public:

    List<OneOfInstruction>
    instructions;

    Utf8String
    print() override;

    void
    writeLoadStoreInstruction(const LoadStoreInstruction* instruction);

    void
    writeDataProcessImmediateInstruction(const DataProcessImmediateInstruction* instruction);

    void
    writeGeneralPurposeRegister32(Register reg);

    void
    writeGeneralPurposeRegister64(Register reg);

    void
    writeByteInstruction(const Instruction* instruction);

    void
    writeBranchExceptionSyscallInstruction(const BranchExceptionSyscallInstruction* pInstruction);

    void
    writeLdpPostIndex(const LoadStoreInstruction* instruction);

    void
    writeBl(const UnconditionalBranchImmediateInstruction* pInstruction);

    void
    writeMovImmediate(const MovWideImmediateInstruction* instruction);

    void
    writeMov(const MovInstruction* pInstruction);
};

}

#endif //ELET_AARCH64BASELINEPRINTER_H
