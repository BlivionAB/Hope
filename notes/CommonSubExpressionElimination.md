```
Let's assume that the following performance timings holds
* Immediate immediateValue access < Register/Memory access < Arithmetic access

fn f1()
{
    var x = 1 + 2;
    //...
    var y = 1 + 2;    
    
    // Can we do?
    // var y = x?
}

fn f2()
{
    var x = 1 + 2;
    //...
    var x = 1 + 2;    
    
    // Can we do?
    // var x = 1 + 2?
}

fn f2()
{
    var x = a + b + c;
    //...
    var y = a + b + d;    
    
    // Can we do?
    // var tmp = a + b;
    // var x = tmp + c;
    // var y = tmp + d;
}
```

## Identifications
It will record expressions using identifiers:
  * For binary expressions:
    * (typeId + instanceId) + operatorId + (typeId + instanceId)
      * Note for literals (int) it is the immediateValue itself that is the id.
  * Variable has a variableId that is unique in local scope.

## Temporary Variable Assignment

* For each expression
  * Check if expression can be substituted with temp variables. 
    * Mark each temp variable as used
  * Assign temporary variables to all possible expression an expression can hold.
* Clean up unused temporary variables.


Please note that variable aliases will be removed:

```stash
fn f1()
{
    var x = 1 + 2;
    //...
    var y = 1 + 2;    
    
    // Can we do?
    // var y = x?
}
```

Will become this:

```stash
fn f1()
{
    var tmp = 1 + 2;
    var x = tmp;
    var y = tmp;
    // Usage of x and y.
}
```


And then after variable substitution:

```stash
fn f1()
{
    var tmp = 1 + 2;
    // Usage of tmp and tmp.
}
```

## Implementation

Should the implementation be done in AST or CFG?
* It would be sub-optimal to do it on the AST, since it would mean a rewrite of AST.
  * We could do it once we transform AST to CFG?
    * Let's try with during the transformation. An alternative, is to do it after the 
      the transformation. But, it's not optimal from a performance perspective.


