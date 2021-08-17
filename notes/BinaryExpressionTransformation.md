# Binary Expression

A binary expression can have any expression in left- and right-hand-side coupled with an operator.

## Problem
How to convert an ambitious binary expression, a binary expression with arbitrary expression in left and right
to assembly?

## Cases


```
Where both are memory values
x + x


One is immediate value
x + 1


When they are expressions.
x + y * a + b
```


## Solution

They are all memory values or immediate values in the end. Expression have to be 
converted to memory values before.

So either a binary expression can be boiled down to 
* memory * memory
* memory * immediate value
* immediate value * memmory


