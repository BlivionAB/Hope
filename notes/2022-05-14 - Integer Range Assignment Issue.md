Integer Range Reassignments
======

Given the problem:
```
var a: s32 = 5;
a = 6;
```
What should happen on the reassignment?

Angel: No Error on reassignment.
Devil: Error on reassignment.

* Us: Currently, the initial assignments sets the bounds to 5. Thus, overflowing the bounds should error.
  * Devil: Yes!
    * Angel: Though, this is not conformant with current programming languages. And could be hard 
      for new programmers to enter Hope.
      * Devil: If we increase the bounds to `s32min..s32max` any operations will cause an overflow.
        * Angel: We should split between *type (with bounds)* and *value track*. The *value track* tracks
          the value, so it is within type bounds. The `s32` should have the default bounds `s32min..s32max`.
    
* Us: Then, we should not widen types(for example s32 -> s64 when we add maxint32 + 1) at all and just error? 
  * Angel: Yes, `IntegerOverflow` for the specific type. 
    * Us: Sounds reasonable!

Us: What about?
```
var a: s32 = 5;
var b: s32 = 6;
b = b * a; // ???????
```
* Angel: The last line shouldn't error. Since, the value bounds are within the type bounds.
  * We should use the terms allocations and values. Since, we allocate values in which the 
    values could hold! And totally discard the term type!
    * Devil: Nobody uses allocation!
      * Us: Let's use type. We don't want to confuse new users, even though allocation is 
        the correct term.
  
Us: What about?
```
fn f(a: s32, b: s32): s32
{
    var c: s32;
    c = b * a; // ???????
}
```
* Angel: It should error, since it isn't able to track values it defaults to allocation bounds 
  `s32min..s32max`.
* Devil: one question that arises if any function should define their parameter bounds. They will
  become to specialized requiring multiple specialization.
  ```
  fn f(a: s32 1..3, b: s32 1..3 ): s32 1..9
  {
      var c: s32;
      c = b * a; // ???????
  }
  ```
  * Angel: One thing to accommodate this is to introduce generics:
    ```
    fn f<s:T,s:U>(a: s32 T, b: s32 U): s32 ?
    {
        var c: s32;
        c = b * a; // ???????
        return c;
    }
    ```
