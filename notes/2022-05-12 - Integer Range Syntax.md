Integer Range Syntax
======

Syntax for defining a *IntegerRange*:

```
var a: int? 1..10;
```

This defines the syntax to be an integer between 1 and 10.

A question arises if we should allow the syntax to implicitly denote an integer range?:

```
var a: 1..10
```

* Devil: 
  * The implicit integer type.
  * What about nullable?
    * Angel:
      * var a?: 1..10;
        * Devil: We want to signify `undefined` from `null` with `var a?` for `undefined` and `var a: int?`.
          * Angel: When do we want `undefined`?
            * Devil: For example in `createSyntax` it populates `start` but not `end`. It's later populated 
              with `end` with `finishSyntax`.
              * Angel: So then `var a: 1..10?`
                * Devil: Yes!.
* Angel:
  * Short syntax for a very common type.
  * Since, float is not able to be a range of digits. The implicit becomes explicit.
    
