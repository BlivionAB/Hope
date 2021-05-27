#include "DyldInfoWriter.h"


namespace elet::domain::compiler::instruction::output
{


DyldInfoWriter::DyldInfoWriter(BaselineObjectFileWriter* machoWriter):
    _machoWriter(machoWriter),
    _bw(new ByteWriter(&_data, &_offset))
{

}

void
DyldInfoWriter::write()
{
    writeRebaseInfo();
    writeBindingInfo();
    writeLazyBindingInfo();
}

void
DyldInfoWriter::writeRebaseInfo()
{
    _bw->writeByte(REBASE_OPCODE_SET_TYPE_IMM);
    _bw->writeByte(REBASE_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB);
    _bw->writeByte(0);
    _bw->writeByte(REBASE_OPCODE_DO_REBASE_IMM_TIMES);
    _bw->writeByte(REBASE_OPCODE_DONE);
}


void
DyldInfoWriter::writeBindingInfo()
{
    _bw->writeByte(BIND_OPCODE_SET_DYLIB_ORDINAL_IMM);
    _bw->writeByte(BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM);
    _bw->writeString("dyld_stub_binder");
    _bw->writeByte(BIND_OPCODE_DO_BIND);
    _bw->writeByte(BIND_OPCODE_DONE);
}


void
DyldInfoWriter::writeLazyBindingInfo()
{

}


}