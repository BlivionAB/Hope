#include "Error.h"


namespace elet::domain::compiler::ast::error
{
    std::string
    TypeCheckError::getTypeString(const Type* type)
    {
        std::string result;
        switch (type->kind)
        {
            case TypeKind::U8:
                result = "u8";
                break;
            case TypeKind::S8:
                result = "s8";
                break;
            case TypeKind::U16:
                result = "u16";
                break;
            case TypeKind::S16:
                result = "s16";
                break;
            case TypeKind::U32:
                result = "u32";
                break;
            case TypeKind::S32:
                result = "s32";
                break;
            case TypeKind::U64:
                result = "u64";
                break;
            case TypeKind::S64:
                result = "s64";
                break;
            case TypeKind::Char:
                result = "char";
                break;
            case TypeKind::Bool:
                result = "bool";
                break;
            default:
                throw std::runtime_error("Not implemented type in getTypeString.");
        }
        return result;
    }


    TypeCheckError::TypeCheckError(
        const SourceFile* sourceFile,
        const Syntax* syntax,
        std::string message):

        SyntaxError(
            sourceFile,
            syntax,
            message)
    { }


    TypeCheckError::TypeCheckError(
        const SourceFile* sourceFile,
        const Syntax* syntax,
        std::string message,
        const Type* type1):

        SyntaxError(
            sourceFile,
            syntax,
            message,
            getTypeString(type1))
    { }
}