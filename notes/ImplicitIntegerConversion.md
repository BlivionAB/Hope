# Implicit Integer Conversion

* If both operands are of the same type. The type is the common type.
* Else the types are different.
  * If the types have the same signedness (both signed or unsigned). The destination whose 
    type has the lesser conversion rank is implicitly converted to the other type.
  * Else, the unsigned type has conversion rank less than the signed type:
    * If the signed type can represent all values of the unsigned type, then the
      destination with the unsigned type is implicitly converted to the signed type.
    * Else, both operands undergo implicit conversion to the unsigned type 
      counterpart of the signed destination's type.

Conversion Ranking


Default implicit integer literal type follows the conventions set in CPP.
https://en.cppreference.com/w/cpp/language/integer_literal


## Integer Literal Expression Coercion

Consider:
```
var s: s8 = 1 + 2;
```

The expression `1 + 2` with implicit integer conversion yields `s32` type. Though, `s32` does
not fit into `s8`. Thus, we have to compute the value of all integer literal
expression in order to know if it fits into placeholder type. Since, we support upto
`u64` and all signed types, we have to have a signed type that encompasses `u64`.
Right now, that type is `s128`.