#include "Aarch64Writer.h"
#include "AArch64Encodings.h"
#include <cassert>

namespace elet::domain::compiler::instruction::output
{


Aarch64Writer::Aarch64Writer(List<uint8_t>* output):
    AssemblyWriterInterface(output)
{
    _callingConvention = { { Aarch64Register::r0, Aarch64Register::r1, Aarch64Register::r2, Aarch64Register::r4 } };
}


void
Aarch64Writer::writeTextSection(FunctionRoutine* routine)
{
    writeFunction(routine);
}


void
Aarch64Writer::writeStubs()
{
    for (const auto& routine : externalRoutines)
    {
        for (const auto& relocationAddress : routine->relocationAddresses)
        {
            uint64_t start = _offset;
            bw->writeDoubleWord(Aarch64Instruction::Nop);
            uint32_t dw = bw->getDoubleWord(relocationAddress);
            bw->writeDoubleWordAtAddress(dw | simm26((start - relocationAddress) / 4), relocationAddress);
            routine->stubAddress = _offset;
            bw->writeDoubleWord(Aarch64Instruction::Ldr64 | simm19(0) | Rt(Aarch64Register::r16));
            writeBr(Aarch64Register::r16);
        }
    }
}


void
Aarch64Writer::writeStubHelper()
{
    dyldPrivateOffset = _offset;
    uint64_t start = _offset;
    bw->writeDoubleWord(Aarch64Instruction::Adr | Rd(17) | immhilo(0));
    bw->writeDoubleWord(Aarch64Instruction::Nop);
    bw->writeDoubleWord(Aarch64Instruction::StpPreIndex64 | simm7(-16) | Rt2(17) | Rn(Aarch64Register::sp) | Rt(16));
    bw->writeDoubleWord(Aarch64Instruction::Nop);

    Utf8String* dyldStubBinderString = new Utf8String("dyld_stub_binder");
    Utf8StringView string = Utf8StringView(*dyldStubBinderString);
    ExternalRoutine* dyldStubBinderRoutine = new ExternalRoutine(string);
    gotBoundRoutines.add(dyldStubBinderRoutine);
    dyldStubBinderRoutine->relocationAddresses.add(_offset);

    bw->writeDoubleWord(Aarch64Instruction::Ldr64 | simm19(0) | Rt(16));
    bw->writeDoubleWord(Aarch64Instruction::Br | Rn(16));

    uint32_t i = 0;
    for (const auto& routine : externalRoutines)
    {
        bw->writeDoubleWord(Aarch64Instruction::Ldr32 | simm19(8) | Rt(16));
        writeB(start - _offset);
        routine->stubHelperAddress = _offset;
        bw->writeDoubleWord(0);
        i++;
    }
}


void
Aarch64Writer::writeFunctionRelocationAddresses(FunctionRoutine* routine)
{
    for (const auto& relocationAddress : routine->relocationAddresses)
    {
        switch (relocationAddress.value1)
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


void
Aarch64Writer::writeCallInstruction(CallInstruction* callInstruction, FunctionRoutine* parentRoutine)
{
    switch (callInstruction->routine->kind)
    {
        case RoutineKind::Function:
        {
            auto routine = reinterpret_cast<FunctionRoutine*>(callInstruction->routine);
            if (routine->offset)
            {
                bw->writeDoubleWordInFunction(Aarch64Instruction::Bl | simm26((routine->offset - _offset) / 4), parentRoutine);
            }
            else
            {
                routine->relocationAddresses.emplace(_offset, Aarch64Instruction::Bl);
                bw->writeDoubleWordInFunction(Aarch64Instruction::Bl, parentRoutine);
                parentRoutine->subRoutines.add(routine);
            }
            break;
        }
        case RoutineKind::External:
        {
            auto routine = reinterpret_cast<ExternalRoutine*>(callInstruction->routine);
            routine->relocationAddresses.add(_offset);
            bw->writeDoubleWordInFunction(Aarch64Instruction::Bl | simm26(0), parentRoutine);
            externalRoutines.add(routine);
            break;
        }
        default:
            throw std::runtime_error("Unknown routine kind.");

    }
}



//void
//Aarch64Writer::writeCallInstructionArguments(CallInstruction* callInstruction, FunctionRoutine* parentRoutine)
//{
//    uint64_t size = 0;
//    unsigned int i = 0;
//    for (const auto& argument : callInstruction->arguments)
//    {
//        auto reg = _callingConvention.registers[i];
//
//        if (auto parameter = std::get_if<ParameterDeclaration*>(&argument->value))
//        {
//            bw->writeDoubleWord(Aarch64Instruction::LdrImmediateBaseOffset64 | uimm12((parentRoutine->stackSize - (*parameter)->stackOffset) / 8) | Rn(Aarch64Register::sp) | Rt(reg));
//            size += 4;
//        }
//        else if (auto string = std::get_if<String*>(&argument->value))
//        {
//            (*string)->relocationAddress.value1 = _offset;
//            bw->writeDoubleWord(Aarch64Instruction::Adrp | immhilo(0) | Rd(Aarch64Register::r0));
//            (*string)->relocationAddress.offset = _offset;
//            writeAddImmediate64(Aarch64Register::r0, Aarch64Register::r0, 0);
//            size += 4;
//            _strings.add(*string);
//        }
//    }
//}


void
Aarch64Writer::writeCStringSection()
{
    for (const auto& string : _strings)
    {
        string->relocationAddress.value2 = _offset;

        for (const auto s : string->value)
        {
            bw->writeByte(static_cast<uint8_t>(s));
        }
        bw->writeByte(0);
    }
}


void
Aarch64Writer::writeFunctionPrologue(FunctionRoutine* function)
{
    if (function->stackSize == 16)
    {
        bw->writeDoubleWordInFunction(Aarch64Instruction::StpPreIndex64 | simm7(-16) | Rt2(Aarch64Register::lr) | Rn(Aarch64Register::sp) | Rt(Aarch64Register::fp), function);
        writeAddImmediate64(Aarch64Register::fp, Aarch64Register::sp, 0, function);
    }
    else
    {
        writeSubImmediate64(Aarch64Register::sp, Aarch64Register::sp, function->stackSize, function);
        bw->writeDoubleWordInFunction(Aarch64Instruction::StpBaseOffset64 | simm7(function->stackSize - 16) | Rt2(Aarch64Register::lr) | Rn(Aarch64Register::sp) | Rt(Aarch64Register::fp), function);
        writeAddImmediate64(Aarch64Register::fp, Aarch64Register::sp, 16, function);
    }
}


void
Aarch64Writer::writeFunctionEpilogue(FunctionRoutine* function)
{
    if (function->isStartFunction)
    {
        bw->writeDoubleWordInFunction(Aarch64Instruction::Movz32 | Rd(Aarch64Register::r0) | uimm16(0), function);
    }
    if (function->stackSize == 16)
    {
        bw->writeDoubleWordInFunction(Aarch64Instruction::LdpPostIndex64 | simm7(16) | Rt2(Aarch64Register::lr) | Rn(Aarch64Register::sp) | Rt(Aarch64Register::fp), function);
    }
    else
    {
        bw->writeDoubleWordInFunction(Aarch64Instruction::LdpBaseOffset64 | simm7(function->stackSize - 16) | Rt2(Aarch64Register::lr) | Rn(Aarch64Register::sp) | Rt(Aarch64Register::fp), function);
        writeAddImmediate64(Aarch64Register::sp, Aarch64Register::sp, function->stackSize, function);
    }
    bw->writeDoubleWordInFunction(Aarch64Instruction::Ret | Rn(Aarch64Register::lr), function);
}

uint32_t
Aarch64Writer::Rm(uint8_t reg) const
{
    assert(reg <= 0b11111 && "Rm cannot be larger than 0b11111");
    return reg << 16;
}


uint32_t
Aarch64Writer::Rd(uint32_t reg) const
{
    return reg << 0;
}


uint32_t
Aarch64Writer::Rt(uint32_t reg) const
{
    return reg << 0;
}


uint32_t
Aarch64Writer::Rn(uint32_t reg) const
{
    return reg << 5;
}


uint32_t
Aarch64Writer::Rt2(uint32_t reg) const
{
    return reg << 10;
}


uint32_t
Aarch64Writer::uimm12(uint16_t value) const
{
    return static_cast<uint32_t>(0x0fff & value) << 10;
}


uint32_t
Aarch64Writer::uimm6(uint8_t value) const
{
    assert(value <= 0b111111 && "Value must be smaller than 0b11111");
    return static_cast<uint32_t>(value) << 10;
}


uint32_t
Aarch64Writer::uimm16(uint16_t value) const
{
    return static_cast<uint32_t>(value) << 5;
}


uint32_t
Aarch64Writer::simm7(int8_t value) const
{
    return static_cast<uint32_t>(0x7f & (value / 8)) << 15;
}


uint32_t
Aarch64Writer::simm19(int32_t value) const
{
    // Encoded as times 4
    return static_cast<uint32_t>(0x7ffff & (value / 4)) << 5;
}


uint32_t
Aarch64Writer::simm26(int32_t value) const
{
    return static_cast<uint32_t>(MASK(6, 0) & value) << 0;
}


uint32_t
Aarch64Writer::immhilo(int32_t value) const
{
    uint32_t result = 0;
    result |= (static_cast<uint32_t>(0b11) & value) << 29;
    result |= (MASK(12, 2) & value) << 5;
    return result;
}


void
Aarch64Writer::writeAddImmediate64(Aarch64Register rd, Aarch64Register rn, int16_t value, output::FunctionRoutine* function)
{
    bw->writeDoubleWordInFunction(Aarch64Instruction::AddImmediate64 | uimm12(value) | Rn(rn) | Rd(rd), function);
}


void
Aarch64Writer::writeSubImmediate64(Aarch64Register rd, Aarch64Register rn, int16_t value, output::FunctionRoutine* function)
{
    bw->writeDoubleWordInFunction(Aarch64Instruction::SubImmediate64 | uimm12(value) | Rn(rn) | Rd(rd), function);
}


void
Aarch64Writer::relocateCStrings(uint64_t textSegmentStartOffset)
{
    for (const auto& string : _strings)
    {
        uint32_t numberOfPageSizes = ((textSegmentStartOffset + string->relocationAddress.value1) / 0x1000) * 0x1000;
        uint32_t dw = bw->getDoubleWord(textSegmentStartOffset + string->relocationAddress.offset) | uimm12((textSegmentStartOffset + string->relocationAddress.value2) - numberOfPageSizes);
        bw->writeDoubleWordAtAddress(dw, textSegmentStartOffset + string->relocationAddress.offset);
    }
}


void
Aarch64Writer::relocateStub(uint64_t offset, uint64_t textSegmentStartOffset, ExternalRoutine* externalRoutine)
{
    uint64_t stubAddress = textSegmentStartOffset + externalRoutine->stubAddress;
    uint32_t dw = bw->getDoubleWord(stubAddress);
    bw->writeDoubleWordAtAddress(dw | simm19(offset - stubAddress), stubAddress);
}


void
Aarch64Writer::relocateDyldPrivate(uint64_t dataSectionOffset, uint64_t textSegmentStartOffset)
{
    uint32_t adrInstruction = bw->getDoubleWord(textSegmentStartOffset + dyldPrivateOffset);
    bw->writeDoubleWordAtAddress(adrInstruction | immhilo(dataSectionOffset - (textSegmentStartOffset + dyldPrivateOffset)), textSegmentStartOffset + dyldPrivateOffset);
}


void
Aarch64Writer::relocateStubHelperOffset(uint64_t offset, uint64_t stubHelperAddress, uint64_t textSegmentStartOffset)
{
    bw->writeDoubleWordAtAddress(offset, textSegmentStartOffset + stubHelperAddress);
}

void

Aarch64Writer::writeBr(Aarch64Register rn)
{
    bw->writeDoubleWord(Aarch64Instruction::Br | Rn(rn));
}


void
Aarch64Writer::relocateGotBoundRoutine(uint64_t gotOffset, uint64_t offset)
{
    uint32_t dw = bw->getDoubleWord(offset);
    bw->writeDoubleWordAtAddress(dw | simm19(gotOffset), offset);
}


void
Aarch64Writer::writeB(int32_t offset)
{
    bw->writeDoubleWord(Aarch64Instruction::B | simm26(offset / 4));
}


void
Aarch64Writer::writeStoreImmediateInstruction(StoreImmediateInstruction* storeImmediateInstruction, FunctionRoutine* function)
{

}


void
Aarch64Writer::writeStoreRegisterInstruction(StoreRegisterInstruction* storeRegisterInstruction, FunctionRoutine* function)
{
    bw->writeDoubleWordInFunction(
        Aarch64Instruction::StrImmediateBaseOffset64 |
        uimm12((function->stackSize - storeRegisterInstruction->stackOffset - 8)) |
        Rn(Aarch64Register::sp) |
        Rt(getAarch64RegisterFromOperandRegister(storeRegisterInstruction->target)), function);
}

void
Aarch64Writer::writeMoveAddressInstruction(MoveAddressInstruction* moveAddressInstruction, FunctionRoutine* function)
{
    output::String* string = std::get<output::String*>(moveAddressInstruction->constant);
    string->relocationAddress.value1 = _offset;
    bw->writeDoubleWordInFunction(Aarch64Instruction::Adrp | immhilo(0) | Rd(Aarch64Register::r0), function);
    string->relocationAddress.offset = _offset;
    writeAddImmediate64(Aarch64Register::r0, Aarch64Register::r0, 0, function);
    _strings.add(string);
}

void
Aarch64Writer::writeInstructionsPadding(FunctionRoutine* function)
{
    uint64_t rest = function->codeSize % 8;
    if (rest != 0)
    {
        bw->writeDoubleWord(Aarch64Instruction::Nop);
    }
}


void
Aarch64Writer::writeLoadInstruction(LoadInstruction* loadInstruction, FunctionRoutine* function)
{
    bw->writeDoubleWordInFunction(
        Aarch64Instruction::LdrImmediateBaseOffset64 |
        uimm12((function->stackSize - loadInstruction->stackOffset - 8) / 8) |
        Rn(Aarch64Register::sp) | Rt(getAarch64RegisterFromOperandRegister(loadInstruction->destination)), function);
}


int
Aarch64Writer::getAarch64RegisterFromOperandRegister(OperandRegister operandRegister) const
{
    switch (operandRegister)
    {
        case OperandRegister::Arg0:
            return Aarch64Register::r0;
        case OperandRegister::Arg1:
            return Aarch64Register::r1;
        case OperandRegister::Arg2:
            return Aarch64Register::r2;
        case OperandRegister::Arg3:
            return Aarch64Register::r3;
        default:
            throw std::runtime_error("Cannot get aarch64 register.");
    }
}


}