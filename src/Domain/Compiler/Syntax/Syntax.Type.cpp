#include "./Syntax.Type.h"

namespace elet::domain::compiler::ast::type
{
    bool
    operator >= (const Int128& value1, const IntegerLimit& value2){
        return value1 >= static_cast<uint64_t>(value2);
    }


    bool
    operator <= (const Int128& value1, const IntegerLimit& value2){
        return value1 <= static_cast<uint64_t>(value2);
    }
}