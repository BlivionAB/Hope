#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-magic-numbers"
#pragma ide diagnostic ignored "cppcoreguidelines-pro-bounds-pointer-arithmetic"
#include "MachoFileWriter.h"
#include "DyldInfoWriter.h"
#include "../Assembly/Aarch/Aarch64Writer.h"


namespace elet::domain::compiler::instruction::output
{


MachoFileWriter::MachoFileWriter(AssemblyTarget assemblyTarget):
    _assemblyTarget(assemblyTarget)
{
    _bw = new ByteWriter(&output, &offset);
    switch (assemblyTarget)
    {
        case AssemblyTarget::x86_64:
            assemblyWriter = new X86_64Writer(&_text);
            break;
        case AssemblyTarget::AArch64:
            assemblyWriter = new Aarch64Writer(&_text);
            break;
    }
    _dyldInfoWriter = new DyldInfoWriter(this);
}


List<uint8_t>*
MachoFileWriter::write(FunctionRoutine* startRoutine)
{
    layoutTextSegment(startRoutine);
    output.reserve(100 * 4096);

    writeHeader();
    writePageZeroSegmentCommand();
    writeTextSegmentCommand();
    writeDataConstSegmentCommand();
    writeDataSegmentCommand();
    writeLinkEditSegmentCommand();
    writeDyldInfoSegmentCommand();
    writeSymtabCommand();
    writeDysymTabCommand();
    writeLoadDylinkerCommand();
    writeUuidCommand();
    writeMainCommand(startRoutine);
    writeLoadDylibCommands();

    writeTextSegment();
    writeDataConstSegment();
    writeDataSegment();
    writeDyldInfo();
    writeSymbolTable();
    writeIndirectSymbolTable();
    writeStringTable();

    return assemblyWriter->getOutput();
}


void
MachoFileWriter::writeTextSegment()
{
    size_t rest = _textSegmentSize % 16;
    size_t modSize = _textSegmentSize - rest;
    if (rest != 0)
    {
        modSize += 16;
    }
    size_t padding = _modTextSegmentSize - modSize;
    size_t newVmAddress = padding + vmAddress;

    writePadding(padding - offset);
    textSegmentStartOffset = offset;
    _textSegmentStartVmAddress = newVmAddress;
    _mainCommand->offset = offset;
    output.add(_text);
    assemblyWriter->bw = _bw;
    relocateCStrings(textSegmentStartOffset);
    offset += _textSegmentSize;
    writePadding(modSize - _textSegmentSize);

    _textSection->address += newVmAddress;
    _textSection->offset += padding;
    _stubsSection->address += newVmAddress;
    _stubsSection->offset += padding;
    _stubHelperSection->address += newVmAddress;
    _stubHelperSection->offset += padding;
    _cstringSection->address += newVmAddress;
    _cstringSection->offset += padding;

    _textSegment->fileSize = _modTextSegmentSize;
    _textSegment->vmSize = _modTextSegmentSize;

    vmAddress += _modTextSegmentSize;
}


void
MachoFileWriter::layoutTextSegment(FunctionRoutine* startRoutine)
{
    writeTextSection(startRoutine);
    writeStubsSection();
    writeStubHelperSection();
    writeCStringSection();

    // Note, each segment needs to be 8 byte aligned.
    size_t currentOffset = assemblyWriter->getOffset();
    _textSegmentSize = currentOffset - _textOffset;
    _modTextSegmentSize = (_textSegmentSize / SEGMENT_SIZE);
    if ((_textSegmentSize % SEGMENT_SIZE) != 0)
    {
        _modTextSegmentSize += SEGMENT_SIZE;
    }
}


void
MachoFileWriter::writeTextSection(FunctionRoutine* startRoutine)
{
    _textOffset = assemblyWriter->getOffset();
    assemblyWriter->writeTextSection(startRoutine);
    _textSize = assemblyWriter->getOffset() - _textOffset;
}


void
MachoFileWriter::writeStubsSection()
{
    _stubsOffset = assemblyWriter->getOffset();
    size_t rest = _stubsOffset % 2;
    if (rest != 0)
    {
        assemblyWriter->writePadding(rest);
        _stubsOffset = assemblyWriter->getOffset();
    }
    assemblyWriter->writeStubs();
    _stubsSize = assemblyWriter->getOffset() - _stubsOffset;
}


void
MachoFileWriter::writeStubHelperSection()
{
    _stubHelperOffset = assemblyWriter->getOffset();
    size_t rest = _stubHelperOffset % 16;
    if (rest != 0)
    {
        assemblyWriter->writePadding(16 - rest);
        _stubHelperOffset = assemblyWriter->getOffset();
    }
    assemblyWriter->writeStubHelper();
    _stubHelperSize = assemblyWriter->getOffset() - _stubHelperOffset;
}


void
MachoFileWriter::writeCStringSection()
{
    _stringOffset = assemblyWriter->getOffset();
    assemblyWriter->writeCStringSection();
    _stringSize = assemblyWriter->getOffset() - _stringOffset;
}


void
MachoFileWriter::writeTextSegmentCommand()
{
    _textSegment = writeSegment({
        LcSegment64,
        sizeof(SegmentCommand64),
        "__TEXT",
        vmAddress,
        0,
        0,
        0,
        VM_PROTECTION_READ | VM_PROTECTION_EXECUTE,
        VM_PROTECTION_READ | VM_PROTECTION_EXECUTE,
        0,
        0,
    });

    _textSection = writeSection({
        "__text",
        "__TEXT",
        0,
        _textSize,
        0,
        4,
        0,
        0,
        MachoSectionFlags::S_ATTR_PURE_INSTRUCTIONS | MachoSectionFlags::S_ATTR_SOME_INSTRUCTIONS,
        0,
        0,
        0
    }, _textSegment);

    _stubsSection = writeSection({
         "__stubs",
         "__TEXT",
         static_cast<uint64_t>(_stubsOffset),
         _stubsSize,
         _stubsOffset,
         1,
         0,
         0,
         MachoSectionFlags::S_SYMBOL_STUBS | MachoSectionFlags::S_ATTR_PURE_INSTRUCTIONS | MachoSectionFlags::S_ATTR_SOME_INSTRUCTIONS,
         0,
         6, /*byte size of each stub entry*/
         0
    }, _textSegment);

    _stubHelperSection = writeSection({
        "__stub_helper",
        "__TEXT",
        static_cast<uint64_t>(_stubHelperOffset),
        _stubHelperSize,
        _stubHelperOffset,
        2,
        0,
        0,
        MachoSectionFlags::S_ATTR_PURE_INSTRUCTIONS | MachoSectionFlags::S_ATTR_SOME_INSTRUCTIONS,
        0,
        0, /*number of stubs*/
        0
    }, _textSegment);

    _cstringSection = writeSection({
        "__cstring",
        "__TEXT",
        static_cast<uint64_t>(_stringOffset),
        _stringSize,
        _stringOffset,
        0,
        0,
        0,
        MachoSectionFlags::S_CSTRING_LITERALS,
        0,
        0,
        0
    }, _textSegment);
}


void
MachoFileWriter::writeHeader()
{
    MachoCpuType cpuType;
    MachoCpuType cpuSubType;
    switch (_assemblyTarget)
    {
        case AssemblyTarget::x86_64:
            cpuType = MachoCpuType::CPU_TYPE_X86_64;
            cpuSubType = MachoCpuType::CPU_SUBTYPE_X86_64_ALL;
            break;
        case AssemblyTarget::AArch64:
            cpuType = MachoCpuType::CPU_TYPE_ARM64;
            cpuSubType = MachoCpuType::CPU_SUBTYPE_ARM64_ALL;
            break;
        default:
            throw std::runtime_error("Unknown assembly target.");
    }
    MachHeader64 header {
        .magic = MachoMagicValue::MACHO_MAGIC_64,
        .cpuType = cpuType,
        .cpuSubType = cpuSubType,
        .fileType = MH_EXECUTE,
        .numberOfCommands = 0,
        .sizeOfCommands = 0,
        .flags = MachoFlags::MH_NOUNDEFS | MachoFlags::MH_DYLDLINK | MachoFlags::MH_TWOLEVEL | MachoFlags::MH_PIE,
        .reserved = 0,
    };
    _header = output.write(&header);
    offset += sizeof(*_header);
}


void
MachoFileWriter::writePageZeroSegmentCommand()
{
    writeSegment({
        LcSegment64,
        sizeof(SegmentCommand64),
        "__PAGEZERO",
        0,
        0x0000000100000000,
        0,
        0,
        VmProtection::VM_PROTECTION_NONE,
        VmProtection::VM_PROTECTION_NONE,
        0,
        0
    });
}


SegmentCommand64*
MachoFileWriter::writeSegment(SegmentCommand64 segmentCommand)
{
    offset += sizeof(SegmentCommand64);
    _header->sizeOfCommands += sizeof(SegmentCommand64);
    _header->numberOfCommands++;
    return output.write(&segmentCommand);
}


Section64*
MachoFileWriter::writeSection(Section64 section, SegmentCommand64* segment)
{
    offset += sizeof(Section64);
    _header->sizeOfCommands += sizeof(Section64);
    segment->commandSize += sizeof(Section64);
    segment->numberOfSections++;
    return output.write(&section);
}


void
MachoFileWriter::writePadding(uint32_t padding)
{
    for (unsigned int i = 0; i < padding; ++i)
    {
        output.add(0);
    }
    offset += padding;
}


void
MachoFileWriter::writeDyldInfoSegmentCommand()
{
    DyldInfoCommand command = {
        .cmd = LC_DYLD_INFO_ONLY,
        .cmdSize = sizeof(DyldInfoCommand),
        .rebaseOffset = 0,
        .rebaseSize = 0,
        .bindOffset = 0,
        .bindSize = 0,
        .weakBindOffset = 0,
        .weakBindSize = 0,
        .lazyBindOffset = 0,
        .lazyBindSize = 0,
        .exportOffset = 0,
        .exportSize = 0,
    };
    _header->numberOfCommands++;
    _header->sizeOfCommands += sizeof(DyldInfoCommand);
    dyldInfoCommand = output.write<DyldInfoCommand>(&command);
    offset += sizeof(DyldInfoCommand);
}


void
MachoFileWriter::writeDyldInfo()
{
    linkEditSegment->fileOffset = offset;
    _dyldInfoWriter->write();
}


void
MachoFileWriter::writeLinkEditSegmentCommand()
{
    linkEditSegment = writeSegment({
        .command = LcSegment64,
        .commandSize = sizeof(SegmentCommand64),
        .segmentName = "__LINKEDIT",
        .vmAddress = vmAddress,
        .vmSize = 0x4000,
        .fileOffset = 0,
        .fileSize = 0x4000,
        .maximumProtection = VM_PROTECTION_READ,
        .initialProtection = VM_PROTECTION_READ,
        .numberOfSections = 0,
        .flags = 0,
    });
}


void
MachoFileWriter::writeDataConstSegmentCommand()
{
    _dataConstSegment = writeSegment({
        .command = LcSegment64,
        .commandSize = sizeof(SegmentCommand64),
        .segmentName = "__DATA_CONST",
        .vmAddress = 0,
        .vmSize = 0,
        .fileOffset = 0,
        .fileSize = 0,
        .maximumProtection = VM_PROTECTION_READ | VM_PROTECTION_WRITE,
        .initialProtection = VM_PROTECTION_READ | VM_PROTECTION_WRITE,
        .numberOfSections = 0,
        .flags = 0x00000010, // Don't know what this flag does? But, its in most binaries.
    });

    _dataConstGotSection = writeSection({
        .sectionName = "__got",
        .segmentName = "__DATA_CONST",
        .address = 0,
        .size = 0,
        .offset = 0,
        .align = 3,
        .relocationOffset = 0,
        .flags = S_NON_LAZY_SYMBOL_POINTERS,
        .reserved1 = 0, // Indirect symbol index, It's where all symbols begins
        .reserved2 = 0,
        .reserved3 = 0,
    }, _dataConstSegment);
}


void
MachoFileWriter::writeDataSegmentCommand()
{
    _dataSegment = writeSegment({
        .command = LcSegment64,
        .commandSize = sizeof(SegmentCommand64),
        .segmentName = "__DATA",
        .vmAddress = 0,
        .vmSize = 0,
        .fileOffset = 0,
        .fileSize = 0,
        .maximumProtection = VM_PROTECTION_READ | VM_PROTECTION_WRITE,
        .initialProtection = VM_PROTECTION_READ | VM_PROTECTION_WRITE,
        .numberOfSections = 0,
        .flags = 0, // Don't know what this flag does? But, its in most binaries.
    });

    _dataLaSymbolPtrSection = writeSection({
        .sectionName = "__la_symbol_ptr",
        .segmentName = "__DATA",
        .address = 0,
        .size = 0,
        .offset = 0,
        .align = 3,
        .relocationOffset = 0,
        .flags = S_LAZY_SYMBOL_POINTERS,
        .reserved1 = 0, // Indirect symbol index, It's where all symbols begins
        .reserved2 = 0,
        .reserved3 = 0,
    }, _dataSegment);

    _dataSection = writeSection({
        .sectionName = "__data",
        .segmentName = "__DATA",
        .address = 0,
        .size = 0,
        .offset = 0,
        .align = 3,
        .relocationOffset = 0,
        .flags = 0,
        .reserved1 = 0,
        .reserved2 = 0,
        .reserved3 = 0,
    }, _dataSegment);
}


void
MachoFileWriter::writeDataConstSegment()
{
    _dataConstSegment->vmAddress = vmAddress;
    _dataConstSegment->vmSize = SEGMENT_SIZE;
    _dataConstSegment->fileOffset = offset;
    _dataConstSegment->fileSize = SEGMENT_SIZE;

    _dataConstGotSection->address = vmAddress;
    _dataConstGotSection->size += 8;
    _dataConstGotSection->offset = offset;

    for (const auto& gotBoundRoutine : assemblyWriter->gotBoundRoutines)
    {
        for (const auto& relocationAddress : gotBoundRoutine->relocationAddresses)
        {
            _bw->writeDoubleWordAtAddress(offset - (textSegmentStartOffset + relocationAddress + 4), textSegmentStartOffset + relocationAddress);
        }
        _bw->writeQuadWord(0);
    }

    size_t rest = _dataConstGotSection->size % SEGMENT_SIZE;
    size_t padding = 0;
    if (rest != 0)
    {
        padding = SEGMENT_SIZE - rest;
    }
    writePadding(padding);
    vmAddress += SEGMENT_SIZE;
}


void
MachoFileWriter::writeDataSegment()
{
    _dataSegment->vmAddress = vmAddress;
    _dataSegment->vmSize = SEGMENT_SIZE;
    _dataSegment->fileOffset = offset;
    _dataSegment->fileSize = SEGMENT_SIZE;

    writeLaSymbolPtrSection();
    writeDataSection();

    uint64_t rest = offset % SEGMENT_SIZE;
    if (rest != 0)
    {
        uint64_t padding = SEGMENT_SIZE - rest;
        writePadding(padding);
        vmAddress += padding;
    }
}


void
MachoFileWriter::writeLaSymbolPtrSection()
{
    _dataLaSymbolPtrSection->address = vmAddress;
    _dataLaSymbolPtrSection->offset = offset;
    for (const auto& externalRoutine : assemblyWriter->externalRoutines)
    {
        // Write the offset in the stub address
        assemblyWriter->relocateStub(offset, textSegmentStartOffset, externalRoutine);
        _bw->writeQuadWord(_textSegmentStartVmAddress + externalRoutine->stubHelperAddress);
    }
    _dataLaSymbolPtrSection->size = offset - _dataLaSymbolPtrSection->offset;
    vmAddress += _dataLaSymbolPtrSection->size;
}


void
MachoFileWriter::writeDataSection()
{
    _dataSection->address = vmAddress;
    _dataSection->size = 8;
    _dataSection->offset = offset;

    _dyldPrivateVmAddress = vmAddress;
    assemblyWriter->relocateDyldPrivate(offset, textSegmentStartOffset);
    _bw->writeQuadWord(0);

    vmAddress += 8;
}


void
MachoFileWriter::writeDysymTabCommand()
{
    DysymTabCommand command =
    {
        .cmd = LC_DYSYMTAB,
        .cmdSize = sizeof(DysymTabCommand),
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
    };
    _header->numberOfCommands++;
    _header->sizeOfCommands += sizeof(DysymTabCommand);
    dysymTabCommand = output.write<DysymTabCommand>(&command);
    offset += sizeof(DysymTabCommand);
}


void
MachoFileWriter::writeSymtabCommand()
{
    SymtabCommand command =
    {
        .cmd = LcSymbtab,
        .cmdSize = sizeof(SymtabCommand),
        0,
        0,
        0,
        0,
    };
    _header->numberOfCommands++;
    _header->sizeOfCommands += sizeof(SymtabCommand);
    symtabCommand = output.write<SymtabCommand>(&command);
    offset += sizeof(SymtabCommand);
}


void
MachoFileWriter::writeLoadDylibCommands()
{
    LoadDylibCommand* command = writeCommand<LoadDylibCommand>(LC_LOAD_DYLIB);
    command->name = 24; // Offset to string
    command->timestamp = 2; // unix timestamp clang compiles this to 2
    command->currentVersion = 0x050c3c01; // 1292.60.1
    command->compatabilityVersion = 0x00010000; // 1.0.0
    _bw->writeString("/usr/lib/libSystem.B.dylib");
    size_t stringLength = std::strlen("/usr/lib/libSystem.B.dylib") + 1;
    command->cmdSize += stringLength;
    _header->sizeOfCommands += stringLength;
    writeCommandPadding(command);
}


template<typename TCommand>
TCommand*
MachoFileWriter::writeCommand(CommandType commandType)
{
    TCommand command =
    {
        commandType,
        sizeof(TCommand)
    };
    _header->numberOfCommands++;
    _header->sizeOfCommands += sizeof(TCommand);
    offset += sizeof(TCommand);
    return output.write<TCommand>(&command);
}


void
MachoFileWriter::writeSymbolTable()
{
    uint64_t symbolTableIndex = 1;
    symtabCommand->symbolOffset = offset;
    writeDyldPrivateSymbol();

    for (FunctionRoutine* internalRoutine : assemblyWriter->internalRoutines)
    {
        internalRoutine->stringTableIndexAddress = offset;

        // String table Index
        _bw->writeDoubleWord(0);

        // Type
        _bw->writeByte(N_SECT);

        // Section Index
        _bw->writeByte(1);

        // Description
        _bw->writeByte(0);

        // Library Ordinal
        _bw->writeByte(0);

        // Value
        _bw->writeQuadWord(_textSegmentStartVmAddress + internalRoutine->offset);

        internalRoutine->symbolTableIndex = symbolTableIndex;
        ++symbolTableIndex;
    }

    dysymTabCommand->undefinedSymbolIndex = symbolTableIndex;
    List<ExternalRoutine*> allRoutines = assemblyWriter->externalRoutines.concat(assemblyWriter->gotBoundRoutines);
    for (ExternalRoutine* externalRoutine : allRoutines)
    {
        externalRoutine->stringTableIndexAddress = offset;

        // String table Index
        _bw->writeDoubleWord(0);

        // Type
        _bw->writeByte(N_UNDF | N_EXT);

        // Section Index
        _bw->writeByte(0);

        // Description
        _bw->writeByte(0);

        // Library Ordinal
        _bw->writeByte(1);

        // Value
        _bw->writeQuadWord(0);

        ++dysymTabCommand->undefinedSymbolNumber;
        externalRoutine->symbolTableIndex = symbolTableIndex;
        ++symbolTableIndex;
    }
    symtabCommand->symbolEntries = symbolTableIndex;
}

void
MachoFileWriter::writeDyldPrivateSymbol()
{
    _dyldPrivateStringTableIndexOffset = offset;

    // String table Index
    _bw->writeDoubleWord(0);

    // Type
    _bw->writeByte(N_SECT);

    // Section Index
    _bw->writeByte(7);

    // Description
    _bw->writeByte(0);

    // Library Ordinal
    _bw->writeByte(0);

    // Value
    _bw->writeQuadWord(_dyldPrivateVmAddress);
}

void
MachoFileWriter::writeStringTable()
{
    symtabCommand->stringOffset = offset;
    _bw->writeString(" "); // This is used to mark empty string using 1 index
    uint64_t stringTableIndex = 2;

    _bw->writeDoubleWordAtAddress(stringTableIndex, _dyldPrivateStringTableIndexOffset);
    stringTableIndex += _bw->writeString("__dyld_private");

    for (FunctionRoutine* functionRoutine : assemblyWriter->internalRoutines)
    {
        _bw->writeDoubleWordAtAddress(stringTableIndex, functionRoutine->stringTableIndexAddress);
        _bw->writeString(functionRoutine->name);
        stringTableIndex += functionRoutine->name.size() + 1;
    }
    List<ExternalRoutine*> allRoutines = assemblyWriter->externalRoutines.concat(assemblyWriter->gotBoundRoutines);
    for (ExternalRoutine* externalRoutine : allRoutines)
    {
        _bw->writeDoubleWordAtAddress(stringTableIndex, externalRoutine->stringTableIndexAddress);
        _bw->writeString(externalRoutine->name);
        stringTableIndex += externalRoutine->name.size() + 1;
    }
    symtabCommand->stringSize = offset - symtabCommand->stringOffset;
    linkEditSegment->fileSize = offset - linkEditSegment->fileOffset;
}


void
MachoFileWriter::writeIndirectSymbolTable()
{
    dysymTabCommand->indirectSymbolTableOffset = offset;

    // Write stubs
    uint32_t index = 0;
    _stubsSection->reserved1 = index;
    for (ExternalRoutine* externalRoutine : assemblyWriter->externalRoutines)
    {
        _bw->writeDoubleWord(externalRoutine->symbolTableIndex);
    }
    index += assemblyWriter->externalRoutines.size();

    // Write got function
    _dataConstGotSection->reserved1 = index;
    for (ExternalRoutine* externalRoutine : assemblyWriter->gotBoundRoutines)
    {
        _bw->writeDoubleWord(externalRoutine->symbolTableIndex);
    }
    index += assemblyWriter->gotBoundRoutines.size();

    // Write laze symbol pointers
    _dataLaSymbolPtrSection->reserved1 = index;
    for (ExternalRoutine* externalRoutine : assemblyWriter->externalRoutines)
    {
        _bw->writeDoubleWord(externalRoutine->symbolTableIndex);
    }
    index += assemblyWriter->externalRoutines.size();

    dysymTabCommand->indirectSymbolTableEntries = index;
}


void
MachoFileWriter::writeLoadDylinkerCommand()
{
    auto command = writeCommand<LoadDylinkerCommand>(LC_LOAD_DYLINKER);
    command->name = 12;
    _bw->writeString("/usr/lib/dyld");
    command->cmdSize += std::strlen("/usr/lib/dyld") + 1;
    _header->sizeOfCommands += std::strlen("/usr/lib/dyld") + 1;
    writeCommandPadding(command);
}


template<typename TCommand>
void
MachoFileWriter::writeCommandPadding(TCommand* command)
{
    uint32_t rest = command->cmdSize % 8;
    if (rest != 0)
    {
        uint32_t padding = 8 - rest;
        writePadding(padding);
        command->cmdSize += padding;
        _header->sizeOfCommands += padding;
    }
}


void
MachoFileWriter::writeUuidCommand()
{
    writeCommand<UuidCommand>(LC_UUID);
}


void
MachoFileWriter::writeMainCommand(FunctionRoutine* startRoutine)
{
    _mainCommand = writeCommand<MainCommand>(LC_MAIN);
    _mainCommand->offset = 0;
    _mainCommand->stackSize = 0;
}


void
MachoFileWriter::relocateCStrings(uint64_t textSegmentStartOffset)
{
    assemblyWriter->relocateCStrings(textSegmentStartOffset);
}


}

#pragma clang diagnostic pop