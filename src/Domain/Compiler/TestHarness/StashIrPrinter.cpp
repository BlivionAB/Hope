#include <Domain/Compiler/Instruction/Instruction.h>
#include "StashIrPrinter.h"

namespace elet::domain::compiler::test
{
    Utf8String
    StashIRPrinter::writeFunctionRoutines(std::queue<output::FunctionRoutine*>& instructions)
    {
        while (!instructions.empty())
        {
            output::FunctionRoutine* routine = instructions.front();
            writeFunctionRoutine(routine);
            instructions.pop();
        }
        return _tw.toString();
    }


    void
    StashIRPrinter::writeFunctionRoutine(output::FunctionRoutine* function)
    {
        _tw.write(function->name);
        _tw.write("(");
        for (const output::ParameterDeclaration* parameterDeclaration : function->parameters)
        {
            _tw.write(static_cast<int>(parameterDeclaration->destinationSize));
        }
        _tw.write("):");
        _tw.newline();
        _tw.indent();
        for (const output::Instruction* instruction : function->instructions)
        {
            writeInstruction(instruction, function);
        }
        _tw.unindent();
        _tw.newline();
        for (output::FunctionRoutine* f : function->subRoutines)
        {
            writeFunctionRoutine(f);
        }
    }


    void
    StashIRPrinter::writeInstruction(const output::Instruction* instruction, output::FunctionRoutine* function)
    {
        switch (instruction->kind)
        {
            case output::InstructionKind::Call:
                writeCallInstruction(reinterpret_cast<const output::CallInstruction*>(instruction), function);
                break;
            case output::InstructionKind::StoreRegister:
                writeStoreRegisterInstruction(reinterpret_cast<const output::StoreRegisterInstruction*>(instruction));
                break;
            case output::InstructionKind::StoreImmediate:
                writeStoreImmediateInstruction(reinterpret_cast<const output::StoreImmediateInstruction*>(instruction));
                break;
            case output::InstructionKind::LoadUnsigned:
                writeLoadUnsignedInstruction(reinterpret_cast<const output::LoadUnsignedInstruction*>(instruction));
                break;
            case output::InstructionKind::LoadSigned:
                writeLoadSignedInstruction(reinterpret_cast<const output::LoadSignedInstruction*>(instruction));
                break;
            case output::InstructionKind::MoveAddress:
                writeMoveAddressInstruction(reinterpret_cast<const output::MoveAddressToRegisterInstruction*>(instruction));
                break;
            case output::InstructionKind::MoveRegister:
                writeMoveRegisterInstruction(reinterpret_cast<const output::MoveRegisterToRegisterInstruction*>(instruction));
                break;
            case output::InstructionKind::MoveImmediate:
                writeMoveImmediateInstruction(reinterpret_cast<const output::MoveImmediateInstruction*>(instruction));
                break;
            case output::InstructionKind::MoveZeroExtend:
                writeMoveZeroExtendInstruction(reinterpret_cast<const output::MoveZeroExtendInstruction*>(instruction));
                break;
            case output::InstructionKind::MoveSignExtend:
                writeMoveSignExtend(reinterpret_cast<const output::MoveSignExtendInstruction*>(instruction));
                break;
            case output::InstructionKind::Return:
                _tw.write("Ret");
                break;
            case output::InstructionKind::AddRegisterToRegister:
                writeRegisterToRegisterOperation("Add", reinterpret_cast<const output::AddRegisterToRegisterInstruction*>(instruction));
                break;
            case output::InstructionKind::AddImmediateToRegister:
                writeImmediateToRegisterOperation("Add", reinterpret_cast<const output::AddImmediateToRegisterInstruction*>(instruction));
                break;
            case output::InstructionKind::SubtractRegisterToRegister:
                writeRegisterToRegisterOperation("Sub", reinterpret_cast<const output::SubtractRegisterToRegisterInstruction*>(instruction));
                break;
            case output::InstructionKind::SubtractImmediateToRegister:
                writeImmediateToRegisterOperation("Sub", reinterpret_cast<const output::SubtractImmediateToRegisterInstruction*>(instruction));
                break;
            case output::InstructionKind::MultiplySignedRegisterToRegister:
                writeRegisterToRegisterOperation("Mul", reinterpret_cast<const output::MultiplySignedRegisterToRegisterInstruction*>(instruction));
                break;
            case output::InstructionKind::DivideSignedRegisterToRegister:
                writeRegisterToRegisterOperation("Div", reinterpret_cast<const output::DivideSignedRegisterToRegisterInstruction*>(instruction));
                break;
            case output::InstructionKind::OrRegisterToRegister:
                writeRegisterToRegisterOperation("Or", reinterpret_cast<const output::OrRegisterToRegisterInstruction*>(instruction));
                break;
            case output::InstructionKind::XorRegisterToRegister:
                writeRegisterToRegisterOperation("Xor", reinterpret_cast<const output::XorRegisterToRegisterInstruction*>(instruction));
                break;
            case output::InstructionKind::AndRegisterToRegister:
                writeRegisterToRegisterOperation("And", reinterpret_cast<const output::AndRegisterToRegisterInstruction*>(instruction));
                break;
            default:
                throw std::runtime_error("Unknown instruction");
        }
        _tw.newline();
    }


