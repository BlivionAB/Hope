#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-avoid-magic-numbers"
#pragma ide diagnostic ignored "cppcoreguidelines-pro-bounds-pointer-arithmetic"
#include "BaselineObjectFileWriter.h"

namespace elet::domain::compiler::instruction::output
{


BaselineObjectFileWriter::BaselineObjectFileWriter()
{
    _assemblyWriter = new X86_64Writer(&_text);
    _baselinePrinter = new x86::X86BaselinePrinter();
}


void
BaselineObjectFileWriter::write(FunctionRoutine* startRoutine)
{
    layoutTextSegment(startRoutine);
    _output.reserve(4096);

    writeHeader();
    writePageZeroSegmentCommand();
    writeTextSegmentCommand();
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
    _textSize = _assemblyWriter->getOutput()->size();
    std::uint32_t rest = _textSize % 16;
    std::uint32_t padding = 0;
    if (rest != 0)
    {
        padding = 16 - rest;
    }
    _textSize += padding;
    writePadding(padding);

    if (_assemblyWriter->hasExternalRoutines())
    {
        _stubOffset = _assemblyWriter->getOffset();
        _assemblyWriter.
    }
    if (_assemblyWriter->hasStrings())
    {
        _stringOffset = _assemblyWriter->getOffset();
        _assemblyWriter->writeCStringSection();
        _numberOfTextSections++;
    }
}


void
BaselineObjectFileWriter::writeTextSegmentCommand()
{
    writeSegment({
        LC_SEGMENT_64,
        sizeof(SegmentCommand64),
        "__TEXT",
        0x0000000100000000,
        _textSize,
        0,
        _textSize,
        VM_PROTECTION_READ | VM_PROTECTION_EXECUTE,
        VM_PROTECTION_READ | VM_PROTECTION_EXECUTE,
        _numberOfTextSections,
        0,
    });

    writeSection({
        "__text",
        "__TEXT",
        0x0000000100000000,
        _textSize,
        0,
        16,
        0,
        0,
        S_ATTR_PURE_INSTRUCTIONS | S_ATTR_SOME_INSTRUCTIONS,
        0,
        0,
        0
    });

    if (_assemblyWriter->hasExternalRoutines())
    {
        writeSection({
             "__cstring",
             "__TEXT",
             0x0000000100000000 + static_cast<uint64_t>(_textSize),
             _textSize,
             0,
             1,
             0,
             0,
             S_CSTRING_LITERALS,
             0,
             0,
             0
        });
    }
    if (_assemblyWriter->hasStrings())
    {
        writeSection({
            "__cstring",
            "__TEXT",
            0x0000000100000000 + static_cast<uint64_t>(_textSize),
            _textSize,
            0,
            1,
            0,
            0,
            S_CSTRING_LITERALS,
            0,
            0,
            0
        });
    }
}


void
BaselineObjectFileWriter::writeHeader()
{
    _header = _output.write(_header);
    _cursor += sizeof(*_header) / 8;
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
    _cursor += sizeof(SegmentCommand64) / 8;
    _header->sizeOfCommands += sizeof(SegmentCommand64);
    _header->numberOfCommands++;
    return _output.write(&segmentCommand);
}

Section64*
BaselineObjectFileWriter::writeSection(Section64 section)
{
    _cursor += sizeof(Section64) / 8;
    _header->sizeOfCommands += sizeof(Section64);
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


}

#pragma clang diagnostic pop