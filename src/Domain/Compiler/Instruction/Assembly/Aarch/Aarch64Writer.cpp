#include "Aarch64Writer.h"
#include "AArch64Encodings.h"
#include <cassert>
#include <Foundation/Bit.h>

namespace elet::domain::compiler::instruction::output
{
    using namespace elet::foundation;


    Aarch64Writer::Aarch64Writer(List<uint8_t>* output):
        AssemblyWriterInterface(output)
    {
        _parameterRegisters = { Aarch64Register::r0, Aarch64Register::r1, Aarch64Register::r2, Aarch64Register::r4 };
    }


    void
    Aarch64Writer::writeTextSection(FunctionRoutine* routine)
    {
        writeFunction(routine);
    }


    void
    Aarch64Writer::writeStubs()
    {
        for (const auto& routine : externalRoutineList)
        {
            for (const auto& relocationAddress : routine->relocationAddressList)
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
        dyldStubBinderRoutine->relocationAddressList.add(_offset);

        bw->writeDoubleWord(Aarch64Instruction::Ldr64 | simm19(0) | Rt(16));
        bw->writeDoubleWord(Aarch64Instruction::Br | Rn(16));

        uint32_t i = 0;
        for (const auto& routine : externalRoutineList)
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
                routine->relocationAddressList.add(_offset);
                bw->writeDoubleWordInFunction(Aarch64Instruction::Bl | simm26(0), parentRoutine);
                externalRoutineList.add(routine);
                break;
            }
            default:
                throw std::runtime_error("Unknown routine kind.");

        }
    }

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
        if (function->inferReturnZero)
        {
            bw->writeDoubleWordInFunction(Aarch64Instruction::Movz | Rd(Aarch64Register::r0) | uimm16(0), function);
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
    Aarch64Writer::Ra(uint8_t reg) const
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
    Aarch64Writer::writeAddRegisterInstruction(AddRegisterToRegisterInstruction* addRegisterInstruction, FunctionRoutine* function)
    {
        writeSfInstructionInFunction(
            Aarch64Instruction::AddShiftedRegister |
            shift(Shift::LSL) |
            Rm(getAarch64RegisterFromOperandRegister(addRegisterInstruction->target)) |
            uimm6(0) |
            Rn(getAarch64RegisterFromOperandRegister(addRegisterInstruction->value)) |
            Rt(getAarch64RegisterFromOperandRegister(addRegisterInstruction->destination)),
            addRegisterInstruction, function);
    }


    void
    Aarch64Writer::writeSubtractRegisterToRegisterInstruction(SubtractRegisterToRegisterInstruction* instruction, FunctionRoutine* function)
    {
        writeSfInstructionInFunction(
            Aarch64Instruction::SubShiftedRegister |
            shift(Shift::LSL) |
            Rm(getAarch64RegisterFromOperandRegister(instruction->target)) |
            uimm6(0) |
            Rn(getAarch64RegisterFromOperandRegister(instruction->value)) |
            Rt(getAarch64RegisterFromOperandRegister(instruction->destination)),
            instruction, function);
    }



    void
    Aarch64Writer::writeMultiplySignedRegisterToRegisterInstruction(MultiplySignedRegisterToRegisterInstruction* instruction, FunctionRoutine* function)
    {
        writeSfInstructionInFunction(
            Aarch64Instruction::Madd |
            Rm(getAarch64RegisterFromOperandRegister(instruction->target)) |
            Ra(Aarch64Register::zero) |
            Rn(getAarch64RegisterFromOperandRegister(instruction->value)) |
            Rd(getAarch64RegisterFromOperandRegister(instruction->destination)),
            instruction, function);
    }


    void
    Aarch64Writer::writeMoveImmediateInstruction(MoveImmediateInstruction* instruction, FunctionRoutine* function)
    {
        Aarch64Register rd = static_cast<Aarch64Register>(Rd(getAarch64RegisterFromOperandRegister(instruction->destination)));
        uint64_t value = instruction->value;
        if (value <= UINT16_MAX)
        {
            return writeSfInstructionInFunction(Aarch64Instruction::Movz | uimm16(value) | rd, instruction, function);
        }
        else if (value <= UINT32_MAX)
        {
            return writeNegatedOrRegularShiftMoves(value, rd, instruction, function);
        }
        else if (value <= UINT64_MAX)
        {
            return writeNegatedOrRegularShiftMoves(value, rd, instruction, function);
        }
        throw std::runtime_error("Cannot process immediate immediateValue in writeMoveImmediateInstruction.");
    }


    void
    Aarch64Writer::writeNegatedOrRegularShiftMoves(uint64_t value, const Aarch64Register& rd, MoveImmediateInstruction* instruction, FunctionRoutine* function)
    {
        if (processLogicalImmediate(value, rd, instruction->registerSize, instruction, function))
        {
            return;
        }
        if (processNegatedImmediateEncoding(value, rd, instruction, function))
        {
            return;
        }
        processPositiveValues(value, rd, instruction, function);
    }


    void
    Aarch64Writer::processPositiveValues(uint64_t value, const Aarch64Register& rd, MoveImmediateInstruction* instruction, FunctionRoutine* function)
    {
        bool hasWrittenMovz = false;
        uint64_t i = 0;
        if (instruction->registerSize == RegisterSize::Quad)
        {
            bool hasBitmaskImmediate = hasWrittenMovz = processLogicalImmediate(0xffff'ffff & value, rd, RegisterSize::Dword, instruction, function);
            if (hasBitmaskImmediate)
            {
                i = 32;
            }
        }
        uint8_t registerSize = static_cast<uint8_t>(instruction->registerSize) * 8;
        for (; i < registerSize; i += 16)
        {
            uint64_t elementValue = ((0xffffui64 << i) & value) >> i;
            if (!hasWrittenMovz && elementValue != 0ui64)
            {
                writeSfInstructionInFunction(Aarch64Instruction::Movz | hw(i) | uimm16(elementValue) | rd, instruction, function);
                hasWrittenMovz = true;
                continue;
            }
            if (elementValue != 0)
            {
                writeSfInstructionInFunction(Aarch64Instruction::Movk | uimm16(elementValue) | hw(i) | rd, instruction, function);
            }
        }

        // We can only reach here if value == 0.
        if (!hasWrittenMovz)
        {
            writeSfInstructionInFunction(Aarch64Instruction::Movz | hw(0) | uimm16(0) | rd, instruction, function);
        }
    }


    bool
    Aarch64Writer::processNegatedImmediateEncoding(uint64_t value, const Aarch64Register& rd, MoveImmediateInstruction* instruction, output::FunctionRoutine* function)
    {
        enum class Stage
        {
            Negation,
            Tail,
        };
        Stage stage = Stage::Negation;
        uint64_t negationIndex = 0;
        uint64_t onesSequenceCount = 0;
        uint64_t zeroesSequenceCount = 0;
        uint8_t registerSize = static_cast<int>(instruction->registerSize) * 8;
        for (int i = registerSize - 16; i >= 0; i -= 16)
        {
            uint32_t elementValue = ((0xffffui64 << i) & value) >> i;
            if (elementValue == 0)
            {
                zeroesSequenceCount++;
            }
            if (stage == Stage::Negation)
            {
                if (elementValue == 0xffff)
                {
                    onesSequenceCount++;
                    continue;
                }
                negationIndex = i;
                stage = Stage::Tail;
                continue;
            }
            if (stage == Stage::Tail)
            {
                if (elementValue == 0xffff)
                {
                    onesSequenceCount++;
                }
                continue;
            }
        }
        // Note that -1 or 111... will fall through here with stage == Stage::Negation and negationIndex == 0.


        // Write a nonNegated value if there exists equal or higher amounts of zero sequences.
        //
        // 11_11_11_11: 4 1s
        // 00_00_00_00: 4 0s
        // 11_00_11_00: 2 1s 2 0s
        // nonOnesSequenceCount plus zeroesSequenceCount
        if (zeroesSequenceCount >= onesSequenceCount)
        {
            return false;
        }

        uint16_t maskedValue = ((0xffffui64 << negationIndex) & value) >> negationIndex;
        uint16_t elementValue = ~maskedValue;
        writeSfInstructionInFunction(Aarch64Instruction::Movn | hw(negationIndex) | uimm16(elementValue) | rd, instruction, function);
        for (int i = 0; i < negationIndex; i += 16)
        {
            elementValue = ((0xffffui64 << i) & value) >> i;
            if (elementValue != 0xffff)
            {
                writeSfInstructionInFunction(Aarch64Instruction::Movk | uimm16(elementValue) | hw(i) | rd, instruction, function);
            }
        }
        return true;
    }


    uint32_t
    Aarch64Writer::hw(uint32_t leftShift) const
    {
        uint32_t hw;
        if (leftShift == 0)
        {
            hw = 0;
        }
        else if (leftShift == 16)
        {
            hw = 1;
        }
        else if (leftShift == 32)
        {
            hw = 2;
        }
        else if (leftShift == 48)
        {
            hw = 3;
        }
        else
        {
            throw std::runtime_error("getLeftShiftFromHv only support 16, 32, 48 values.");
        }
        return hw << 21;
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
    Aarch64Writer::writeStoreImmediateInstruction(StoreImmediateInstruction* instruction, FunctionRoutine* function)
    {
        MoveImmediateInstruction mov(OperandRegister::Scratch0, instruction->value, instruction->registerSize);
        writeMoveImmediateInstruction(&mov, function);
        uint32_t sf;
        uint16_t stackOffset;
        if (instruction->registerSize == RegisterSize::Quad)
        {
            sf = 1 << 30;
            stackOffset = instruction->stackOffset / 8;
        }
        else
        {
            sf = 0;
            stackOffset = instruction->stackOffset / 4;
        }
        bw->writeDoubleWordInFunction(
            Aarch64Instruction::StrImmediateUnsignedOffset | sf |
            uimm12(stackOffset) |
            Rn(Aarch64Register::sp) | Rt(getAarch64RegisterFromOperandRegister(OperandRegister::Scratch0)), function);
    }


    void
    Aarch64Writer::writeStoreRegisterInstruction(StoreRegisterInstruction* storeRegisterInstruction, FunctionRoutine* function)
    {
        writeSfInstructionInFunction(
            Aarch64Instruction::StrImmediateUnsignedOffset |
            uimm12(storeRegisterInstruction->stackOffset) |
            Rn(Aarch64Register::sp) |
            Rt(getAarch64RegisterFromOperandRegister(storeRegisterInstruction->target)), storeRegisterInstruction, function);
    }

    void
    Aarch64Writer::writeMoveAddressInstruction(MoveAddressToRegisterInstruction* moveAddressInstruction, FunctionRoutine* function)
    {
        output::String* string = std::get<output::String*>(moveAddressInstruction->constant);
        string->relocationAddress.value1 = _offset;
        bw->writeDoubleWordInFunction(Aarch64Instruction::Adrp | immhilo(0) | Rd(Aarch64Register::r0), function);
        string->relocationAddress.offset = _offset;
        writeAddImmediate64(Aarch64Register::r0, Aarch64Register::r0, 0, function);
        _strings.add(string);
    }


    void
    Aarch64Writer::writeMoveRegisterInstruction(MoveRegisterToRegisterInstruction* moveRegisterInstruction, FunctionRoutine* function)
    {
        if (moveRegisterInstruction->destination == OperandRegister::Return && moveRegisterInstruction->target == OperandRegister::Scratch0)
        {
            return;
        }
        writeSfInstructionInFunction(
            Aarch64Instruction::AddShiftedRegister |
            shift(Shift::LSL) |
            Rm(getAarch64RegisterFromOperandRegister(moveRegisterInstruction->target)) |
            uimm6(0) |
            Rn(getAarch64RegisterFromOperandRegister(moveRegisterInstruction->target)) |
            Rt(getAarch64RegisterFromOperandRegister(moveRegisterInstruction->destination)),
            moveRegisterInstruction, function);
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
        uint16_t stackOffset;
        uint32_t sf;
        if (loadInstruction->registerSize == RegisterSize::Quad)
        {
            sf = 1 << 30;
            stackOffset = loadInstruction->stackOffset / 8;
        }
        else
        {
            sf = 0;
            stackOffset = loadInstruction->stackOffset / 4;
        }
        writeSfInstructionInFunction(
            Aarch64Instruction::LdrImmediateUnsignedOffset | sf |
            uimm12(stackOffset) |
            Rn(Aarch64Register::sp) |
            Rt(getAarch64RegisterFromOperandRegister(loadInstruction->destination)), loadInstruction, function);
    }


    int
    Aarch64Writer::getAarch64RegisterFromOperandRegister(OperandRegister operandRegister) const
    {
        switch (operandRegister)
        {
            case OperandRegister::Return:
                return Aarch64Register::r0;
            case OperandRegister::Arg0:
                return Aarch64Register::r0;
            case OperandRegister::Arg1:
                return Aarch64Register::r1;
            case OperandRegister::Arg2:
                return Aarch64Register::r2;
            case OperandRegister::Arg3:
                return Aarch64Register::r3;
            case OperandRegister::Scratch0:
                return Aarch64Register::r0;
            case OperandRegister::Scratch1:
                return Aarch64Register::r1;
            case OperandRegister::Scratch2:
                return Aarch64Register::r2;
            default:
                throw std::runtime_error("Cannot get aarch64 register.");
        }
    }


    bool
    Aarch64Writer::moveWideIsPreferred(uint8_t sf, uint8_t immN, uint8_t imms, uint8_t immr) const
    {
        uint8_t S = imms;
        uint8_t R = immr;
        uint8_t width = sf == 1 ? 64 : 32;

        if (sf == 1 && immN != 1)
        {
            return false;
        }
        if (sf == 0 && !(immN == 0 && (imms & 0b00100000)))
        {
            return false;
        }
        if (S < 16)
        {
            return (-R % 16) <= (15 - S);
        }
        if (S >= width - 15)
        {
            return (R % 16) <= (S - (width - S));
        }
        return false;
    }


    bool
    Aarch64Writer::processLogicalImmediate(uint64_t imm, const Aarch64Register& rd, RegisterSize registerSize, MoveImmediateInstruction* instruction, output::FunctionRoutine* function)
    {
        // Ensures that it is not only zero bit pattern or only one bit pattern.
        if (imm == 0ULL || imm == ~0ULL)
        {
            return false;
        }
        if ((registerSize != RegisterSize::Quad) &&
            (imm >> (static_cast<int>(registerSize) * 8) != 0 || imm == (~0ULL >> ((static_cast<int>(RegisterSize::Quad) - static_cast<int>(registerSize)) * 8))))
        {
            return false;
        }

        // Decide on element elementSize
        unsigned int elementSize = static_cast<int>(registerSize) * 8;
        do
        {
            elementSize /= 2;
            uint64_t mask = (1ULL << elementSize) - 1;

            if ((imm & mask) != ((imm >> elementSize) & mask))
            {
                elementSize *= 2;
                break;
            }
        }
        while (elementSize > 2);

        if (elementSize == static_cast<int>(registerSize) * 8)
        {
            return false;
        }

        // Determine the rotation
        uint32_t trailingOnesCount;
        uint32_t trailingZeroesCount;
        uint64_t mask = ((uint64_t)-1ULL) >> (static_cast<int>(RegisterSize::Quad) * 8 - elementSize);
        imm &= mask;

        if (Bit::isShiftedMask64(imm))
        {
            trailingZeroesCount = Bit::countTrailingZeroes(imm);
            trailingOnesCount = Bit::countTrailingOnes(imm >> trailingZeroesCount);
        }
        else
        {
            imm |= ~mask;
            if (!Bit::isShiftedMask64(~imm))
            {
                return false;
            }
            uint32_t leadingOnesCount = Bit::countLeadingOnes(imm);
            trailingZeroesCount = static_cast<int>(RegisterSize::Quad) * 8 - leadingOnesCount;
            trailingOnesCount = leadingOnesCount + Bit::countTrailingOnes(imm) - (static_cast<int>(RegisterSize::Quad) * 8 - elementSize);
        }

        uint64_t immr = (elementSize - trailingZeroesCount) & (elementSize - 1);
        uint64_t Nimms = ~(elementSize - 1) << 1;
        Nimms |= (trailingOnesCount - 1);
        uint64_t N = ((Nimms >> 6) & 1) ^ 1;
        uint32_t logicalBitmask = (N << 22) | (immr << 16) | ((Nimms & 0x3f) << 10);

        uint32_t instructionBytes = Aarch64Instruction::MovBitmaskImmediate | logicalBitmask | Rn(Aarch64Register::r31) | rd;
        if (registerSize == RegisterSize::Quad)
        {
            instructionBytes |= Aarch64Instruction::sf;
        }
        bw->writeDoubleWordInFunction(instructionBytes, function);
        return true;
    }


    void
    Aarch64Writer::writeInstruction(uint32_t instruction, uint64_t value, FunctionRoutine* function)
    {
        if (value <= UINT32_MAX)
        {
            bw->writeInstructionInFunction(instruction, function);
        }
        else if (value <= UINT64_MAX)
        {
            bw->writeInstructionInFunction(Aarch64Instruction::sf | instruction, function);
        }
        else
        {
            throw std::runtime_error("Value is too large in writeInstruction.");
        }
    }

    uint32_t
    Aarch64Writer::shift(Shift shift)
    {
        return static_cast<uint32_t>(shift) << 22;
    }

    void
    Aarch64Writer::writeSfInstructionInFunction(uint32_t instruction, Instruction* referenceInstruction, FunctionRoutine* function) const
    {
        uint32_t sf = referenceInstruction->registerSize == RegisterSize::Quad ? 1 << 31 : 0;
        bw->writeDoubleWordInFunction(instruction | sf, function);
    }
}