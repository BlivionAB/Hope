Division and Modulo
====================

How should we handle division and modulu emission?

Divsion generates the following IR:
```
Case x / y:
ldr x1, x // Mark x1 as busy
ldr x2, y // Mark x2 as busy
sdiv x1, x2, x1
```

Modulo generates the following IR:
```
Case x % y:
ldr x1, x // Mark x1 as busy
ldr x2, y // Mark x2 as busy
sdiv x3, x2, x1
ssub x1, x3, x2, x1
```


Combining division with modulo:
```
Case var a = x / y; var b = x % y:
ldr x1, x // Mark x1 as busy
ldr x2, y // Mark x2 as busy

// var a = x / y:
sdiv x1, x2, x1
str x1, a;

// var b = x % y:
ldr x1, x // Mark x1 as busy
ldr x2, y // Mark x2 as busy
sdiv x3, x2, x1
ssub x1, x3, x2, x1
str x1, b
```

After optimization with common sub-expression elimination:
```
Case var a = x / y; var b = x % y:
ldr x1, x // Mark x1 as busy
ldr x2, y // Mark x2 as busy

// var b = x % y:
sdiv x3, x2, x1 // It knows x1 and x2 hasn't changed and x3 is required by the next instruction so we keep this sdiv
str x3, a;
ssub x1, x3, x2, x1
str x1, b
```

The case where modulo is before division:
```
Case var a = x % y; var b = x / y:
ldr x1, x // Mark x1 as busy
ldr x2, y // Mark x2 as busy

// var b = x % y:
sdiv x3, x2, x1 // It knows x1 and x2 hasn't changed and x3 is required by the next instruction so we keep this sdiv
ssub x1, x3, x2, x1
str x1, a

// var a = x / y:
sdiv x1, x2, x1
str x1, a;
```

After optimization with common sub-expression elimination:
```
Case var a = x / y; var b = x % y:
ldr x1, x // Mark x1 as busy
ldr x2, y // Mark x2 as busy

// var b = x % y:
sdiv x3, x2, x1 // It knows x1 and x2 hasn't changed and x3 is required by the next instruction so we keep this sdiv
str x3, a;
ssub x1, x3, x2, x1
str x1, b
```

The case where there is some statements between division and modulo:
```
Case var a = x % y; var c = z; var b = x / y:
ldr x1, x // Mark x1 as busy
ldr x2, y // Mark x2 as busy

// var b = x % y:
sdiv x3, x2, x1 // It knows x1 and x2 hasn't changed and x3 is required by the next instruction so we keep this sdiv
ssub x1, x3, x2, x1
str x1, a

ldr x1, z;
str x1, c;

// var a = x / y:
ldr x1, x // x is marked unchanged by checker
ldr x2, y // y is marked unchanged by checker
sdiv x1, x2, x1 
str x1, a;
```
Since, they are marked unchanged and they are canonicalized and can be id:ed. We can do a lookup and find 
the previous sdiv and check that x3 is required because it's needed in the next instruction
```
Case var a = x % y; var c = z; var b = x / y:
ldr x1, x // Mark x1 as busy
ldr x2, y // Mark x2 as busy

// var b = x % y:
sdiv x3, x2, x1 // It knows x1 and x2 hasn't changed and x3 is required by the next instruction so we keep this sdiv
str x3, a;
ssub x1, x3, x2, x1
str x1, a

ldr x1, z;
str x1, c;
```
If the division happened before modulos. We would do a downward replacement:
```
Case var a = x % y; var c = z; var b = x / y:

// var a = x / y:
ldr x1, x // Mark x1 as busy
ldr x2, y // Mark x2 as busy
sdiv x1, x2, x1 
str x1, a;

ldr x1, z;
str x1, c;

// var b = x % y:
ldr x1, x  // x is marked unchanged by checker
ldr x2, y  // y is marked unchanged by checker
sdiv x3, x2, x1 // It knows x1 and x2 hasn't changed and x3 is required by the next instruction so we keep this sdiv
ssub x1, x3, x2, x1
str x1, a
```
Will be:
```
Case var a = x % y; var c = z; var b = x / y:
ldr x1, x // Mark x1 as busy
ldr x2, y // Mark x2 as busy

// var b = x % y:
sdiv x3, x2, x1 // It knows x1 and x2 hasn't changed and x3 is required by the next instruction so we keep this sdiv
str x3, a;
ssub x1, x3, x2, x1
str x1, a

ldr x1, z;
str x1, c;
```