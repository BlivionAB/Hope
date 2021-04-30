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
Param

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

# Binary
SHL
SHR
```
### Examples

MIR1
```
multiply:
Par P1
Par P2
Str L1, P1
Str L2, P2
Mul L3, L1, L2
Str R, L3
Ret 

Str L1, 1
Str L2, 2
Arg P1, L1
Arg P2, L2
Call multiply
```

2nd stage(without optimization).
```
multiply:

# Function pre
Push BP
Str SP, BP

# Parameter assignments
Str [BP - 8], P1 // Param P1
Str [BP - 16], P2 // Param P2

# Scratch assignments
Str L1, [BP - 8] // Str L1, P1
Str L2, [BP - 16] // Str L2, P2

# Operation
Mul L1, L1, L2

# Function post
Str R, L1
Pop BP
Ret

Str L1, 1
Str L2, 2
Str P1, L1
Str P2, L2
Call multiply
Str L1, R
```
### Phi-function insertions
```
using Common::Crypto::PseudoRandomNumberGenerator::{ CreateRandomNumber };
using Common::Console::{ WriteLine };

fn OnProcessStart(): void
{
    var a: number = 0;
    if (CreateRandomNumber() > 0.5) 
    {
        if (CreateRandomNumber() > 0.5) 
        {
            a = 2;
        }
        else
        {
            a = 1;
        }
    }
    else
    {
        a = 0;
    }
    WriteLine(a);
}
```

1:st step:
```
var a: number = 0;
if (CreateRandomNumber() > 0.5) 
{
}
else
{
}
WriteLine(a);
```
The block inside if and else statements are "walked back" to parent block's next block.
```
var a: number = 0;
if (CreateRandomNumber() > 0.5) 
{
    if (CreateRandomNumber() > 0.5) 
    {
    }
    else
    {
    }
}
else
{
    a = 0;
}
WriteLine(a);
```

The block inside if and else statements are "walked back" to parent block's next block.
```
var a: number = 0;
if (CreateRandomNumber() > 0.5) 
{
    if (CreateRandomNumber() > 0.5) 
    {
        a = 3;
    }
    else
    {
        a = 1;
    }
}
else
{
    a = 0;
}
WriteLine(a);
```

It will generate the following CFG:
```
_OnProcessStart:

Block1:
Var32 a_0, 0
Call _CreateRandomNumber
Str a_1, R
Jumps: [
    Jump {
        conditions: [
            Jg a_1, 0,
        ],
        target: Block3
    }
    Jump {
        conditions: [],
        target Block4
    }
]
Next: Block2

Block2
Arg P1, a_1
Call _WriteLine
```

Stage 2:
```
_OnProcessStart:

Block1:
Var32 a_0, 0
Call _CreateRandomNumber
Str a_1, R
Jumps: [
    Jump {
        conditions: [
            Jg a_1, 0,
        ],
        target: Block3
    }
    Jump {
        conditions: [],
        target Block4
    }
]
Next: Block2

BLock3:
Jumps: [
    Jump {
        conditions: [],
        target: Block2
    }
]


BLock4:
Str a_2, 0
Jumps: [
    Jump {
        conditions: [],
        target: Block2
    }
]

Block5:
Str a_3, 3
Jumps: [
    Jump {
        conditions: [],
        target: Block3
    }
]

Block6:
Str a_4, 1
Jumps: [
    Jump {
        conditions: [],
        target: Block3
    }
]

Block2:
Arg P1, a_1
Call _WriteLine
```
Stage 3: Note, Block3 jumps to Block2 and bringing `a_3` and `a_4`, Block4 jumps to Block2, bringing `a_2`, since Block2 uses `a` it has to be rewritten:

```
Block2:
Str a_5, Phi(a_2, a_3, a_4)
Arg P1, a_5
Call _WriteLine
```


It will generate the following opcodes:

```
_OnProcessStart:
Var32 L1, 0
Call _CreateRandomNumber
Str L1, R
Cmp L1, 0
Jg B1
Str L1, 0
B1:
Str L1, 1
Arg P1, L1
Call _WriteLine
```

After SSA & Phi function pass
```
_OnProcessStart:
Var32 L1
Call _CreateRandomNumber
Str L1_2, R
Cmp L1_2, 0
Jg B1
Str L1_3, 0
B1:
Str L1_4, 1
Str L1_5, Phi(L1_3, L1_4)
Arg P1, L1
Call _WriteLine
```

### Optimizations
####Inlining

```
multiply:
Par P1
Par P2
Str L1, P1
Str L2, P2
Mul L3, L1, L2
Str R, L3
Ret 

Str L1, 1
Str L2, 2
Arg P1, L1
Arg P2, L2
Call multiply
```
1 stage: Inline
```
Str L1, 1
Str L2, 2
Arg P1, L1
Arg P2, L2
Par P1
Par P2
Str L1, P1
Str L2, P2
Mul L3, L1, L2
Str R, L3
```

2nd stage: Remove forward indirections
```
Mul L3, 1, 2
Str R, L3
```
3rd stage: Remove backward indirection:
```
Mul R, 1, 2
```
