# MIR

MIR is based on SSA. So, no double assignment on variables. Types/Sizes are based on the register.

##Variables
```
Var64
Var32
```

##Instructions
```
Str

# Comparisons 
CmpEq 
CmpGt
CmpGtEq
CmpLt
CmpLtEq


# Jumps
Call
Jmp
Ret

# Arithmetic
Add
Sub
Mul
Div
```
### Examples
```
multiply:
Str X1, Param1
Str X2, Param2
Mul X3, X1, X2
Ret X3

Str X1, 1
Str X2, 2
Call X3, multiply(X1, X2)
```