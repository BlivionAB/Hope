#include "./Syntax.Type.h"

namespace elet::domain::compiler::ast::type
{
    Type::Type():
        Type(TypeKind::Any, 0)
    { }


    Type::Type(TypeKind kind):
        Type(kind, 0)
    { }


    Type::Type(TypeKind kind, unsigned int pointers):
        Type(kind, pointers, 0, 0, false)
    {
        switch (kind)
        {
            case TypeKind::U64:
                minValue = static_cast<uint64_t>(IntegerLimit::U64Min);
                maxValue = static_cast<uint64_t>(IntegerLimit::U64Max);
                break;
            case TypeKind::U32:
                minValue = static_cast<uint64_t>(IntegerLimit::U32Min);
                maxValue = static_cast<uint64_t>(IntegerLimit::U32Max);
                break;
            case TypeKind::U16:
                minValue = static_cast<uint64_t>(IntegerLimit::U16Min);
                maxValue = static_cast<uint64_t>(IntegerLimit::U16Max);
                break;
            case TypeKind::U8:
                minValue = static_cast<uint64_t>(IntegerLimit::U8Min);
                maxValue = static_cast<uint64_t>(IntegerLimit::U8Max);
                break;
            case TypeKind::S64:
                minValue = static_cast<int64_t>(SignedIntegerLimit::S64Min);
                maxValue = static_cast<int64_t>(SignedIntegerLimit::S64Max);
                break;
            case TypeKind::S32:
                minValue = static_cast<int64_t>(SignedIntegerLimit::S32Min);
                maxValue = static_cast<int64_t>(SignedIntegerLimit::S32Max);
                break;
            case TypeKind::S16:
                minValue = static_cast<int64_t>(SignedIntegerLimit::S16Min);
                maxValue = static_cast<int64_t>(SignedIntegerLimit::S16Max);
                break;
            case TypeKind::S8:
                minValue = static_cast<int64_t>(SignedIntegerLimit::S8Min);
                maxValue = static_cast<int64_t>(SignedIntegerLimit::S8Max);
                break;
            case TypeKind::Bool:
                minValue = 0;
                maxValue = 1;
                break;
            case TypeKind::Any:
                minValue = static_cast<int64_t>(SignedIntegerLimit::S64Min);
                maxValue = static_cast<uint64_t>(IntegerLimit::U64Max);
                break;
            default:
                throw std::runtime_error("Unknown kind in Type(kind, pointers).");
        }
    }


    Type::Type(Int128 minValue, Int128 maxValue):
        minValue(minValue),
        maxValue(maxValue)
    {
        if (minValue >= SignedIntegerLimit::S32Min && minValue <= SignedIntegerLimit::S32Max)
        {
            kind = TypeKind::S32;
        }
        else if (minValue >= IntegerLimit::U32Min && minValue <= IntegerLimit::U32Max)
        {
            kind = TypeKind::U32;
        }
        else if (minValue >= SignedIntegerLimit::S64Min && minValue <= SignedIntegerLimit::S64Max)
        {
            kind = TypeKind::S64;
        }
        else if (minValue >= IntegerLimit::U64Min && minValue <= IntegerLimit::U64Max)
        {
            kind = TypeKind::U64;
        }
        else
        {
            throw std::runtime_error("Could not construct type from min and max values.");
        }
    }


    Type::Type(TypeKind kind, Int128 minValue, Int128 maxValue):
        Type(kind, 0, minValue, maxValue, explicitSet)
    { }


