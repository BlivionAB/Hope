#ifndef ELET_AARCH64ASSEMBLYPRINTER_H
#define ELET_AARCH64ASSEMBLYPRINTER_H


#include "../BaselinePrinter.h"
#include "./AarchTypes.h"


namespace elet::domain::compiler::test::aarch
{

class Aarch64AssemblyPrinter : public BaselinePrinter<OneOfInstruction>
{
public:

    void
    writeInstructions(const List<OneOfInstruction>& instructions) override;

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
    writeBranchExceptionSyscallInstruction(const BrInstruction* pInstruction);

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
    writeAdrInstruction(const AdrInstruction* instruction);

    void
    writeAdrpInstruction(const AdrpInstruction* instruction);

    void
    writeNopInstruction();

    void
    writeLdr(const LdrInstruction* instruction);

    void
    writeB(const BInstruction* instruction);

    void
    writeUdf(const UdfInstruction* instruction);

    void
    writeBr(const BrInstruction* instruction);
};

}

#endif //ELET_AARCH64ASSEMBLYPRINTER_H
