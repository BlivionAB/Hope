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
            _tw.write(static_cast<int>(parameterDeclaration->allocationSize));
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
            case output::InstructionKind::Load:
                writeLoadInstruction(reinterpret_cast<const output::LoadInstruction*>(instruction));
                break;
            case output::InstructionKind::MoveAddress:
                writeMoveAddressInstruction(reinterpret_cast<const output::MoveAddressInstruction*>(instruction));
                break;
            case output::InstructionKind::MoveRegister:
                writeMoveRegisterInstruction(reinterpret_cast<const output::MoveRegisterInstruction*>(instruction));
                break;
            case output::InstructionKind::MoveImmediate:
                writeMoveImmediateInstruction(reinterpret_cast<const output::MoveImmediateInstruction*>(instruction));
                break;
            case output::InstructionKind::Return:
                _tw.write("Ret");
                break;
            case output::InstructionKind::MultiplyRegister:
                writeOperation("Mul", reinterpret_cast<const output::MultiplyRegisterInstruction*>(instruction));
                break;
            case output::InstructionKind::AddRegister:
                writeOperation("Add", reinterpret_cast<const output::MultiplyRegisterInstruction*>(instruction));
                break;

            case output::InstructionKind::OrRegister:
                writeOperation("Or", reinterpret_cast<const output::MultiplyRegisterInstruction*>(instruction));
                break;
            case output::InstructionKind::XorRegister:
                writeOperation("Xor", reinterpret_cast<const output::MultiplyRegisterInstruction*>(instruction));
                break;
            case output::InstructionKind::AndRegister:
                writeOperation("And", reinterpret_cast<const output::MultiplyRegisterInstruction*>(instruction));
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
        _tw.write("[Sp + ");
        _tw.write(storeRegisterInstruction->stackOffset);
        _tw.write("], ");
        writeOperandRegister(storeRegisterInstruction->target);
    }

    void
    StashIRPrinter::writeMoveRegisterInstruction(const output::MoveRegisterInstruction* moveRegisterInstruction)
    {
        _tw.write("Mov ");
        writeOperandRegister(moveRegisterInstruction->destination);
        _tw.write(", ");
        writeOperandRegister(moveRegisterInstruction->target);
    }


    void
    StashIRPrinter::writeLoadInstruction(const output::LoadInstruction* loadInstruction)
    {
        _tw.write("Ldr ");
        writeOperandRegister(loadInstruction->destination);
        _tw.write(", [Sp + #");
        _tw.write(-static_cast<int64_t>(loadInstruction->stackOffset) - static_cast<int64_t>(loadInstruction->allocationSize));
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
    StashIRPrinter::writeStoreImmediateInstruction(const output::StoreImmediateInstruction* storeImmediateInstruction)
    {
        _tw.write("Str [Sp + #");
        _tw.write(-static_cast<int64_t>(storeImmediateInstruction->stackOffset) - static_cast<int64_t>(storeImmediateInstruction->allocationSize));
        _tw.write("], ");
        _tw.writeSignedImmediateValue(storeImmediateInstruction->value);
    }


    void
    StashIRPrinter::writeMoveAddressInstruction(const output::MoveAddressInstruction* moveAddressInstruction)
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
    StashIRPrinter::writeOperation(Utf8String operationString, const output::MultiplyRegisterInstruction* multiplyRegisterInstruction)
    {
        _tw.write(operationString);
        _tw.write(" ");
        writeOperationRegisters(multiplyRegisterInstruction);
    }


    void
    StashIRPrinter::writeOperationRegisters(const output::OperationInstruction* operationInstruction)
    {
        writeOperandRegister(operationInstruction->destination);
        _tw.write(", ");
        writeOperandRegister(operationInstruction->target);
        _tw.write(", ");
        writeOperandRegister(operationInstruction->value);
    }
}
