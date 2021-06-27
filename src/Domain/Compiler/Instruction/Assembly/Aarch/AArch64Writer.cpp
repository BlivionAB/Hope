#include "AArch64Writer.h"
#include "AArch64Encodings.h"

namespace elet::domain::compiler::instruction::output
{


AArch64Writer::AArch64Writer(List<uint8_t>* output):
    AssemblyWriterInterface(output)
{

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
        return;
    }
    writeFunctionPrologue(routine);
}


void
AArch64Writer::writeFunctionPrologue(FunctionRoutine* routine)
{
    bw->writeDoubleWord(Aarch64Instruction::StpPreIndex64 | imm7(-16) | Rt2(AArch64Register::LR) | Rn(AArch64Register::SP) | Rt(AArch64Register::FP));
    bw->writeDoubleWord(Aarch64Instruction::AddImmediate64 | imm12(0) | Rn(AArch64Register::SP) | Rd(AArch64Register::FP));
}


constexpr uint32_t
AArch64Writer::Rd(uint32_t reg) const
{
    return reg << 0;
}


constexpr uint32_t
AArch64Writer::Rt(uint32_t reg) const
{
    return reg << 0;
}


constexpr uint32_t
AArch64Writer::Rn(uint32_t reg) const
{
    return reg << 5;
}


constexpr uint32_t
AArch64Writer::Rt2(uint32_t reg) const
{
    return reg << 10;
}


constexpr uint32_t
AArch64Writer::imm12(int16_t value) const
{
    return (static_cast<uint32_t>(0x7fff & (value / 8))) << 10;
}


constexpr uint32_t
AArch64Writer::imm7(int8_t value) const
{
    return (static_cast<uint32_t>(0x7f & (value / 8))) << 15;
}


}