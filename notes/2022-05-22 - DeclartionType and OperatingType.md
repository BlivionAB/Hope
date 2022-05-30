DeclarationType and OperatingType
======

`Hope` requires all variables to be declared with a type:

```
var a: u32; 
```

And the corresponding `OperationType` is given by the `DeclarationType`.

```
var a: u32 = 12; 
             ~~ optype = u32
```
```
var a: u32 = 12 + 13;
             ~~~~~~~ optype = u32
```
What happens when the operation contains mixed types?

When one of the operands has a smaller type than the declaration type. It will not throw any errors, 
unless it overflows.

```
var a: u32 = 12;
var b: u64 = 12 + a; // no error
             ~~~~~~~ optype = u64
```
If the opposite occurs. It first has to boe downcasted (even though the operation fits into the type).
```
var a: u64 = 1;
var b: u32 = 12 + a; 
                  ~ error: Operand type u64 is not operable with operation type u32;
```
```
var a: u64 = 1;
var b: u32 = 12 + (a as u32);
```

Note, also that `DeclarationType` is defined as the return type for return statements:
```
fn f(): u32 
{
    return 1 + 2;
           ~~~~~ optype = u32
}
```