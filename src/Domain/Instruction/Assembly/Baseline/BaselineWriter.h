#ifndef ELET_BASELINEWRITER_H
#define ELET_BASELINEWRITER_H


#include <mutex>
#include <map>
#include <Foundation/Utf8String.h>
#include <Domain/Instruction/AssemblyWriter.h>
#include "Domain/Instruction/Instruction.h"
#include "Domain/Instruction/TextWriter.h"


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
    writeRoutine(Routine* routine);

private:

    static
    thread_local
    TextWriter*
    _tw;

    List<TextWriter*>
    _textWriters;

    std::map<Routine*, Utf8String>
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
