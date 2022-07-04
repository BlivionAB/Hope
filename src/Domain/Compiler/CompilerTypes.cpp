#include "CompilerTypes.h"


namespace elet::domain::compiler
{
    std::string
    integerToString(IntegerKind integerKind)
    {
        std::string result;
        switch (integerKind)
        {
            case IntegerKind::U8:
                result = "u8";
                break;
            case IntegerKind::S8:
                result = "s8";
                break;
            case IntegerKind::U16:
                result = "u16";
                break;
            case IntegerKind::S16:
                result = "s16";
                break;
            case IntegerKind::U32:
                result = "u32";
                break;
            case IntegerKind::S32:
                result = "s32";
                break;
            case IntegerKind::U64:
                result = "u64";
                break;
            case IntegerKind::S64:
                result = "s64";
                break;
        }
        return result;
    }

    CompilerOptions::CompilerOptions(AssemblyTarget assemblyTarget, ObjectFileTarget objectFileTarget):
        assemblyTarget(assemblyTarget),
        objectFileTarget(objectFileTarget)
    { }


    ExpandedCompilerOptions::ExpandedCompilerOptions(AssemblyTarget assemblyTarget, ObjectFileTarget objectFileTarget)
        : CompilerOptions(assemblyTarget, objectFileTarget)
    {
        switch (assemblyTarget)
        {
            case AssemblyTarget::x86_64:
                assemblyHasMultiRegisterOperands = true;
                treatModuloAsDivision = true;
                break;
            case AssemblyTarget::Aarch64:
                assemblyHasMultiRegisterOperands = false;
                treatModuloAsDivision = false;
                break;
            default:;
        }

    }
}