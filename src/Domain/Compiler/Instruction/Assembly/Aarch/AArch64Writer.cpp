#include "AArch64Writer.h"
#include "AArch64Encodings.h"

namespace elet::domain::compiler::instruction::output
{


AArch64Writer::AArch64Writer(List<uint8_t>* output):
    AssemblyWriterInterface(output)
{
    _callingConvention = { { Aarch64Register::r0, Aarch64Register::r1, Aarch64Register::r2, Aarch64Register::r4 } };
}


void
AArch64Writer::writeTextSection(FunctionRoutine* routine)
{
    writeFunction(routine);
}


void
AArch64Writer::writeStubs()
{

}


void
AArch64Writer::writeStubHelper()
{
    dyldPrivateOffset = _offset;
    _dyldStubBinderRelocationAddress = _offset;
}


void
AArch64Writer::writeFunction(FunctionRoutine* routine)
{
    if (routine->hasWrittenOutput)
    {
        writeFunctionRelocationAddresses(routine);
        return;
    }
    uint64_t stackSize = getStackSizeFromFunctionParameters(routine);
    uint64_t stackOffset = 0;
    uint64_t routineSize = 0;
    routine->offset = _offset;
    writeFunctionRelocationAddresses(routine);
    writeFunctionPrologue(routine, stackSize);
    stackSize += writeFunctionInstructions(routine);
    if (routine->isStartFunction)
    {
        bw->writeDoubleWord(Aarch64Instruction::Movz64 | Rd(Aarch64Register::r8) | uimm16(0));
        bw->writeDoubleWord(Aarch64Instruction::Mov64 | Rm(Aarch64Register::r8) | uimm6(0) | Rn(0b11111) | Rd(Aarch64Register::r0));
        routineSize += 1;
        exportedRoutines.add(routine);
    }
    writeFunctionEpilogue(routine, <#initializer#>, <#initializer#>);
    for (const auto& subRoutine : routine->subRoutines)
    {
        writeFunction(subRoutine);
    }
    routine->hasWrittenOutput = true;
    internalRoutines.add(routine);
}


void
AArch64Writer::writeFunctionRelocationAddresses(FunctionRoutine* routine)
{
    for (const auto& relocationAddress : routine->relocationAddresses)
    {
        switch (relocationAddress.value)
        {
            case Aarch64Instruction::Bl:
                bw->writeDoubleWordAtAddress(Aarch64Instruction::Bl | simm26((routine->offset - relocationAddress.offset) / 4), relocationAddress.offset);
                break;
            default:
                throw std::runtime_error("Unknown relocation instruction.");
        }
    }
    routine->relocationAddresses.clear();
}


uint64_t
AArch64Writer::writeFunctionInstructions(FunctionRoutine* routine)
{
    size_t size = 0;
    for (const auto& instruction : routine->instructions)
    {
        switch (instruction->kind)
        {
            case InstructionKind::Call:
                size += writeCallInstruction(reinterpret_cast<CallInstruction*>(instruction), routine);
                break;
        }
    }
    return size;
}


uint64_t
AArch64Writer::writeCallInstruction(CallInstruction* callInstruction, FunctionRoutine* parentRoutine)
{
    uint64_t size = 0;
    switch (callInstruction->routine->kind)
    {
        case RoutineKind::Function:
        {
            auto routine = reinterpret_cast<FunctionRoutine*>(callInstruction->routine);
            if (routine->offset)
            {
                bw->writeDoubleWord(Aarch64Instruction::Bl | simm26((routine->offset - _offset) / 4));
            }
            else
            {
                routine->relocationAddresses.emplace(_offset, Aarch64Instruction::Bl);
                bw->writeDoubleWord(Aarch64Instruction::Bl);
                parentRoutine->subRoutines.add(routine);
            }
            size += 4;
            return size;
        }
    }
    return 4;
}


void
AArch64Writer::writeFunctionPrologue(FunctionRoutine* routine, uint64_t& stackSize)
{
    if (stackSize == 0)
    {
        bw->writeDoubleWord(Aarch64Instruction::StpPreIndex64 | simm7(-16) | Rt2(Aarch64Register::LR) | Rn(Aarch64Register::SP) | Rt(Aarch64Register::FP));
        bw->writeDoubleWord(Aarch64Instruction::AddImmediate64 | uimm12(0) | Rn(Aarch64Register::SP) | Rd(Aarch64Register::FP));
        stackSize += 8;
    }
    else
    {
        /*16 for stp fp, sp*/
        stackSize += 16;

        bw->writeDoubleWord(Aarch64Instruction::SubImmediate64 | uimm12(stackSize) | Rn(Aarch64Register::SP) | Rd(Aarch64Register::SP));
        bw->writeDoubleWord(Aarch64Instruction::StpBaseOffset64 | simm7(16) | Rt2(Aarch64Register::LR) | Rn(Aarch64Register::SP) | Rt(Aarch64Register::FP));
        bw->writeDoubleWord(Aarch64Instruction::AddImmediate64 | uimm12(16) | Rn(Aarch64Register::SP) | Rd(Aarch64Register::FP));
    }
}


void
AArch64Writer::writeFunctionEpilogue(FunctionRoutine* routine, uint64_t& stackSize, uint64_t& stackOffset)
{
    bw->writeDoubleWord(Aarch64Instruction::LdpPostIndex64 | simm7(16)  | Rt2(Aarch64Register::LR) | Rn(Aarch64Register::SP) | Rt(Aarch64Register::FP));
    bw->writeDoubleWord(Aarch64Instruction::Ret | Rn(Aarch64Register::LR));
}


uint32_t
AArch64Writer::Rm(uint8_t reg) const
{
    assert(reg <= 0b11111 && "Rm cannot be larger than 0b11111");
    return reg << 16;
}


uint32_t
AArch64Writer::Rd(uint32_t reg) const
{
    return reg << 0;
}


uint32_t
AArch64Writer::Rt(uint32_t reg) const
{
    return reg << 0;
}


uint32_t
AArch64Writer::Rn(uint32_t reg) const
{
    return reg << 5;
}


uint32_t
AArch64Writer::Rt2(uint32_t reg) const
{
    return reg << 10;
}


uint32_t
AArch64Writer::uimm12(uint16_t value) const
{
    return static_cast<uint32_t>(0x7fff & value) << 10;
}


uint32_t
AArch64Writer::uimm6(uint8_t value) const
{
    assert(value <= 0b111111 && "Value must be smaller than 0b11111");
    return static_cast<uint32_t>(value) << 10;
}


uint32_t
AArch64Writer::uimm16(uint16_t value) const
{
    return static_cast<uint32_t>(value) << 5;
}


uint32_t
AArch64Writer::simm7(int8_t value) const
{
    return static_cast<uint32_t>(0x7f & (value / 8)) << 15;
}


uint32_t
AArch64Writer::simm26(int32_t value) const
{
    return static_cast<uint32_t>(MASK(6, 0) & value) << 0;
}


}