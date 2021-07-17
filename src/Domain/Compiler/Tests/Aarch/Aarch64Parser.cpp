#include "Aarch64Parser.h"
#include "AarchTypes.h"
#include <Domain/Compiler/Instruction/Assembly/Aarch/AArch64Encodings.h>

namespace elet::domain::compiler::test::aarch
{


using namespace elet::domain::compiler::instruction::output;

void
Aarch64Parser::parse(List<OneOfInstruction>& instructions, List<uint8_t>& output, size_t offset, size_t size)
{
    _output = &output;
    _offset = offset;
    _cursor = offset;
    _size = size;

    while (_cursor - _offset < _size)
    {
        Instruction* instruction = reinterpret_cast<Instruction*>(instructions.emplace());
        uint32_t dw = getDoubleWord(instruction);

        if (tryParse21Instructions(instruction, dw))
        {
            continue;
        }
        if (tryParse22Instructions(instruction, dw))
        {
            continue;
        }
        if (tryParse25Instructions(instruction, dw))
        {
            continue;
        }
        if (tryParse26Instructions(instruction, dw))
        {
            continue;
        }
        if (Aarch64Instruction::AdrpImmediate64 == (dw & Aarch64Instruction::AdrpMask))
        {
            parseAdrpInstruction(instruction, dw);
            continue;
        }
        throw std::runtime_error("Could not find instruction when parsing.");
    }
}

bool
Aarch64Parser::tryParse26Instructions(Instruction* instruction, uint32_t dw)
{
    uint32_t kind26 = dw & Mask26;

    if (Bl == kind26)
    {
        UnconditionalBranchImmediateInstruction* unbrimm = reinterpret_cast<UnconditionalBranchImmediateInstruction*>(instruction);
        unbrimm->kind = Aarch64Instruction::Bl;
        unbrimm->imm26 = imm26(dw);
        return true;
    }
    return false;
}

bool
Aarch64Parser::tryParse25Instructions(Instruction* instruction, uint32_t dw)
{
    uint32_t kind25 = dw & Mask25;

    if (Aarch64Instruction::UnconditionalBranchRegister == kind25)
    {
        BranchExceptionSyscallInstruction* brexsysc = reinterpret_cast<BranchExceptionSyscallInstruction*>(instruction);
        brexsysc->kind = Aarch64Instruction::Ret;
        brexsysc->rn = Rn(dw);
        return true;
    }
    return false;
}


bool
Aarch64Parser::tryParse22Instructions(Instruction* instruction, uint32_t dw)
{
    uint32_t kind22 = dw & Mask22;

    switch (kind22)
    {
        case Aarch64Instruction::StrImmediateBaseOffset64:
        case Aarch64Instruction::LdrImmediateBaseOffset64:
            parseLoadStoreInstruction(instruction, dw, kind22);
            return true;
        case Aarch64Instruction::StpPreIndex64:
        case Aarch64Instruction::StpBaseOffset64:
        case Aarch64Instruction::LdpPostIndex64:
        case Aarch64Instruction::LdpBaseOffset64:
            parseLoadStorePairInstruction(instruction, dw, kind22);
            return true;
        case Aarch64Instruction::AddImmediate64:
        case Aarch64Instruction::SubImmediate64:
            parseDataProcessImmediateInstruction(instruction, dw, kind22);
            return true;
    }
    return false;
}


void
Aarch64Parser::parseDataProcessImmediateInstruction(Instruction* instruction, uint32_t dw, uint32_t kind22)
{
    DataProcessImmediateInstruction* dataProcessImmediateInstruction = reinterpret_cast<DataProcessImmediateInstruction*>(instruction);
    dataProcessImmediateInstruction->kind = static_cast<Aarch64Instruction>(kind22);
    dataProcessImmediateInstruction->rd = Rd(dw);
    dataProcessImmediateInstruction->rn = Rn(dw);
    dataProcessImmediateInstruction->imm12 = imm12(dw);
}



void
Aarch64Parser::parseLoadStoreInstruction(Instruction* instruction, uint32_t dw, uint32_t kind22)
{
    LoadStoreInstruction* loadStoreInstruction = reinterpret_cast<LoadStoreInstruction*>(instruction);
    loadStoreInstruction->kind = static_cast<Aarch64Instruction>(kind22);
    loadStoreInstruction->rt = Rt(dw);
    loadStoreInstruction->rn = Rn(dw);
    loadStoreInstruction->imm12 = imm12(dw) * 8;

    switch (loadStoreInstruction->kind)
    {
        case Aarch64Instruction::StrImmediateBaseOffset64:
        case Aarch64Instruction::LdrImmediateBaseOffset64:
            loadStoreInstruction->addressMode = AddressMode::BaseOffset;
            break;
        default:
            throw std::runtime_error("Unknown load store instruction.");
    }
}


void
Aarch64Parser::parseLoadStorePairInstruction(Instruction* instruction, uint32_t dw, uint32_t kind22)
{
    LoadStorePairInstruction* loadStoreInstruction = reinterpret_cast<LoadStorePairInstruction*>(instruction);
    loadStoreInstruction->kind = static_cast<Aarch64Instruction>(kind22);
    loadStoreInstruction->rt = Rt(dw);
    loadStoreInstruction->rn = Rn(dw);
    loadStoreInstruction->rt2 = Rt2(dw);
    loadStoreInstruction->imm7 = imm7(dw);

    switch (loadStoreInstruction->kind)
    {
        case Aarch64Instruction::StpPreIndex64:
            loadStoreInstruction->addressMode = AddressMode::PreIndex;
            break;
        case Aarch64Instruction::StrImmediateBaseOffset64:
        case Aarch64Instruction::StpBaseOffset64:
        case Aarch64Instruction::LdpBaseOffset64:
            loadStoreInstruction->addressMode = AddressMode::BaseOffset;
            break;
        case Aarch64Instruction::LdpPostIndex64:
            loadStoreInstruction->addressMode = AddressMode::PostIndex;
            break;
        default:
            throw std::runtime_error("Unknown load store pair instruction.");
    }
}


bool
Aarch64Parser::tryParse21Instructions(Instruction* instruction, uint32_t dw)
{
    uint32_t kind21 = dw & Mask21;

    switch (kind21)
    {
        case Movz64:
            parseMovImmediateInstruction(instruction, dw);
            return true;
        case Mov64:
            parseMovInstruction(instruction, dw);
            return true;
    }

    return false;
}

void
Aarch64Parser::parseMovInstruction(Instruction* instruction, uint32_t dw)
{
    MovInstruction* movz = reinterpret_cast<MovInstruction*>(instruction);
    movz->kind = Mov64;
    movz->rd = Rd(dw);
    movz->rm = Rm(dw);
}

void
Aarch64Parser::parseMovImmediateInstruction(Instruction* instruction, uint32_t dw)
{
    MovWideImmediateInstruction* movz = reinterpret_cast<MovWideImmediateInstruction*>(instruction);
    movz->kind = Movz64;
    movz->rd = Rd(dw);
    movz->imm16 = uimm16(dw);
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

Register
Aarch64Parser::Rn(uint32_t dw)
{
    return static_cast<Register>((dw & Aarch64Instruction::RnMask) >> 5);
}


Register
Aarch64Parser::Rm(uint32_t dw)
{
    return static_cast<Register>((dw & Aarch64Instruction::RmMask) >> 16);
}


Register
Aarch64Parser::Rt(uint32_t dw)
{
    return static_cast<Register>((dw & Aarch64Instruction::RtMask) >> 0);
}


Register
Aarch64Parser::Rd(uint32_t dw)
{
    return static_cast<Register>((dw & Aarch64Instruction::RtMask) >> 0);
}


Register
Aarch64Parser::Rt2(uint32_t dw)
{
    return static_cast<Register>((dw & Aarch64Instruction::Rt2Mask) >> 10);
}


int8_t
Aarch64Parser::imm7(uint32_t dw)
{
    return (((dw & Aarch64Instruction::Imm7Mask) >> 15) | 0x80) * 8;
}


int16_t
Aarch64Parser::imm12(uint32_t dw)
{
    return (dw & Aarch64Instruction::Imm12Mask) >> 10;
}


int32_t
Aarch64Parser::imm26(uint32_t dw)
{
    return (dw & Aarch64Instruction::Imm26Mask);
}


uint16_t
Aarch64Parser::uimm16(uint32_t dw)
{
    return (dw & Aarch64Instruction::Imm16Mask) >> 5;
}


void
Aarch64Parser::parseAdrpInstruction(Instruction* instruction, uint32_t dw)
{
    AdrpInstruction* adrp = reinterpret_cast<AdrpInstruction*>(instruction);
    adrp->kind = Aarch64Instruction::AdrpImmediate64;
    adrp->rd = Rd(dw);
    adrp->immhilo = immhilo(dw);
}


uint32_t
Aarch64Parser::immhilo(uint32_t dw)
{
    uint32_t result = 0;
    result |= (dw & MASK(12, 5)) >> 3;
    result |= (dw & (0b11 << 29)) >> 29;
    return result;

}


}