    void
    StashIRPrinter::writeMoveImmediateInstruction(const output::MoveImmediateInstruction* moveImmediateInstruction)
    {
        _tw.write("Mov ");
        writeOperandRegister(moveImmediateInstruction->destination);
        _tw.write(", #");
        _tw.writeUnsignedHexValue(moveImmediateInstruction->value);
    }


    void
    StashIRPrinter::writeStoreRegisterInstruction(const output::StoreRegisterInstruction* storeRegisterInstruction)
    {
        _tw.write("Str ");
        _tw.write("[Sp - ");
        _tw.write(storeRegisterInstruction->stackOffset);
        _tw.write("], ");
        writeOperandRegister(storeRegisterInstruction->target);
    }

    void
    StashIRPrinter::writeMoveRegisterInstruction(const output::MoveRegisterToRegisterInstruction* instruction)
    {
        writeOperationName("Mov", instruction);
        _tw.write(" ");
        writeOperandRegister(instruction->destination);
        _tw.write(", ");
        writeOperandRegister(instruction->target);
    }


    void
    StashIRPrinter::writeMoveZeroExtendInstruction(const output::MoveZeroExtendInstruction* instruction)
    {
        writeOperationName("Movzx", instruction->targetSize);
        _tw.write(" ");
        writeOperandRegister(instruction->destination);
        _tw.write(", ");
        writeOperandRegister(instruction->target);
    }


    void
    StashIRPrinter::writeMoveSignExtend(const output::MoveSignExtendInstruction* instruction)
    {
        writeOperationName("Movsx", instruction->targetSize);
        _tw.write(" ");
        writeOperandRegister(instruction->destination);
        _tw.write(", ");
        writeOperandRegister(instruction->target);
    }


    void
    StashIRPrinter::writeLoadUnsignedInstruction(const output::LoadUnsignedInstruction* instruction)
    {
        writeOperationName("Ldr", instruction);
        _tw.write(" ");
        writeOperandRegister(instruction->destination);
        _tw.write(", [Sp + #");
        _tw.write(-static_cast<int64_t>(instruction->stackOffset));
        _tw.write("]");
    }


    void
    StashIRPrinter::writeLoadSignedInstruction(const output::LoadSignedInstruction* instruction)
    {
        writeOperationName("Ldrs", instruction);
        _tw.write(" ");
        writeOperandRegister(instruction->destination);
        _tw.write(", [Sp + #");
        _tw.write(-static_cast<int64_t>(instruction->stackOffset));
        _tw.write("]");
    }


