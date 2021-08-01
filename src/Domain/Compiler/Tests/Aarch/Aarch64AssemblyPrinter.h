#ifndef ELET_AARCH64ASSEMBLYPRINTER_H
#define ELET_AARCH64ASSEMBLYPRINTER_H


#include "../BaselinePrinter.h"
#include "./AarchTypes.h"


namespace elet::domain::compiler::test::aarch
{

class Aarch64AssemblyPrinter : public BaselinePrinter<OneOfInstruction>
{
public:

    Utf8String
    print(List<OneOfInstruction>& instructions) override;

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
    writeBl(const BlInstruction* pInstruction);

    void
    writeMovImmediate(const MovWideImmediateInstruction* instruction);

    void
    writeMov(const MovInstruction* pInstruction);

    void
    writeLoadStoreInstruction(const LoadStoreInstruction* instruction);

    void
    writeIndexedAddressSuffix(AddressMode addressMode, int16_t offset);

    void
    writeAdrpInstruction(const AdrInstruction* pInstruction);

    void
    writeNopInstruction();
};

}

#endif //ELET_AARCH64ASSEMBLYPRINTER_H
