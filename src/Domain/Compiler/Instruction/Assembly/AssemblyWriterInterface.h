#ifndef ELET_ASSEMBLYWRITERINTERFACE_H
#define ELET_ASSEMBLYWRITERINTERFACE_H

#include <Domain/Compiler/Instruction/Instruction.h>
#include <Domain/Compiler/Instruction/ByteWriter.h>

//namespace elet::domain::compiler::instruction
//{
//    struct FunctionRoutine;
//}

namespace elet::domain::compiler::instruction::output
{


    struct String;



    enum class CallingConvention
    {
        SysV,
        MicrosoftX64,
    };


    class AssemblyWriterInterface
    {

    public:

        AssemblyWriterInterface(List<uint8_t>* output);

        virtual
        void
        setCallingConvention(CallingConvention callingConvention);

        virtual
        void
        writeTextSection(FunctionRoutine* routine) = 0;

        void
        writeFunction(FunctionRoutine* function);

        void
        writeFunctionInstructions(FunctionRoutine* function);

        virtual
        void
        writeFunctionRelocationAddresses(FunctionRoutine* routine) = 0;

        virtual
        void
        writeCallInstruction(CallInstruction* callInstruction, FunctionRoutine* parentRoutine);

        virtual
        void
        writeStoreImmediateInstruction(StoreImmediateInstruction* storeImmediateInstruction, FunctionRoutine* function);

        virtual
        void
        writeStoreRegisterInstruction(StoreRegisterInstruction* storeRegisterInstruction, FunctionRoutine* function);

        virtual
        void
        writeLoadInstruction(LoadInstruction* loadInstruction, FunctionRoutine* function);

        virtual
        void
        writeAddRegisterInstruction(AddRegisterToRegisterInstruction* addRegisterInstruction, FunctionRoutine* function);

        virtual
        void
        writeAddRegisterAddressInstruction(AddAddressToRegisterInstruction* addRegisterAddressInstruction, FunctionRoutine* function);

        virtual
        void
        writeMultiplySignedRegisterAddressInstruction(MultiplySignedAddressToRegisterInstruction* multiplyRegisterAddressInstruction, FunctionRoutine* function);

        virtual
        void
        writeDivideUnsignedRegisterAddressInstruction(DivideUnsignedAddressToRegisterInstruction* instruction, FunctionRoutine* function);

        virtual
        void
        writeDivideSignedRegisterAddressInstruction(DivideSignedAddressToRegisterInstruction* instruction, FunctionRoutine* function);

        virtual
        void
        writeModuloUnsignedRegisterAddressInstruction(ModuloUnsignedAddressToRegisterInstruction* instruction, FunctionRoutine* function);

        virtual
        void
        writeModuloSignedRegisterAddressInstruction(ModuloSignedAddressToRegisterInstruction* instruction, FunctionRoutine* function);

        virtual
        void
        writeMoveImmediateInstruction(MoveImmediateInstruction* moveImmediateInstruction, FunctionRoutine* function);

        virtual
        void
        writeMoveRegisterInstruction(MoveRegisterInstruction* moveRegisterInstruction, FunctionRoutine* function);

        virtual
        void
        writeMoveAddressInstruction(MoveAddressInstruction* moveAddressInstruction, FunctionRoutine* function);

        virtual
        void
        writeAddImmediateInstruction(AddImmediateToRegisterInstruction* subtractImmediateInstruction, FunctionRoutine* function);

        virtual
        void
        writeSubtractImmediateInstruction(SubtractImmediateToRegisterInstruction* subtractImmediateInstruction, FunctionRoutine* function);

        virtual
        void
        writeSubtractRegisterAddressInstruction(SubtractRegisterToAddressInstruction* subtractRegisterAddressInstruction, FunctionRoutine* function);

        virtual
        void
        writeReturnInstruction(ReturnInstruction* returnInstruction, FunctionRoutine* function);

        virtual
        void
        writeInstructionsPadding(FunctionRoutine* function);

        virtual
        void
        writeFunctionPrologue(FunctionRoutine* function);

        virtual
        void
        writeFunctionEpilogue(FunctionRoutine* function);

        virtual
        void
        writeCStringSection() = 0;

        virtual
        void
        relocateCStrings(uint64_t textSegmentStartOffset);

        virtual
        void
        relocateStub(uint64_t offset, uint64_t textSegmentStartOffset, ExternalRoutine* externalRoutine) = 0;

        virtual
        void
        relocateDyldPrivate(uint64_t dataSectionOffset, uint64_t textSegmentStartOffset) = 0;

        virtual
        void
        relocateGotBoundRoutine(uint64_t gotOffset, uint64_t offset) = 0;

        virtual
        void
        relocateStubHelperOffset(uint64_t offset, uint64_t stubHelperAddress, uint64_t textSegmentStartOffset) = 0;

        virtual
        void
        writeStubs() = 0;

        virtual
        void
        writeStubHelper() = 0;

        List<uint8_t>*
        getOutput();

        uint64_t
        getOffset();

        List<String*>
        getStrings() const;

        uint32_t
        getExternRoutinesSize() const;

        ByteWriter*
        bw;

        List<FunctionRoutine*>
        internalRoutineList;

        List<ExternalRoutine*>
        externalRoutineList;

        List<ExternalRoutine*>
        gotBoundRoutines;

        List<FunctionRoutine*>
        exportedRoutines;

        void
        writePadding(size_t amount);

        uint64_t
        dyldPrivateOffset;

    protected:

        output::Instruction*
        _nextInstruction;

        List<output::Instruction*>::Iterator*
        _instructionIterator;

        CallingConvention
        _callingConvention;

        List<uint8_t>*
        _output;

        List<String*>
        _strings;

        uint64_t
        _offset = 0;

//        uint64_t
//        getStackSizeFromFunctionParameters(const FunctionRoutine* routine);

        bool
        nextInstructionIs(InstructionKind kind) const;
};
}


#endif //ELET_ASSEMBLYWRITERINTERFACE_H
