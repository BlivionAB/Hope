#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-magic-numbers"
#pragma ide diagnostic ignored "cppcoreguidelines-pro-bounds-pointer-arithmetic"
#include "BaselineObjectFileWriter.h"
#include "DyldInfoWriter.h"


namespace elet::domain::compiler::instruction::output
{


BaselineObjectFileWriter::BaselineObjectFileWriter()
{
    _bw = new ByteWriter(&output, &offset);
    assemblyWriter = new X86_64Writer(&_text);
    _dyldInfoWriter = new DyldInfoWriter(this);
    _baselinePrinter = new x86::X86BaselinePrinter();
}


void
BaselineObjectFileWriter::write(FunctionRoutine* startRoutine)
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
    writeLoadDylibCommands();

    writeTextSegment();
    writeDataConstSegment();
    writeDataSegment();
    writeDyldInfo();
    writeSymbolTable();
    writeIndirectSymbolTable();
    writeStringTable();

    std::ofstream file;
    const char* path = Path::resolve(Path::cwd(), "cmake-build-debug/test.o").toString().toString();
    file.open(path, std::ios_base::binary);
    for (int i = 0; i < output.size(); ++i)
    {
        file.write(reinterpret_cast<char*>(&output[i]), 1);
    }
    std::cout << path << std::endl;
    file.close();

    auto output = assemblyWriter->getOutput();
    _baselinePrinter->print(output);
}