    Type::Type(TypeKind kind, unsigned int pointers, Int128 minValue, Int128 maxValue, bool explicitSet):
        kind(kind),
        pointers(pointers),
        minValue(minValue),
        maxValue(maxValue),
        explicitSet(explicitSet)
    {
        if (explicitSet)
        {
            return;
        }
        switch (kind)
        {
            case TypeKind::U64:
                this->minValue = 0;
                this->maxValue = static_cast<uint64_t>(IntegerLimit::U64Max);
                break;
            case TypeKind::U32:
                this->minValue = 0;
                this->maxValue = static_cast<uint64_t>(IntegerLimit::U32Max);
                break;
            case TypeKind::U16:
                this->minValue = 0;
                this->maxValue = static_cast<uint64_t>(IntegerLimit::U16Max);
                break;
            case TypeKind::U8:
                this->minValue = 0;
                this->maxValue = static_cast<uint64_t>(IntegerLimit::U8Max);
            case TypeKind::S64:
                this->minValue = static_cast<int64_t>(SignedIntegerLimit::S64Min);
                this->maxValue = static_cast<int64_t>(SignedIntegerLimit::S64Max);
                break;
            case TypeKind::S32:
                this->minValue = static_cast<int64_t>(SignedIntegerLimit::S32Min);
                this->maxValue = static_cast<int64_t>(SignedIntegerLimit::S32Max);
                break;
            case TypeKind::S16:
                this->minValue = static_cast<int64_t>(SignedIntegerLimit::S16Min);
                this->maxValue = static_cast<int64_t>(SignedIntegerLimit::S16Max);
                break;
            case TypeKind::S8:
                this->minValue = static_cast<int64_t>(SignedIntegerLimit::S8Min);
                this->maxValue = static_cast<int64_t>(SignedIntegerLimit::S8Max);
                break;
        }
    }

    RegisterSize
    Type::size()
    {
        if (pointers > 0)
        {
            return RegisterSize::Pointer;
        }
        switch (kind)
        {
            case TypeKind::Uint:
            case TypeKind::S64:
            case TypeKind::U64:
                return RegisterSize::Quad;
            case TypeKind::S32:
            case TypeKind::U32:
                return RegisterSize::Dword;
            case TypeKind::U16:
            case TypeKind::S16:
                return RegisterSize::Word;
            case TypeKind::U8:
            case TypeKind::S8:
            case TypeKind::Char:
            case TypeKind::Bool:
                return RegisterSize::Byte;
            default:
                throw std::runtime_error("Could not resolve primitive type size.");
        }
    }

    Sign
    Type::sign()
    {
        switch (kind)
        {
            case TypeKind::S8:
            case TypeKind::S16:
            case TypeKind::S32:
            case TypeKind::S64:
                return Sign::Signed;
            case TypeKind::Char:
            case TypeKind::U8:
            case TypeKind::U16:
            case TypeKind::U32:
            case TypeKind::U64:
            case TypeKind::Bool:
                return Sign::Unsigned;
            default:
                throw std::runtime_error("Cannot get signedness from type.");
        }
    }


    bool
    Type::operator !=(TypeKind typeKind)
    {
                return kind != typeKind;
    }


    bool
    Type::operator==(TypeKind typeKind)
    {
        return kind == typeKind;
    }


    void
    Type::setSet(Int128 minValue, Int128 maxValue)
    {
        this->minValue = minValue;
        this->maxValue = maxValue;
        explicitSet = true;
    }


    void
    Type::setSet(Type* type)
    {
        setSet(type->minValue, type->maxValue);
    }


    bool
    operator >= (const Int128& value1, const IntegerLimit& value2){
        return value1 >= static_cast<uint64_t>(value2);
    }


    bool
    operator >= (const Int128& value1, const SignedIntegerLimit& value2){
        return value1 >= static_cast<int64_t>(value2);
    }


    bool
    operator <= (const Int128& value1, const IntegerLimit& value2){
        return value1 <= static_cast<uint64_t>(value2);
    }


    bool
    operator <= (const Int128& value1, const SignedIntegerLimit& value2){
        return value1 <= static_cast<int64_t>(value2);
    }


    bool
    isIntegralType(const Type* type)
    {
        return type->kind >= TypeKind::IntegralStart && type->kind <= TypeKind::IntegralEnd;
    }
}