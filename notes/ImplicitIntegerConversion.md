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

* *Global maximum:* `u64max` `18446744073709551615`.
* *Global minimum:* `s64min` `-9223372036854775808`

## Notations

The syntax for denoting the type of an expression is
```bnf
TypeAnnotation: Type`:` Integer`..`Integer
```
* `s32` denotes 

## Questions

* What type should integer literal be?
  * DecimalLiteral?
    * In C/C++ it's signed type. Is there any benefit of having it being unsigned as well?
      * One thing I see if it is MAX_UINT32 or MAX_UINT64. But it should follow whatever 
      * is the binary expression here, since it is another expression
  * HexadecimalLiteral?
      * Follow expression type!
  * BinaryLiteral?
      * Follow expression type!

* What type(kind) should binary expression have?
  * If *left* and *right* type is *explicit* and have same sign. The resulting type should keep the sign.
    * Note: 
      ```ho
      fn f(): u32 {
          var x: u32 = 1;
          var y: u32 = 1;
          return x + y; // We expect this to be u32 as well
      }
      ```
    * Same thing applies to `s32`.
  * Otherwise. it should have `UndecidedType`.
  * What happens with a binary expression of `UndecidedInt` and `DecidedInt`?
    * ~~It should have the same sign as `DecidedInt` (but with the right bounds)~~.
    * What a about?
      ```ho
      fn f(): s32 {
          var x: u32 = 1;
          return x - 2; // We expect this to be u32 as well
      }
      ```
      * Here it must widen to `s32`?
        * Then mixing `UndecidedInt` with `DecidedInt` should yield `UndecidedInt` or subtracting `Unsigned` should yield `UndecidedInt`.
    * What about mixing *Signed* with *Unsigned*?
      * It should be `UndecidedInt`.
    * What about subtracting *Unsigned* from *Unsigned*?
      * It should be `Undecided` if the min value is negative. Otherwise, keep the sign.
    