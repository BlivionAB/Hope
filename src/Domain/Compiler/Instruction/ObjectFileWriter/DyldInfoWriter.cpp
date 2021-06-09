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
    _machoWriter->linkEditSegment->fileSize = size;
    _machoWriter->linkEditSegment->vmSize = size;
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
    _machoWriter->dyldInfoCommand->rebaseSize = _machoWriter->offset - _machoWriter->dyldInfoCommand->rebaseOffset;
}


void
DyldInfoWriter::writeBindingInfo()
{
    _machoWriter->dyldInfoCommand->bindOffset = _machoWriter->offset;
    _bw->writeByte(BIND_OPCODE_SET_DYLIB_ORDINAL_IMM | 1);
    _bw->writeByte(BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM);
    _bw->writeString("dyld_stub_binder");
    _bw->writeByte(BIND_OPCODE_DO_BIND);
    _bw->writeByte(BIND_OPCODE_DONE);
    _machoWriter->dyldInfoCommand->bindSize = _machoWriter->offset - _machoWriter->dyldInfoCommand->bindOffset;

}


void
DyldInfoWriter::writeLazyBindingInfo()
{
    _machoWriter->dyldInfoCommand->lazyBindOffset = _machoWriter->offset;
    std::size_t offset = 0;
    for (const auto& externalRoutine : _machoWriter->assemblyWriter->externalRoutines)
    {
        auto start = _machoWriter->offset;
        _bw->writeByte(BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB);
        _bw->writeByte(offset);
        _bw->writeByte(BIND_OPCODE_SET_DYLIB_ORDINAL_IMM | externalRoutine->libraryOrdinal);
        _bw->writeByte(BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM);
        _bw->writeString(externalRoutine->name);
        _bw->writeByte(BIND_OPCODE_DO_BIND);
        offset += _machoWriter->offset - start;
    }
    _machoWriter->dyldInfoCommand->lazyBindSize = _machoWriter->offset - _machoWriter->dyldInfoCommand->lazyBindOffset;
}


void
DyldInfoWriter::writeExportInfo()
{
    _machoWriter->dyldInfoCommand->exportOffset = _machoWriter->offset;
    _exportInfoWriter->write();
    _machoWriter->dyldInfoCommand->exportSize = _machoWriter->offset - _machoWriter->dyldInfoCommand->exportOffset;
}


}