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
        if (isIntegralType(this))
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
                    default:
                        throw std::runtime_error("Unknown kind in Type(kind, pointers).");
            }
        }
    }


    Type::Type(Int128 minValue, Int128 maxValue, Sign sign):
        minValue(minValue),
        maxValue(maxValue)
    {
        switch (sign)
        {
            case Sign::Signed:
                kind = getDefaultSignedTypeFromBounds(minValue, maxValue);
                break;
            case Sign::Unsigned:
                kind = getDefaultUnsignedTypeFromBounds(minValue, maxValue);
                break;
            default:
                throw std::runtime_error("Not supported sign");
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
        return getRegisterSizeFromTypeKind(kind);
    }


    RegisterSize
    Type::boundSize()
    {
        return getRegisterSizeFromTypeKind(getMinimumTypeKindFromBounds(minValue, maxValue));
    }


    Sign
    Type::sign() const
    {
        switch (kind)
        {
            case TypeKind::Int:
                return Sign::Unknown;
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
    Type::setBounds(Int128 minValue, Int128 maxValue)
    {
        this->minValue = minValue;
        this->maxValue = maxValue;
        explicitSet = true;
    }


    void
    Type::setBounds(Type* type)
    {
        setBounds(type->minValue, type->maxValue);
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


    TypeKind
    getMinimumTypeKindFromBounds(Int128 minValue, Int128 maxValue)
    {
        if (minValue >= SignedIntegerLimit::S8Min && maxValue <= SignedIntegerLimit::S8Max)
        {
            return TypeKind::S8;
        }
        if (minValue >= IntegerLimit::U8Min && maxValue <= IntegerLimit::U8Max)
        {
            return TypeKind::U8;
        }
        if (minValue >= SignedIntegerLimit::S16Min && maxValue <= SignedIntegerLimit::S16Max)
        {
            return TypeKind::S16;
        }
        if (minValue >= IntegerLimit::U16Min && maxValue <= IntegerLimit::U16Max)
        {
            return TypeKind::U16;
        }
        if (minValue >= SignedIntegerLimit::S32Min && maxValue <= SignedIntegerLimit::S32Max)
        {
            return TypeKind::S32;
        }
        if (minValue >= IntegerLimit::U32Min && maxValue <= IntegerLimit::U32Max)
        {
            return TypeKind::U32;
        }
        if (minValue >= SignedIntegerLimit::S64Min && maxValue <= SignedIntegerLimit::S64Max)
        {
            return TypeKind::S64;
        }
        if (minValue >= IntegerLimit::U64Min && maxValue <= IntegerLimit::U64Max)
        {
            return TypeKind::U64;
        }
        throw std::runtime_error("Could not construct type from min and max values.");
    }


    TypeKind
    getDefaultSignedTypeFromBounds(const Int128& minValue, const Int128& maxValue)
    {
        if (minValue >= SignedIntegerLimit::S32Min && maxValue <= SignedIntegerLimit::S32Max)
        {
            return TypeKind::S32;
        }
        if (minValue >= SignedIntegerLimit::S64Min && maxValue <= SignedIntegerLimit::S64Max)
        {
            return TypeKind::S64;
        }
        throw std::runtime_error("Could not construct type from min and max values.");
    }


    TypeKind
    getDefaultUnsignedTypeFromBounds(const Int128& minValue, const Int128& maxValue)
    {
        if (minValue >= IntegerLimit::U32Min && maxValue <= IntegerLimit::U32Max)
        {
            return TypeKind::U32;
        }
        if (minValue >= IntegerLimit::U64Min && maxValue <= IntegerLimit::U64Max)
        {
            return TypeKind::U64;
        }
        throw std::runtime_error("Could not construct type from min and max values.");
    }


    TypeKind
    getDefaultTypeFromBounds(const Int128& minValue, const Int128& maxValue)
    {
        if (minValue >= SignedIntegerLimit::S32Min && maxValue <= SignedIntegerLimit::S32Max)
        {
            return TypeKind::S32;
        }
        if (minValue >= IntegerLimit::U32Min && maxValue <= IntegerLimit::U32Max)
        {
            return TypeKind::U32;
        }
        if (minValue >= SignedIntegerLimit::S64Min && maxValue <= SignedIntegerLimit::S64Max)
        {
            return TypeKind::S64;
        }
        if (minValue >= IntegerLimit::U64Min && maxValue <= IntegerLimit::U64Max)
        {
            return TypeKind::U64;
        }
        throw std::runtime_error("Could not construct type from min and max values.");
    }


    RegisterSize
    getRegisterSizeFromTypeKind(TypeKind kind)
    {
        switch (kind)
        {
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
}