void
BaselineObjectFileWriter::writeTextSegment()
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
    _textSegmentStartOffset = offset;
    _textSegmentStartVmAddress = newVmAddress;
    output.add(_text);
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
BaselineObjectFileWriter::layoutTextSegment(FunctionRoutine* startRoutine)
{
    writeTextSection(startRoutine);
    writeStubsSection();
    writeStubHelperSection();

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
BaselineObjectFileWriter::writeTextSection(FunctionRoutine* startRoutine)
{
    _textOffset = assemblyWriter->getOffset();
    assemblyWriter->writeTextSection(startRoutine);
    _textSize = assemblyWriter->getOffset() - _textOffset;
}


void
BaselineObjectFileWriter::writeStubsSection()
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
BaselineObjectFileWriter::writeStubHelperSection()
{
    _stubHelperOffset = assemblyWriter->getOffset();
    size_t rest = _stubHelperOffset % 4;
    if (rest != 0)
    {
        assemblyWriter->writePadding(rest);
        _stubHelperOffset = assemblyWriter->getOffset();
    }
    assemblyWriter->writeStubHelper();
    _stubHelperSize = assemblyWriter->getOffset() - _stubHelperOffset;
    _stringOffset = assemblyWriter->getOffset();
    assemblyWriter->writeCStringSection();
    _stringSize = assemblyWriter->getOffset() - _stringOffset;
}


void
BaselineObjectFileWriter::writeTextSegmentCommand()
{
    _textSegment = writeSegment({
        LC_SEGMENT_64,
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
        S_ATTR_PURE_INSTRUCTIONS | S_ATTR_SOME_INSTRUCTIONS,
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
         S_SYMBOL_STUBS | S_ATTR_PURE_INSTRUCTIONS | S_ATTR_SOME_INSTRUCTIONS,
         0,
         assemblyWriter->getExternRoutinesSize(), /*number of stubs*/
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
        S_ATTR_PURE_INSTRUCTIONS | S_ATTR_SOME_INSTRUCTIONS,
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
        S_CSTRING_LITERALS,
        0,
        0,
        0
    }, _textSegment);
}


void
BaselineObjectFileWriter::writeHeader()
{
    MachHeader64 header {
        .magic = MACHO_MAGIC_64,
        .cpuType = CPU_TYPE_X86_64,
        .cpuSubType = CPU_SUBTYPE_X86_64_ALL,
        .fileType = MH_EXECUTE,
        .numberOfCommands = 0,
        .sizeOfCommands = 0,
        .flags = MH_NOUNDEFS | MH_DYLDLINK | MH_TWOLEVEL | MH_PIE,
        .reserved = 0,
    };
    _header = output.write(&header);
    offset += sizeof(*_header);
}


void
BaselineObjectFileWriter::writePageZeroSegmentCommand()
{
    writeSegment({
        LC_SEGMENT_64,
        sizeof(SegmentCommand64),
        "__PAGEZERO",
        0,
        0x0000000100000000,
        0,
        0,
        VM_PROTECTION_NONE,
        VM_PROTECTION_NONE,
        0,
        0
    });
}


SegmentCommand64*
BaselineObjectFileWriter::writeSegment(SegmentCommand64 segmentCommand)
{
    offset += sizeof(SegmentCommand64);
    _header->sizeOfCommands += sizeof(SegmentCommand64);
    _header->numberOfCommands++;
    return output.write(&segmentCommand);
}


Section64*
BaselineObjectFileWriter::writeSection(Section64 section, SegmentCommand64* segment)
{
    offset += sizeof(Section64);
    _header->sizeOfCommands += sizeof(Section64);
    segment->commandSize += sizeof(Section64);
    segment->numberOfSections++;
    return output.write(&section);
}


void
BaselineObjectFileWriter::writePadding(uint32_t padding)
{
    for (unsigned int i = 0; i < padding; ++i)
    {
        output.add(0);
    }
    offset += padding;
}


void
BaselineObjectFileWriter::writeDyldInfoSegmentCommand()
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
BaselineObjectFileWriter::writeDyldInfo()
{
    _dyldInfoWriter->write();
}


void
BaselineObjectFileWriter::writeLinkEditSegmentCommand()
{
    linkEditSegment = writeSegment({
        .command = LC_SEGMENT_64,
        .commandSize = sizeof(SegmentCommand64),
        .segmentName = "__LINKEDIT",
        .vmAddress = vmAddress,
        .vmSize = 0x4000,
        .fileOffset = 0,
        .fileSize = 0x4000,
        .maximumProtection = VM_PROTECTION_NONE,
        .initialProtection = VM_PROTECTION_NONE,
        .numberOfSections = 0,
        .flags = 0,
    });
}


void
BaselineObjectFileWriter::writeDataConstSegmentCommand()
{
    _dataConstSegment = writeSegment({
        .command = LC_SEGMENT_64,
        .commandSize = sizeof(SegmentCommand64),
        .segmentName = "__DATA_CONST",
        .vmAddress = 0,
        .vmSize = 0,
        .fileOffset = 0,
        .fileSize = 0,
        .maximumProtection = VM_PROTECTION_READ | VM_PROTECTION_WRITE,
        .initialProtection = VM_PROTECTION_READ | VM_PROTECTION_WRITE,
        .numberOfSections = 0,
        .flags = 0b00000010, // Don't know what this flag does? But, its in most binaries.
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
BaselineObjectFileWriter::writeDataSegmentCommand()
{
    _dataSegment = writeSegment({
        .command = LC_SEGMENT_64,
        .commandSize = sizeof(SegmentCommand64),
        .segmentName = "__DATA",
        .vmAddress = 0,
        .vmSize = 0,
        .fileOffset = 0,
        .fileSize = 0,
        .maximumProtection = VM_PROTECTION_READ | VM_PROTECTION_WRITE,
        .initialProtection = VM_PROTECTION_READ | VM_PROTECTION_WRITE,
        .numberOfSections = 0,
        .flags = 0b00000010, // Don't know what this flag does? But, its in most binaries.
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
}


void
BaselineObjectFileWriter::writeDataConstSegment()
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
            _bw->writeDoubleWordAtAddress(offset - (relocationAddress + 4), relocationAddress);
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
BaselineObjectFileWriter::writeDataSegment()
{
    _dataSegment->vmAddress = vmAddress;
    _dataSegment->vmSize = SEGMENT_SIZE;
    _dataSegment->fileOffset = offset;
    _dataSegment->fileSize = SEGMENT_SIZE;

    writeLaSymbolPtrSection();
}


void
BaselineObjectFileWriter::writeLaSymbolPtrSection()
{
    _dataLaSymbolPtrSection->address = vmAddress;
    _dataLaSymbolPtrSection->offset = offset;
    for (const auto& externalRoutine : assemblyWriter->externalRoutines)
    {
        // TODO: This is causing a crash right now, due to no dsymtab and symtab
        _bw->writeDoubleWordAtAddress(offset - (_textSegmentStartOffset + externalRoutine->stubAddress + 4) /* It should be based after the instruction*/, _textSegmentStartOffset + externalRoutine->stubAddress);
        _bw->writeQuadWord(_textSegmentStartVmAddress + externalRoutine->stubHelperAddress);
    }
    _dataLaSymbolPtrSection->size = offset - _dataLaSymbolPtrSection->offset;
    writePadding(SEGMENT_SIZE - _dataLaSymbolPtrSection->size);
    vmAddress += SEGMENT_SIZE;
}


void
BaselineObjectFileWriter::writeDysymTabCommand()
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
BaselineObjectFileWriter::writeSymtabCommand()
{
    SymtabCommand command =
    {
        .cmd = LC_SYMTAB,
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
BaselineObjectFileWriter::writeLoadDylibCommands()
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

    // Command size must be 4-byte aligned
    uint32_t rest = command->cmdSize % 4;
    if (rest != 0)
    {
        uint32_t padding = 4 - rest;
        writePadding(padding);
        command->cmdSize += padding;
        _header->sizeOfCommands += padding;
    }
}


template<typename TCommand>
TCommand*
BaselineObjectFileWriter::writeCommand(CommandType commandType)
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
BaselineObjectFileWriter::writeSymbolTable()
{
    uint64_t symbolTableIndex = 0;
    dysymTabCommand->undefinedSymbolIndex = symbolTableIndex;
    symtabCommand->symbolOffset = offset;
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
    symtabCommand->symbolEntries = allRoutines.size();
}

void
BaselineObjectFileWriter::writeStringTable()
{
    symtabCommand->stringOffset = offset;
    _bw->writeString(" ");
    uint64_t stringTableIndex = 2;
    List<ExternalRoutine*> allRoutines = assemblyWriter->externalRoutines.concat(assemblyWriter->gotBoundRoutines);
    for (ExternalRoutine* externalRoutine : allRoutines)
    {
        _bw->writeDoubleWordAtAddress(stringTableIndex, externalRoutine->stringTableIndexAddress);
        _bw->writeString(externalRoutine->name);
        stringTableIndex += externalRoutine->name.size() + 1;
    }
    symtabCommand->stringSize = offset - symtabCommand->stringOffset;
}


void
BaselineObjectFileWriter::writeIndirectSymbolTable()
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


}

#pragma clang diagnostic pop