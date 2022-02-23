#ifndef ELET_AARCH64PRINTER_H
#define ELET_AARCH64PRINTER_H


#include "Domain/Compiler/TestHarness/BaselinePrinter.h"
#include "./Aarch64Instructions.h"


namespace elet::domain::compiler::test::aarch
{

class Aarch64Printer : public BaselinePrinter<OneOfInstruction>
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
    writeMovz(const MovzInstruction* instruction);

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

    void
    writeShiftedRegisterInstruction(const ShiftedRegisterInstruction* add);

    void
    writeLdrStrImmediateUnsignedOffsetInstruction(const LdrStrImmediateUnsignedOffsetInstruction* instruction);

    void
    writeMaddSubInstruction(const MaddSubInstruction* instruction);

    void
    writeDivInstruction(const DivInstruction* instruction);

    void
    writeLdrbStrbImmediateUnsignedOffsetInstruction(const LdrbStrbImmediateUnsignedOffsetInstruction* instruction);

    void
    writeLdrsbImmediateUnsignedOffsetInstruction(const LdrsbImmediateUnsignedOffsetInstruction* instruction);

    void
    writeLdrhStrhImmediateUnsignedOffsetInstruction(const LdrhStrhImmediateUnsignedOffsetInstruction* instruction);

    void
    writeLdrshImmediateUnsignedOffsetInstruction(const LdrshImmediateUnsignedOffsetInstruction* instruction);

    void
    writeAndImmediateInstruction(const AndImmediateInstruction* pInstruction);

    void
    writeSxtb(const SxtbInstruction* instruction);

    void
    writeSxth(const SxthInstruction* instruction);
};

}

#endif //ELET_AARCH64PRINTER_H
