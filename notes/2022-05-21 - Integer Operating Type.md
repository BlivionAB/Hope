Integer Operating Type
======

Trivial cases for operating type:
```
fn f(): u32 {
    var x: u32 = 1;
    var y: u32 = 2;
    return x + y;
           ~~~~~  optype: u32
}
```

```
fn f(): s32 {
    var x: s32 = 1;
    var y: s32 = 2;
    return x + y; 
           ~~~~~  optype: s32
}
```

Non-trivial cases for operating type:

It should be able to combine within the same sign type. Though the overall operating type
is the largest type.

`s32` and `s64`:
```
fn f(): s32 {
    var x: s32 = 1;
    var y: s32 = 2;
    var z: s64 = 3;
    return z + x + y;
               ~~~~~ optype: s32
           ~~~~~~~~~ optype: s64
               
}
```
Vice versa, if `s64` is on right side.

`u64` and `s64`:
```
fn f(): u32 {
    var x: u32 = 1;
    var y: u32 = 2;
    var z: u64 = 3;
    return z + x + y;
               ~~~~~ optype: u32
           ~~~~~~~~~ optype: u64
               
}
```
Vice versa, if `u64` is on right side.

A single declared type defines the whole expression's operating type:

```
fn f(): s32 {
    var x: s32 = 1;
    return 1 + 2 + x;
               ~~~~~ optype: s32
           ~~~~~~~~~ optype: s32
}
```

If two types are of different sign: 
```
fn f(): s32 {
    var x: s32 = 1;
    var y: u64 = 3;
    return y + x;
           ~~~~~~ error: `s32` is not related to `u64`. 
                     help: cast `s32` to `u64` or vice versa. 
}
```

We have to either cast `s32` to `u64` or vice versa:
```
fn f(): s32 {
    var x: s32 = 1;
    var y: u64 = 3;
    return y + (x as u64);
}
```
Note, that we still track overflows
```
fn f(): s32 {
    var x: s32 = 4;
    var y: u64 = 3;
    return y - (x as u64);
           ~~~~~~~~~~~~~~ error: Expression underflows u64.
}
```