    void
    StashIRPrinter::writeOperandRegister(output::OperandRegister operandRegister)
    {
        switch (operandRegister)
        {
            case output::OperandRegister::Return:
                _tw.write("OpRet");
                break;
            case output::OperandRegister::Scratch0:
                _tw.write("X0");
                break;
            case output::OperandRegister::Scratch1:
                _tw.write("X1");
                break;
            case output::OperandRegister::Scratch2:
                _tw.write("X2");
                break;
            case output::OperandRegister::Arg0:
                _tw.write("Arg0");
                break;
            case output::OperandRegister::Arg1:
                _tw.write("Arg1");
                break;
            case output::OperandRegister::Arg2:
                _tw.write("Arg2");
                break;
            case output::OperandRegister::Arg3:
                _tw.write("Arg3");
                break;
            default:
                throw std::runtime_error("Unknown OperandRegister in writeOperandRegister.");
        }
    }


    void
    StashIRPrinter::writeStoreImmediateInstruction(const output::StoreImmediateInstruction* instruction)
    {
        writeOperationName("Str", instruction);
        _tw.write(" [Sp + #");
        _tw.write(-static_cast<int64_t>(instruction->stackOffset));
        _tw.write("], ");
        _tw.writeSignedImmediateValue(instruction->value);
    }


    void
    StashIRPrinter::writeMoveAddressInstruction(const output::MoveAddressToRegisterInstruction* moveAddressInstruction)
    {
        _tw.write("Mov ");
        writeOperandRegister(moveAddressInstruction->destination);
        _tw.write(", [\"");
        _tw.write(std::get<output::String*>(moveAddressInstruction->constant)->value);
        _tw.write("\"]");
    }

    void
    StashIRPrinter::writeCallInstruction(const output::CallInstruction* callInstruction, output::FunctionRoutine* function)
    {
        _tw.write("Call ");
        if (callInstruction->routine->kind == output::RoutineKind::Function)
        {
            _tw.write(reinterpret_cast<output::FunctionRoutine*>(callInstruction->routine)->name);
            function->subRoutines.add(reinterpret_cast<output::FunctionRoutine*>(callInstruction->routine));
        }
        else if (callInstruction->routine->kind == output::RoutineKind::External)
        {
            _tw.write(reinterpret_cast<output::ExternalRoutine*>(callInstruction->routine)->name);
        }
        else
        {
            throw std::runtime_error("Unknown RoutineKind in writeCallInstruction.");
        }
    }


    void
    StashIRPrinter::writeRegisterToRegisterOperation(Utf8String operationName, const output::OperationRegisterToRegisterInstruction* instruction)
    {
        writeOperationName(operationName, instruction);
        _tw.write(" ");
        writeOperationRegisters(instruction);
    }


    void
    StashIRPrinter::writeOperationRegisters(const output::OperationRegisterToRegisterInstruction* operationInstruction)
    {
        writeOperandRegister(operationInstruction->destination);
        _tw.write(", ");
        writeOperandRegister(operationInstruction->target);
        _tw.write(", ");
        writeOperandRegister(operationInstruction->value);
    }

    void
    StashIRPrinter::writeImmediateToRegisterOperation(const char* operationName, const output::OperationImmediateToRegisterInstruction* instruction)
    {
        _tw.write(operationName);
        _tw.write(" ");
        writeOperandRegister(instruction->destination);
        _tw.write(", ");
        writeOperandRegister(instruction->target);
        _tw.write(", ");
        _tw.writeUnsignedHexValue(instruction->value);
    }


    void
    StashIRPrinter::writeOperationName(Utf8String name, const output::Instruction* instruction)
    {
        writeOperationName(name, instruction->destinationSize);
    }


    void
    StashIRPrinter::writeOperationName(Utf8String name, RegisterSize registerSize)
    {
        _tw.write(name);
        switch (registerSize)
        {
            case RegisterSize::Quad:
                _tw.write("Q");
                break;
            case RegisterSize::Dword:
                _tw.write("L");
                break;
            case RegisterSize::Word:
                _tw.write("W");
                break;
            case RegisterSize::Byte:
                _tw.write("B");
                break;
        }
    }
}
