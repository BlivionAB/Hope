#ifndef ELET_BASELINEWRITER_H
#define ELET_BASELINEWRITER_H


#include <mutex>
#include <map>
#include <Foundation/Utf8String.h>
#include <Domain/Compiler/Instruction/AssemblyWriter.h>
#include "Domain/Compiler/Instruction/Instruction.h"
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

    BaselineWriter(List<std::uint8_t>* output);

    void
    writeStartRoutine(FunctionRoutine* routine, std::size_t offset);

private:

    void
    writeFunctionRoutine(FunctionRoutine* routine);

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

    void
    writeCallInstruction(const CallInstruction* callInstruction);

    void
    writeArgumentDeclaration(ArgumentDeclaration* argumentDeclaration, unsigned int argumentIndex);

    TextWriter*
    _tw;

    output::FunctionRoutine*
    _currentRoutine;

    List<TextWriter*>
    _textWriters;

    std::map<FunctionRoutine*, Utf8String>
    _baselineOutput;

};


}


#endif //ELET_BASELINEWRITER_H
