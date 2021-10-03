#ifndef ELET_AARCH64ASSEMBLYPRINTER_H
#define ELET_AARCH64ASSEMBLYPRINTER_H


#include "Domain/Compiler/TestHarness/BaselinePrinter.h"
#include "./Aarch64Instructions.h"


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
    writeGeneralPurposeRegister(Register reg, const Instruction* instruction);

    void
    writeByteInstruction(const Instruction* instruction);

    void
    writeBranchExceptionSyscallInstruction(const BrInstruction* pInstruction);

    void
    writeBl(const BlInstruction* pInstruction);

    void
    writeMovImmediate(const MovzInstruction* instruction);

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

    void
    writeOrrImmediate(const OrrImmediateInstruction* instruction);

    void
    writeMovn(const MovnInstruction* instruction);

    void
    writeMovk(const MovkInstruction* instruction);

    void
    writeHw(const MovkInstruction* movInstruction);
};

}

#endif //ELET_AARCH64ASSEMBLYPRINTER_H
