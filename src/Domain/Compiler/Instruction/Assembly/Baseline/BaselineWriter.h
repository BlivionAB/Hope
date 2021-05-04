#ifndef ELET_BASELINEWRITER_H
#define ELET_BASELINEWRITER_H


#include <mutex>
#include <map>
#include <Foundation/Utf8String.h>
#include <Domain/Compiler/Instruction/AssemblyWriter.h>
#include "Domain/Compiler/Syntax/Instruction.h"
#include "Domain/Compiler/Instruction/TextWriter.h"


namespace elet::domain::compiler::instruction::output
{


struct UnknownRoutineError
{

};


struct UnknownRegisterError
{

};


class BaselineWriter : public AssemblyWriterInterface
{

public:

    BaselineWriter();

    ~BaselineWriter();

    void
    writeRoutine(FunctionRoutine* routine);

private:

    static
    thread_local
    TextWriter*
    _tw;

    output::FunctionRoutine*
    _currentRoutine;

    List<TextWriter*>
    _textWriters;

    std::map<FunctionRoutine*, Utf8String>
    _baselineOutput;

    TextWriter*
    createTextWriter();

    void
    writeInstruction(output::Instruction* instruction);

    void
    writeOperand(output::Operand* operand);

    void
    writeInteger(output::Int32* operand);

    void
    writeRegister(output::Register* _register);

    void
    writeLabel(output::Label* label);

    void
    writeVariableReference(output::VariableReference* variableReference);
};


}


#endif //ELET_BASELINEWRITER_H
