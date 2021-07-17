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
    writeLoadStorePairInstruction(const LoadStorePairInstruction* instruction);

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
    writeBl(const UnconditionalBranchImmediateInstruction* pInstruction);

    void
    writeMovImmediate(const MovWideImmediateInstruction* instruction);

    void
    writeMov(const MovInstruction* pInstruction);

    void
    writeLoadStoreInstruction(const LoadStoreInstruction* instruction);

    void
    writeIndexedAddressSuffix(AddressMode addressMode, int16_t offset);

    void
    writeAdrpInstruction(const AdrpInstruction* pInstruction);
};

}

#endif //ELET_AARCH64BASELINEPRINTER_H
