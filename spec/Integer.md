## Syntax

The syntax for denoting an `IntegralType`:
```bnf
IntegralTypeAnnotation: IntegralType`:` Integer`..`Integer
```

where `IntegralType` can be any of:

```
IntegralType :: one of 
    `u64` `u32` `u16` `u8`
    `s64` `s32` `s16` `s8`
```

## Integer Literal Expression Coercion

Consider:
```
var s: s8 = 1 + 2;
```

The expression `1 + 2` with implicit integer conversion yields `s32` type. Though, `s32` does
not fit into `s8`. Thus, we have to compute the value of all integer literal
expression in order to know if it fits into placeholder type. Since, we support upto
`u64` and all signed types, we have to have a signed type that encompasses `u64`.
Right now, that type is `s128`. Note, for bitwise operators `&`, `|` and `^` we truncate the value at 64 bit.

## Operating Type and Resulting Type
There is two types that we keep track of in expressions:

* `ResultingType` is the type that fits the result of the operation.
* `OperatingType` is the type that fits the operands and the `ResultingType`.

Note, the `OperatingType`'s operators are the `ResultingType`:s of their expressions.

## Global Integral Expression Minimum And Maximum
Any integral expressions cannot exceed the *global maximum* or subceed the *global minimum*.

* *Global maximum:* `u64max` 
  * Decimal: `18446744073709551615`.
  * Hexadecimal: `0xffff_ffff_ffff_ffff`.
* *Global minimum:* `s64min`
  * Decimal: `-9223372036854775808`.
  * Hexadecimal: `0x8000_0000_0000_0000`.

These include dynamic(non-constant) expressions such as variables as well.

Note it is written in [grammarmarkdown](https://github.com/rbuckton/grammarkdown).

