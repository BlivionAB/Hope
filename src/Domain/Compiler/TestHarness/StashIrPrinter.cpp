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
            case output::InstructionKind::AddRegister:
                _tw.write("Add OpLeft, OpLeft, OpRight");
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
        _tw.write(-loadInstruction->stackOffset - static_cast<int>(loadInstruction->allocationSize));
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
            case output::OperandRegister::Left:
                _tw.write("OpLeft");
                break;
            case output::OperandRegister::Right:
                _tw.write("OpRight");
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
        _tw.write(-storeImmediateInstruction->stackOffset - static_cast<int>(storeImmediateInstruction->allocationSize));
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
}
