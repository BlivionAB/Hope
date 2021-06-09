#include "DyldInfoWriter.h"
#include "ExportInfoWriter.h"


namespace elet::domain::compiler::instruction::output
{


DyldInfoWriter::DyldInfoWriter(BaselineObjectFileWriter* machoWriter):
    _machoWriter(machoWriter),
    _exportInfoWriter(new ExportInfoWriter(machoWriter)),
    _bw(new ByteWriter(&machoWriter->output, &machoWriter->offset))
{

}


void
DyldInfoWriter::write()
{
    _machoWriter->linkEditSegment->fileOffset = _machoWriter->offset;
    size_t start = _machoWriter->offset;
    writeRebaseInfo();
    writeBindingInfo();
    writeLazyBindingInfo();
    writeExportInfo();
    size_t size = _machoWriter->offset - start;
    _machoWriter->linkEditSegment->vmAddress = _machoWriter->vmAddress;
    _machoWriter->linkEditSegment->fileSize = 0x4000;
    _machoWriter->linkEditSegment->vmSize = 0x4000;
}


void
DyldInfoWriter::writeRebaseInfo()
{
    _machoWriter->dyldInfoCommand->rebaseOffset = _machoWriter->offset;
    _bw->writeByte(REBASE_OPCODE_SET_TYPE_IMM | REBASE_TYPE_POINTER);
    _bw->writeByte(REBASE_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB | 3);
    _bw->writeUleb128(0);
    _bw->writeByte(REBASE_OPCODE_DO_REBASE_IMM_TIMES | _machoWriter->assemblyWriter->exportedRoutines.size());
    _bw->writeByte(REBASE_OPCODE_DONE);
    uint32_t size = _machoWriter->offset - _machoWriter->dyldInfoCommand->rebaseOffset;
    uint32_t rest = size % 8;
    if (rest != 0)
    {
        uint32_t padding = 8 - rest;
        _bw->writePadding(padding);
        _machoWriter->dyldInfoCommand->rebaseSize = size + padding;
    }
    else
    {
        _machoWriter->dyldInfoCommand->rebaseSize = size;
    }
}


void
DyldInfoWriter::writeBindingInfo()
{
    _machoWriter->dyldInfoCommand->bindOffset = _machoWriter->offset;
    _bw->writeByte(BIND_OPCODE_SET_DYLIB_ORDINAL_IMM | 1);
    _bw->writeByte(BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM);
    _bw->writeString("dyld_stub_binder");
    _bw->writeByte(BIND_OPCODE_SET_TYPE_IMM | 1);
    _bw->writeByte(BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB | 2);
    _bw->writeUleb128(0);
    _bw->writeByte(BIND_OPCODE_DO_BIND);
    _bw->writeByte(BIND_OPCODE_DONE);
    uint32_t size = _machoWriter->offset - _machoWriter->dyldInfoCommand->bindOffset;
    uint32_t rest = size % 8;
    if (rest != 0)
    {
        uint32_t padding = 8 - rest;
        _bw->writePadding(padding);
        _machoWriter->dyldInfoCommand->bindSize = size + padding;
    }
    else
    {
        _machoWriter->dyldInfoCommand->bindSize = size;
    }
}


void
DyldInfoWriter::writeLazyBindingInfo()
{
    _machoWriter->dyldInfoCommand->lazyBindOffset = _machoWriter->offset;
    std::size_t offset = 0;
    for (const auto& externalRoutine : _machoWriter->assemblyWriter->externalRoutines)
    {
        auto start = _machoWriter->offset;
        _bw->writeByte(BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB | 3);
        _bw->writeByte(offset);
        _bw->writeByte(BIND_OPCODE_SET_DYLIB_ORDINAL_IMM | 1);
        _bw->writeByte(BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM);
        _bw->writeString(externalRoutine->name);
        _bw->writeByte(BIND_OPCODE_DO_BIND);
        offset += _machoWriter->offset - start;
    }
    uint32_t size = _machoWriter->offset - _machoWriter->dyldInfoCommand->lazyBindOffset;
    uint32_t rest = size % 8;
    if (rest != 0)
    {
        uint32_t padding = 8 - rest;
        _bw->writePadding(padding);
        _machoWriter->dyldInfoCommand->lazyBindSize = size + padding;
    }
    else
    {
        _machoWriter->dyldInfoCommand->lazyBindSize = size;
    }
}


void
DyldInfoWriter::writeExportInfo()
{
    _machoWriter->dyldInfoCommand->exportOffset = _machoWriter->offset;
    _exportInfoWriter->write();
    uint32_t size = _machoWriter->offset - _machoWriter->dyldInfoCommand->exportOffset;
    uint32_t rest = size % 8;
    if (rest != 0)
    {
        uint32_t padding = 8 - rest;
        _bw->writePadding(padding);
        _machoWriter->dyldInfoCommand->exportSize = size + padding;
    }
    else
    {
        _machoWriter->dyldInfoCommand->exportSize = size;
    }
}


}