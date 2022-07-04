Modulo IR and Optimization
====

# Background
We should discuss how to implement the modulo operation:
```
return x % y;
```

# Ideas
Our current compiler treats modulo as a separate instruction instead of a composition
of division and multiply-subtraction. Since, some compilers executes remainder operations
the same time as divisions (x86) and some do separate division and multiply subtraction 
(aarch64).

* **Moderator: Should we keep this or have compose it two different**?
  * Angel: We should keep it, since when using modulo singly. Though, during the transformation
    stage, we should transform it, if it involves a target that is not StashIR.
    * Angel: StashIR should still print remainder operations.
    * Angel: Optimizer should then optimize :
      * on x86: Reducing it to one operation if modulo follows division.
  * Devil: Though StashIR should be used to confirm optimization?
    * Angel: Maybe we can force the developers to choose one(or have default on x86?).
      * Devil: It's better to have a default.

* Moderator:
