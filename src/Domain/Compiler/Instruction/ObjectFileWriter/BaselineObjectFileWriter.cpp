#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-magic-numbers"
#pragma ide diagnostic ignored "cppcoreguidelines-pro-bounds-pointer-arithmetic"
#include "BaselineObjectFileWriter.h"
#include "DyldInfoWriter.h"


namespace elet::domain::compiler::instruction::output
{


BaselineObjectFileWriter::BaselineObjectFileWriter()
{
    _assemblyWriter = new X86_64Writer(&_text);
    _dyldInfoWriter = new DyldInfoWriter();
    _baselinePrinter = new x86::X86BaselinePrinter();
}


void
BaselineObjectFileWriter::write(FunctionRoutine* startRoutine)
{
    layoutTextSegment(startRoutine);
    layoutDyldInfo();
    _output.reserve(100*4096);

    writeHeader();
    writePageZeroSegmentCommand();
    writeTextSegmentCommand();
    writeDyldInfoSegmentCommand();
    _textSection->address += _cursor;
    _textSection->offset += _cursor;
    _stubsSection->address += _cursor;
    _stubsSection->offset += _cursor;
    _stubHelperSection->address += _cursor;
    _stubHelperSection->offset += _cursor;
    _cstringSection->address += _cursor;
    _cstringSection->offset += _cursor;
    _textSegment->fileSize = _textSegmentSize;
    _textSegment->vmSize = _textSegmentSize;

    _output.add(_text);
    std::ofstream file;
    const char* path = Path::resolve(Path::cwd(), "cmake-build-debug/test.o").toString().toString();
    file.open(path, std::ios_base::binary);
    for (int i = 0; i < _output.size(); ++i)
    {
        file.write(reinterpret_cast<char*>(&_output[i]), 1);
    }
    std::cout << path << std::endl;
    file.close();

    auto output = _assemblyWriter->getOutput();
    _baselinePrinter->print(output);
}


void
BaselineObjectFileWriter::layoutTextSegment(FunctionRoutine* startRoutine)
{
    _textOffset = _assemblyWriter->getOffset();
    _assemblyWriter->writeTextSection(startRoutine);
    _textSize = _assemblyWriter->getOffset() - _textOffset;

    if (_assemblyWriter->hasExternalRoutines())
    {
        {
            _stubsOffset = _assemblyWriter->getOffset();
            _assemblyWriter->writeStubs();
            _stubsSize = _assemblyWriter->getOffset() - _stubsOffset;
        }
        {
            _stubHelperOffset = _assemblyWriter->getOffset();
            _assemblyWriter->writeStubHelper();
            _stubHelperSize = _assemblyWriter->getOffset() - _stubHelperOffset;
        }
    }
    if (_assemblyWriter->hasStrings())
    {
        _stringOffset = _assemblyWriter->getOffset();
        _assemblyWriter->writeCStringSection();
        _stringSize = _assemblyWriter->getOffset() - _stringOffset;
    }
    _textSegmentSize = _assemblyWriter->getOffset() - _textOffset;
}


void
BaselineObjectFileWriter::writeTextSegmentCommand()
{
    _textSegment = writeSegment({
        LC_SEGMENT_64,
        sizeof(SegmentCommand64),
        "__TEXT",
        0x0000000100000000,
        _textSize,
        0,
        _textSize,
        VM_PROTECTION_READ | VM_PROTECTION_EXECUTE,
        VM_PROTECTION_READ | VM_PROTECTION_EXECUTE,
        0,
        0,
    });

    _textSection = writeSection({
        "__text",
        "__TEXT",
        0x0000000100000000,
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

    if (_assemblyWriter->hasExternalRoutines())
    {
        _stubsSection = writeSection({
             "__stubs",
             "__TEXT",
             0x0000000100000000 + static_cast<uint64_t>(_stubsOffset),
             _stubsSize,
             _stubsOffset,
             1,
             0,
             0,
             S_SYMBOL_STUBS | S_ATTR_PURE_INSTRUCTIONS | S_ATTR_SOME_INSTRUCTIONS,
             0,
             _assemblyWriter->getExternRoutinesSize(), /*number of stubs*/
             0
        }, _textSegment);

        _stubHelperSection = writeSection({
            "__stub_helper",
            "__TEXT",
            0x0000000100000000 + static_cast<uint64_t>(_stubHelperOffset),
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
    }
    if (_assemblyWriter->hasStrings())
    {
        _cstringSection = writeSection({
            "__cstring",
            "__TEXT",
            0x0000000100000000 + static_cast<uint64_t>(_stringOffset),
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
    _header = _output.write(&header);
    _cursor += sizeof(*_header);
//    std::cout << sizeof(_header) << std::endl;
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
    _cursor += sizeof(SegmentCommand64);
    _header->sizeOfCommands += sizeof(SegmentCommand64);
    _header->numberOfCommands++;
    return _output.write(&segmentCommand);
}

Section64*
BaselineObjectFileWriter::writeSection(Section64 section, SegmentCommand64* segment)
{
    _cursor += sizeof(Section64);
    _header->sizeOfCommands += sizeof(Section64);
    segment->commandSize += sizeof(Section64);
    segment->numberOfSections++;
    return _output.write(&section);
}

void
BaselineObjectFileWriter::writePadding(unsigned int padding)
{
    for (unsigned int i = 0; i < padding; ++i)
    {
        _assemblyWriter->writeByte(0);
    }
}


void
BaselineObjectFileWriter::writeDyldInfoSegmentCommand()
{
    DyldInfoCommand command {
        .cmd = 0,
        .cmdSize = 0,
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
    _dyldInfoCommand = _output.write<DyldInfoCommand>(&command);
}


void
BaselineObjectFileWriter::layoutDyldInfo()
{
    _dyldInfoWriter
    writeByte();

}


}

#pragma clang diagnostic pop