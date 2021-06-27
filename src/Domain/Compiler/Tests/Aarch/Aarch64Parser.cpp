#include "Aarch64Parser.h"
#include "AarchTypes.h"
#include <Domain/Compiler/Instruction/Assembly/Aarch/AArch64Encodings.h>

namespace elet::domain::compiler::test::aarch
{


using namespace elet::domain::compiler::instruction::output;

List<Instruction>
Aarch64Parser::parse(List<uint8_t>& output, size_t offset, size_t size)
{
    List<Instruction> instructions;
    _output = &output;
    _offset = offset;
    _cursor = offset;
    _size = size;

    while (_cursor - _offset < _size)
    {
        Instruction* instruction = instructions.emplace();
        uint32_t dw = getDoubleWord(instruction);

        if (Aarch64Instruction::StpPreIndex64 == (dw & Aarch64Instruction::LoadStoreMask))
        {
            instruction->rt = static_cast<Register>(dw & Aarch64Instruction::RtMask);
            instruction->rn = static_cast<Register>((dw & Aarch64Instruction::RnMask) >> 5);
            instruction->rt2 = static_cast<Register>((dw & Aarch64Instruction::Rt2Mask) >> 10);
            instruction->imm7 = (((dw & Aarch64Instruction::Imm7Mask) >> 15) | 0x80) * 8;
        }
    }

    return instructions;
}



uint32_t
Aarch64Parser::getDoubleWord(Instruction* instruction)
{
    uint32_t result = 0;
    for (int i = 0; i < 4; ++i)
    {
        uint8_t byte = getByte(instruction);
        result |= (byte << 8*i);
    }
    return result;
}


uint8_t
Aarch64Parser::getByte(Instruction* instruction)
{
    uint8_t result = (*_output)[_cursor++];
    instruction->bytes.add(result);
    return result;
}


}