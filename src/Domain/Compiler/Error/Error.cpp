#include "Error.h"
#include "../Syntax/Syntax.Type.h"

namespace elet::domain::compiler::ast::error
{
    std::string
    TypeCheckError::getTypeString(const Type* type)
    {
        std::string result;
        switch (type->kind)
        {
            case TypeKind::Int:
                result = "int";
                break;
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
        if (type::isIntegralType(type))
        {
            Int128 minValue = type->minValue;
            Int128 maxValue = type->maxValue;
            switch (type->kind)
            {
                case TypeKind::U64:
                    if (minValue == static_cast<uint64_t>(IntegerLimit::U64Min) && maxValue == static_cast<uint64_t>(IntegerLimit::U64Max))
                    {
                        return result;
                    }
                    break;
                case TypeKind::U32:
                    if (minValue == static_cast<uint64_t>(IntegerLimit::U32Min) && maxValue == static_cast<uint64_t>(IntegerLimit::U32Max))
                    {
                        return result;
                    }
                    break;
                case TypeKind::U16:
                    if (minValue == static_cast<uint64_t>(IntegerLimit::U16Min) && maxValue == static_cast<uint64_t>(IntegerLimit::U16Max))
                    {
                        return result;
                    }
                    break;
                case TypeKind::U8:
                    if (minValue == static_cast<uint64_t>(IntegerLimit::U8Min) && maxValue == static_cast<uint64_t>(IntegerLimit::U8Max))
                    {
                        return result;
                    }
                    break;

                case TypeKind::S64:
                    if (minValue == static_cast<int64_t>(SignedIntegerLimit::S64Min) && maxValue == static_cast<int64_t>(SignedIntegerLimit::S64Max))
                    {
                        return result;
                    }
                    break;
                case TypeKind::S32:
                    if (minValue == static_cast<int64_t>(SignedIntegerLimit::S32Min) && maxValue == static_cast<int64_t>(SignedIntegerLimit::S32Max))
                    {
                        return result;
                    }
                    break;
                case TypeKind::S16:
                    if (minValue == static_cast<int64_t>(SignedIntegerLimit::S16Min) && maxValue == static_cast<int64_t>(SignedIntegerLimit::S16Max))
                    {
                        return result;
                    }
                    break;
                case TypeKind::S8:
                    if (minValue == static_cast<int64_t>(SignedIntegerLimit::S8Min) && maxValue == static_cast<int64_t>(SignedIntegerLimit::S8Max))
                    {
                        return result;
                    }
                    break;
            }
            if (type->minValue != type->maxValue)
            {
                result += " " + type->minValue.toString() + ".." + type->maxValue.toString();
            }
            else
            {
                result += " " + type->minValue.toString();
            }
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