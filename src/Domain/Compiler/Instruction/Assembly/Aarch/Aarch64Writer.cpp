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
            bw->writeDoubleWordInFunction(Aarch64Instruction::StpOffset64 | simm7(function->stackSize - 16) | Rt2(Aarch64Register::lr) | Rn(Aarch64Register::sp) | Rt(Aarch64Register::fp), function);
            writeAddImmediate64(Aarch64Register::fp, Aarch64Register::sp, 16, function);
        }
    }


    void
    Aarch64Writer::writeFunctionEpilogue(FunctionRoutine* function)
    {
        if (function->inferReturnZero)
        {
            bw->writeDoubleWordInFunction(Aarch64Instruction::Movz | Rd(Aarch64Register::r0) | imm16(0), function);
        }
        if (function->stackSize == 16)
        {
            bw->writeDoubleWordInFunction(Aarch64Instruction::LdpPostIndex64 | simm7(16) | Rt2(Aarch64Register::lr) | Rn(Aarch64Register::sp) | Rt(Aarch64Register::fp), function);
        }
        else
        {
            bw->writeDoubleWordInFunction(Aarch64Instruction::LdpOffset64 | simm7(function->stackSize - 16) | Rt2(Aarch64Register::lr) | Rn(Aarch64Register::sp) | Rt(Aarch64Register::fp), function);
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
    Aarch64Writer::imm12(uint16_t value) const
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
    Aarch64Writer::imm16(uint16_t value) const
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
    Aarch64Writer::writeAddImmediate64(
        Aarch64Register rd,
        Aarch64Register rn,
        int16_t value,
        output::FunctionRoutine* function)
    {
        bw->writeDoubleWordInFunction(Aarch64Instruction::AddImmediate64 | imm12(value) | Rn(rn) | Rd(rd), function);
    }


    void
    Aarch64Writer::writeSubImmediate64(
        Aarch64Register rd,
        Aarch64Register rn,
        int16_t value,
        output::FunctionRoutine* function)
    {
        bw->writeDoubleWordInFunction(Aarch64Instruction::SubImmediate64 | imm12(value) | Rn(rn) | Rd(rd), function);
    }


    void
    Aarch64Writer::writeAddRegisterToRegisterInstruction(
        AddRegisterToRegisterInstruction* instruction,
        FunctionRoutine* function)
    {
        writeSfInstructionInFunction(
            Aarch64Instruction::AddShiftedRegister |
            shift(Shift::LSL) |
            Rm(getAarch64RegisterFromOperandRegister(instruction->value)) |
            uimm6(0) |
            Rn(getAarch64RegisterFromOperandRegister(instruction->target)) |
            Rt(getAarch64RegisterFromOperandRegister(instruction->destination)),
            instruction, function);
    }


    void
    Aarch64Writer::writeSubtractRegisterToRegisterInstruction(
        SubtractRegisterToRegisterInstruction* instruction,
        FunctionRoutine* function)
    {
        writeSfInstructionInFunction(
            Aarch64Instruction::SubShiftedRegister |
            shift(Shift::LSL) |
            Rm(getAarch64RegisterFromOperandRegister(instruction->value)) |
            uimm6(0) |
            Rn(getAarch64RegisterFromOperandRegister(instruction->target)) |
            Rt(getAarch64RegisterFromOperandRegister(instruction->destination)),
            instruction, function);
    }



    void
    Aarch64Writer::writeMultiplySignedRegisterToRegisterInstruction(
        MultiplySignedRegisterToRegisterInstruction* instruction,
        FunctionRoutine* function)
    {
        writeSfInstructionInFunction(
            Aarch64Instruction::Madd |
            Rm(getAarch64RegisterFromOperandRegister(instruction->value)) |
            Ra(Aarch64Register::zero) |
            Rn(getAarch64RegisterFromOperandRegister(instruction->target)) |
            Rd(getAarch64RegisterFromOperandRegister(instruction->destination)),
            instruction, function);
    }


    void
    Aarch64Writer::writeDivideSignedRegisterToRegisterInstruction(
        DivideSignedRegisterToRegisterInstruction* instruction,
        FunctionRoutine* function)
    {
        writeSfInstructionInFunction(
            Aarch64Instruction::Sdiv |
            Rm(getAarch64RegisterFromOperandRegister(instruction->value)) |
            Rn(getAarch64RegisterFromOperandRegister(instruction->target)) |
            Rd(getAarch64RegisterFromOperandRegister(instruction->destination)),
            instruction, function);
    }



    void
    Aarch64Writer::writeDivideUnsignedRegisterToRegisterInstruction(
        DivideUnsignedRegisterToRegisterInstruction* instruction,
        FunctionRoutine* function)
    {
        writeSfInstructionInFunction(
            Aarch64Instruction::Udiv |
            Rm(getAarch64RegisterFromOperandRegister(instruction->value)) |
            Rn(getAarch64RegisterFromOperandRegister(instruction->target)) |
            Rd(getAarch64RegisterFromOperandRegister(instruction->destination)),
            instruction, function);
    }


    void
    Aarch64Writer::writeModuloUnsignedRegisterToRegisterInstruction(
        ModuloUnsignedRegisterToRegisterInstruction* instruction,
        FunctionRoutine* function)
    {
        writeSfInstructionInFunction(
            Aarch64Instruction::Udiv |
            Rm(getAarch64RegisterFromOperandRegister(instruction->value)) |
            Rn(getAarch64RegisterFromOperandRegister(instruction->target)) |
            Rd(getAarch64RegisterFromOperandRegister(instruction->divisionDestinationRegister)),
            instruction, function);

        writeSfInstructionInFunction(
            Aarch64Instruction::Msub |
            Rn(getAarch64RegisterFromOperandRegister(instruction->divisionDestinationRegister)) |
            Rm(getAarch64RegisterFromOperandRegister(instruction->target)) |
            Ra(getAarch64RegisterFromOperandRegister(instruction->value)) |
            Rd(getAarch64RegisterFromOperandRegister(instruction->destination)),
            instruction, function);
    }


    void
    Aarch64Writer::writeModuloSignedRegisterToRegisterInstruction(
        ModuloSignedRegisterToRegisterInstruction* instruction,
        FunctionRoutine* function)
    {
        writeSfInstructionInFunction(
            Aarch64Instruction::Sdiv |
            Rm(getAarch64RegisterFromOperandRegister(instruction->value)) |
            Rn(getAarch64RegisterFromOperandRegister(instruction->target)) |
            Rd(getAarch64RegisterFromOperandRegister(instruction->divisionDestinationRegister)),
            instruction, function);

        writeSfInstructionInFunction(
            Aarch64Instruction::Msub |
            Rn(getAarch64RegisterFromOperandRegister(instruction->divisionDestinationRegister)) |
            Rm(getAarch64RegisterFromOperandRegister(instruction->target)) |
            Ra(getAarch64RegisterFromOperandRegister(instruction->value)) |
            Rd(getAarch64RegisterFromOperandRegister(instruction->destination)),
            instruction, function);
    }


    void
    Aarch64Writer::writeMoveImmediateInstruction(
        MoveImmediateInstruction* instruction,
        FunctionRoutine* function)
    {
        Aarch64Register rd = static_cast<Aarch64Register>(Rd(getAarch64RegisterFromOperandRegister(instruction->destination)));
        uint64_t value = instruction->value;
        if (value <= UINT16_MAX)
        {
            return writeSfInstructionInFunction(Aarch64Instruction::Movz | imm16(value) | rd, instruction, function);
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
    Aarch64Writer::writeNegatedOrRegularShiftMoves(
        uint64_t value,
        Aarch64Register rd,
        MoveImmediateInstruction* instruction,
        FunctionRoutine* function)
    {
        if (tryWriteSingleMovInstruction(value, rd, instruction, function))
        {
            return;
        }
        if (tryWriteMovnWithMovkInstruction(value, rd, instruction, function))
        {
            return;
        }
        writePositiveValuesKeepInstructions(value, rd, instruction, function);
    }


    bool
    Aarch64Writer::tryWriteSingleMovInstruction(
        uint64_t value,
        const Aarch64Register& rd,
        MoveImmediateInstruction* instruction,
        FunctionRoutine* function)
    {
        if (tryWriteSingleMovzInstruction(value, rd, instruction, function))
        {
            return true;
        }
        return tryWriteSingleMovBitmaskImmediate(value, rd, instruction, function);
    }


    bool
    Aarch64Writer::tryWriteSingleMovBitmaskImmediate(
        uint64_t value,
        const Aarch64Register& rd,
        MoveImmediateInstruction* instruction,
        FunctionRoutine* function)
    {
        uint32_t logicalBitmask;
        if (tryGetBitmaskImmediate(value, logicalBitmask, instruction->destinationSize))
        {
            writeSfInstructionInFunction(
                MovBitmaskImmediate |
                logicalBitmask |
                Rn(r31) |
                rd,
                instruction, function);
            return true;
        }
        return false;
    }


    bool
    Aarch64Writer::tryWriteSingleMovzInstruction(
        uint64_t value,
        Aarch64Register rd,
        const Instruction* instruction,
        FunctionRoutine* function) const
    {
        return tryWriteSingleMovzInstructionWithRegisterSize(
            value,
            rd,
            instruction->destinationSize,
            instruction,
            function);
    }


    bool
    Aarch64Writer::tryWriteSingleMovzInstructionWithRegisterSize(
        uint64_t value,
        Aarch64Register rd,
        RegisterSize registerSize,
        const Instruction* instruction,
        FunctionRoutine* function) const
    {
        int nonZeroWordCount = 0;
        uint32_t i = 0;
        uint32_t leftBitShifts = 0;
        uint32_t result_value;
        uint32_t result_leftBitShift;
        for (; i < static_cast<int>(getSupportedBinopSize(registerSize)); i += 2)
        {
            leftBitShifts = 8 * i;
            uint64_t wordBitmask = (1 << 16) - 1;
            uint64_t bitmask = wordBitmask << leftBitShifts;
            if ((value & bitmask) >> leftBitShifts)
            {
                result_value = (value & bitmask) >> leftBitShifts;
                result_leftBitShift = leftBitShifts;
                nonZeroWordCount++;
            }
        }
        if (nonZeroWordCount == 1)
        {
            writeSfInstructionInFunction(
                Aarch64Instruction::Movz |
                hw(result_leftBitShift) |
                imm16(result_value) |
                Rd(rd),
                instruction, function);
            return true;
        }
        return false;
    }


    void
    Aarch64Writer::writePositiveValuesKeepInstructions(
        uint64_t value,
        const Aarch64Register& rd,
        MoveImmediateInstruction* instruction,
        FunctionRoutine* function)
    {
        bool hasWrittenMov = false;
        uint64_t i = 0;
        if (instruction->destinationSize == RegisterSize::Quad)
        {
            if (tryWriteSingleMovzInstructionWithRegisterSize(value, rd, RegisterSize::Dword, instruction, function))
            {
                hasWrittenMov = true;
            }
            else
            {
                uint32_t logicalBitmask;
                bool hasBitmaskImmediate = tryGetBitmaskImmediate(0xffff'ffff & value, logicalBitmask,
                    RegisterSize::Dword);
                if (hasBitmaskImmediate)
                {
                    bw->writeDoubleWordInFunction(
                        Aarch64Instruction::MovBitmaskImmediate |
                        logicalBitmask |
                        Rn(Aarch64Register::r31) |
                        rd, function);

                    hasWrittenMov = true;
                }
            }
            if (hasWrittenMov)
            {
                i = 32;
            }
        }
        uint8_t registerSize = static_cast<uint8_t>(instruction->destinationSize) * 8;
        for (; i < registerSize; i += 16)
        {
            uint64_t elementValue = ((0xffffui64 << i) & value) >> i;
            if (!hasWrittenMov && elementValue != 0ui64)
            {
                writeSfInstructionInFunction(Aarch64Instruction::Movz | hw(i) | imm16(elementValue) | rd, instruction, function);
                hasWrittenMov = true;
                continue;
            }
            if (elementValue != 0)
            {
                writeSfInstructionInFunction(Aarch64Instruction::Movk | imm16(elementValue) | hw(i) | rd, instruction, function);
            }
        }

        // We can only reach here if value == 0.
        if (!hasWrittenMov)
        {
            writeSfInstructionInFunction(Aarch64Instruction::Movz | hw(0) | imm16(0) | rd, instruction, function);
        }
    }


    bool
    Aarch64Writer::tryWriteMovnWithMovkInstruction(uint64_t value, const Aarch64Register& rd, MoveImmediateInstruction* instruction, output::FunctionRoutine* function)
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
        uint8_t registerSize = static_cast<int>(instruction->destinationSize) * 8;
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
        writeSfInstructionInFunction(Aarch64Instruction::Movn | hw(negationIndex) | imm16(elementValue) | rd, instruction, function);
        for (int i = 0; i < negationIndex; i += 16)
        {
            elementValue = ((0xffffui64 << i) & value) >> i;
            if (elementValue != 0xffff)
            {
                writeSfInstructionInFunction(Aarch64Instruction::Movk | imm16(elementValue) | hw(i) | rd, instruction, function);
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
            uint32_t dw = bw->getDoubleWord(textSegmentStartOffset + string->relocationAddress.offset) |
                imm12((textSegmentStartOffset + string->relocationAddress.value2) - numberOfPageSizes);
            bw->writeDoubleWordAtAddress(dw, textSegmentStartOffset + string->relocationAddress.offset);
        }
    }


    void
    Aarch64Writer::relocateStub(
        uint64_t offset,
        uint64_t textSegmentStartOffset,
        ExternalRoutine* externalRoutine)
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
    Aarch64Writer::writeB(
        int32_t offset)
    {
        bw->writeDoubleWord(Aarch64Instruction::B | simm26(offset / 4));
    }


    void
    Aarch64Writer::writeStoreImmediateInstruction(
        StoreImmediateInstruction* instruction,
        FunctionRoutine* function)
    {
        MoveImmediateInstruction mov(OperandRegister::Scratch0, instruction->value, instruction->destinationSize);
        writeMoveImmediateInstruction(&mov, function);
        if (isAtLeastDword(instruction))
        {
            writeStoreImmediateUnsignedOffsetAtLeastDword(
                instruction,
                OperandRegister::Scratch0,
                function);
        }
        else if (instruction->destinationSize == RegisterSize::Byte)
        {
            bw->writeDoubleWordInFunction(Aarch64Instruction::StrbImmediateUnsignedOffset |
                imm12(instruction->stackOffset) |
                Rn(Aarch64Register::sp) |
                Rt(getAarch64RegisterFromOperandRegister(OperandRegister::Scratch0)), function);
        }
        else if (instruction->destinationSize == RegisterSize::Word)
        {
            bw->writeDoubleWordInFunction(Aarch64Instruction::StrhImmediateUnsignedOffset |
                imm12(instruction->stackOffset) |
                Rn(Aarch64Register::sp) |
                Rt(getAarch64RegisterFromOperandRegister(OperandRegister::Scratch0)), function);
        }
    }


    void
    Aarch64Writer::writeStoreImmediateUnsignedOffsetAtLeastDword(
        const StoreImmediateInstruction* instruction,
        OperandRegister destination,
        FunctionRoutine* function)
    {
        uint32_t sf;
        uint16_t stackOffset;
        if (instruction->destinationSize == RegisterSize::Quad)
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
            StrImmediateUnsignedOffset | sf |
            imm12(stackOffset) |
            Rn(sp) | Rt(getAarch64RegisterFromOperandRegister(destination)), function);
    }


    void
    Aarch64Writer::writeStoreRegisterInstruction(
        StoreRegisterInstruction* storeRegisterInstruction,
        FunctionRoutine* function)
    {
        writeSfInstructionInFunction(
            Aarch64Instruction::StrImmediateUnsignedOffset |
            imm12(storeRegisterInstruction->stackOffset) |
            Rn(Aarch64Register::sp) |
            Rt(getAarch64RegisterFromOperandRegister(storeRegisterInstruction->target)), storeRegisterInstruction, function);
    }


    void
    Aarch64Writer::writeMoveAddressInstruction(
        MoveAddressToRegisterInstruction* moveAddressInstruction,
        FunctionRoutine* function)
    {
        output::String* string = std::get<output::String*>(moveAddressInstruction->constant);
        string->relocationAddress.value1 = _offset;
        bw->writeDoubleWordInFunction(Aarch64Instruction::Adrp | immhilo(0) | Rd(Aarch64Register::r0), function);
        string->relocationAddress.offset = _offset;
        writeAddImmediate64(Aarch64Register::r0, Aarch64Register::r0, 0, function);
        _strings.add(string);
    }


    void
    Aarch64Writer::writeMoveZeroExtendInstruction(
        MoveZeroExtendInstruction* instruction,
        FunctionRoutine* function)
    {
        uint32_t bitmaskImmediate;
        if (!tryGetBitmaskImmediate(
            getRegisterSizeBitmask(instruction->targetSize),
            bitmaskImmediate,
            getSupportedBinopSize(instruction->destinationSize)))
        {
            throw std::runtime_error("Could not get bitmask immediate");
        }
        writeSfInstructionInFunction(
            Aarch64Instruction::AndImmediate |
            bitmaskImmediate |
            Rn(getAarch64RegisterFromOperandRegister(instruction->target)) |
            Rd(getAarch64RegisterFromOperandRegister(instruction->destination)),
            instruction, function);
    }


    void
    Aarch64Writer::writeMoveSignExtendInstruction(MoveSignExtendInstruction* instruction, FunctionRoutine* function)
    {
        uint32_t bitmaskImmediate;
        if (!tryGetBitmaskImmediate(
            getRegisterSizeBitmask(instruction->targetSize),
            bitmaskImmediate,
            getSupportedBinopSize(instruction->destinationSize)))
        {
            throw std::runtime_error("Could not get bitmask immediate");
        }
        uint32_t sf = instruction->destinationSize == RegisterSize::Quad ? 1 << 31 : 0;
        uint32_t N = instruction->destinationSize == RegisterSize::Quad ? 1 << 22 : 0;
        bw->writeDoubleWordInFunction(
            Aarch64Instruction::AndImmediate |
            sf |
            bitmaskImmediate |
            Rn(getAarch64RegisterFromOperandRegister(instruction->target)) |
            Rd(getAarch64RegisterFromOperandRegister(instruction->destination)),
            function);
        if (instruction->targetSize == RegisterSize::Byte)
        {
            bw->writeDoubleWordInFunction(
                Aarch64Instruction::Sxtb |
                sf | N |
                Rn(getAarch64RegisterFromOperandRegister(instruction->target)) |
                Rd(getAarch64RegisterFromOperandRegister(instruction->destination)), function);
        }
        else if (instruction->targetSize == RegisterSize::Word)
        {
            bw->writeDoubleWordInFunction(
                Aarch64Instruction::Sxth |
                sf | N |
                Rn(getAarch64RegisterFromOperandRegister(instruction->target)) |
                Rd(getAarch64RegisterFromOperandRegister(instruction->destination)), function);
        }
        else
        {
            assert("Larger the word size is not supported yet.");
        }
    }


    uint32_t
    Aarch64Writer::getRegisterSizeBitmask(
        RegisterSize size)
    {
        switch (size)
        {
            case RegisterSize::Dword:
                return 0xffff'ffff;
            case RegisterSize::Word:
                return 0xffff;
            case RegisterSize::Byte:
                return 0xff;
        }
        assert("Unknown register size.");
    }


    void
    Aarch64Writer::writeMoveRegisterToRegisterInstruction(
        MoveRegisterToRegisterInstruction* moveRegisterInstruction,
        FunctionRoutine* function)
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
    Aarch64Writer::writeLoadUnsignedInstruction(
        LoadUnsignedInstruction* instruction,
        FunctionRoutine* function)
    {
        uint16_t stackOffset;
        uint32_t sf;
        if (isAtLeastDword(instruction))
        {
            if (instruction->destinationSize == RegisterSize::Quad)
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
                Aarch64Instruction::LdrImmediateUnsignedOffset | sf |
                imm12(stackOffset) |
                Rn(Aarch64Register::sp) |
                Rt(getAarch64RegisterFromOperandRegister(instruction->destination)), function);
        }
        else if (instruction->destinationSize == RegisterSize::Byte)
        {
            bw->writeDoubleWordInFunction(Aarch64Instruction::LdrbImmediateUnsignedOffset |
                imm12(instruction->stackOffset) |
                Rn(Aarch64Register::sp) |
                Rt(getAarch64RegisterFromOperandRegister(instruction->destination)), function);
        }
        else // Word
        {
            bw->writeDoubleWordInFunction(Aarch64Instruction::LdrhImmediateUnsignedOffset |
                imm12(instruction->stackOffset) |
                Rn(Aarch64Register::sp) |
                Rt(getAarch64RegisterFromOperandRegister(instruction->destination)), function);
        }
    }


    void
    Aarch64Writer::writeLoadSignedInstruction(
        LoadSignedInstruction* instruction,
        FunctionRoutine* function)
    {
        uint16_t stackOffset;
        uint32_t sf;
        if (isAtLeastDword(instruction))
        {
            if (instruction->destinationSize == RegisterSize::Quad)
            {
                sf = 1 << 30;
                stackOffset = instruction->stackOffset / 8;
            }
            else
            {
                sf = 0;
                stackOffset = instruction->stackOffset / 4;
            }

            // Note aarch64 stores them as 64 bit, so no need for dedicated signed load.
            bw->writeDoubleWordInFunction(
                Aarch64Instruction::LdrImmediateUnsignedOffset | sf |
                imm12(stackOffset) |
                Rn(Aarch64Register::sp) |
                Rt(getAarch64RegisterFromOperandRegister(instruction->destination)), function);
        }
        else if (instruction->destinationSize == RegisterSize::Byte)
        {
            uint32_t opc = 1 << 22;
            bw->writeDoubleWordInFunction(Aarch64Instruction::LdrsbImmediateUnsignedOffset |
            opc |
            imm12(instruction->stackOffset) |
            Rn(Aarch64Register::sp) |
            Rt(getAarch64RegisterFromOperandRegister(instruction->destination)), function);
        }
        else // Word
        {
            uint32_t opc = 1 << 22;
            bw->writeDoubleWordInFunction(Aarch64Instruction::LdrshImmediateUnsignedOffset |
            opc |
            imm12(instruction->stackOffset) |
            Rn(Aarch64Register::sp) |
            Rt(getAarch64RegisterFromOperandRegister(instruction->destination)), function);
        }
    }


    int
    Aarch64Writer::getAarch64RegisterFromOperandRegister(
        OperandRegister operandRegister) const
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
    Aarch64Writer::moveWideIsPreferred(
        uint8_t sf,
        uint8_t immN,
        uint8_t imms,
        uint8_t immr) const
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
    Aarch64Writer::tryGetBitmaskImmediate(
        uint64_t imm,
        uint32_t& logicalImmediate,
        RegisterSize registerSize)
    {
        // Ensures that it is not only zero bit pattern or only one bit pattern.
        if (imm == 0ULL || imm == ~0ULL)
        {
            return false;
        }
        if ((registerSize != RegisterSize::Quad) &&

            // Value larger than registerSize
            (imm >> (static_cast<int>(registerSize) * 8) != 0 ||

                // Value equals all 1s.
                imm == (~0ULL >> ((static_cast<int>(RegisterSize::Quad) - static_cast<int>(registerSize)) * 8))))
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
        // Cannot fit Nimms
        if (elementSize == 64 || trailingOnesCount > 32)
        {
            return false;
        }
        // Cannot fit immr
        if (elementSize - trailingZeroesCount > 32)
        {
            return false;
        }

        uint64_t immr = (elementSize - trailingZeroesCount) & (elementSize - 1);
        uint64_t Nimms = ~(elementSize - 1) << 1;
        Nimms |= (trailingOnesCount - 1);
        uint64_t N = ((Nimms >> 6) & 1) ^ 1;
        logicalImmediate = (N << 22) | (immr << 16) | ((Nimms & 0x3f) << 10);
        return true;
    }


    uint32_t
    Aarch64Writer::shift(Shift shift)
    {
        return static_cast<uint32_t>(shift) << 22;
    }


    void
    Aarch64Writer::writeSfInstructionInFunction(
        uint32_t instruction,
        const Instruction* referenceInstruction,
        FunctionRoutine* function) const
    {
        uint32_t sf = referenceInstruction->destinationSize == RegisterSize::Quad ? 1 << 31 : 0;
        bw->writeDoubleWordInFunction(instruction | sf, function);
    }


    inline
    bool
    Aarch64Writer::isAtLeastDword(Instruction* instruction) const
    {
        return instruction->destinationSize >= RegisterSize::Dword;
    }


    RegisterSize
    Aarch64Writer::getSupportedBinopSize(RegisterSize registerSize) const
    {
        switch (registerSize)
        {
            case RegisterSize::Quad:
            case RegisterSize::Dword:
                return registerSize;
            default:
                return RegisterSize::Dword;
        }
    }
}