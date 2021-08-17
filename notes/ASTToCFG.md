AST to CFG
====

To generate a Control Flow Graph from an Abstract Syntax Tree is quite a complex task. And it's a task that is required. Since, the machine code is a graph structure.

1. Classify types.
    * Note, parameter passing with classified types changes the types, but not the data.
1. Generate procedures that conforms to calling conventions.
    * If `SysV`, use `SysV` calling convention.
    * If `cstdcall`, us `cstcall` etc.
    * ...

[Q]
When should we generate CFG?

* One idea is as soon as possible?
    * If soon as possible we risk allocate too much?
        * But, it's more convenient to do tree shaking on CFG.
    * We need to visit a CFG for type checking anyway? 
        * Nope, we only need a tree structure for this. Since, it only need to figure out the branches from the same base if they are all being assigned or not. Etc.
    * Decision: Do CFG type narrowing during checking phase. Also do a last transformation
    after the CFG checking to do consolidation optimizations.
    
`[Q]`
Where do we do tree shaking on AST or CFG?

* We would need it for CFG anyway? Since calling conventions can be reduced.

    * `[Q]` If, we do it in the CFG. Can we source map the AST?
        * We don't do tree shaking on O0 optimization. And it's only then source map is needed.

    * `[S]` We should probably do it on type checking as well, because we are already traversing it there. In that case, we can mark nodes that are dead store, common expressions. etc.
        *  But it won't catch everything. But we should do as much as possible in each pass.
            * What won't it catch?
                * Low-level calling convention redundance only? Like parameter allocations and then assigning ti on return statements?
                    * We could probably optimize this as well? Worst case is to add another pass.
                * Inlining will be more cumbersome to do on the AST rather than the CFG. It's easer to rewrite CFG and then AST we think.

`[Q]` Do we have to have a representation that is abstract for calling convention and one that is abstract for machine code?
* I think only one representation is enough, the one that abstracts machine code. The rest can be generated directly from AST.
* What about making the calling convention parametric? Meaning calling convention relying on the parameter of "parameters passing registers", "return registers", "classification" etc?
* `[S]` We should at least have the MachineCodeWriters to only know some basic "rules". Such as, `R0` maps to `eax` etc.

Decision:

* Checking phase will add the following tasks:
    * Flagging
        * Type narrowing.
        * Common sub-expression flagging.
        * Un-use flagging.
    * There is no need to construct a CFG to accomplish this. Better done with AST.
    
* Optimization phase will be:
    * Tree-shaking (from un-use flagging).
    * Common sub-expression elimination.
    * Inlining
    * Consolidations.
        * If two instruction can be merged (like store in subsequent load instructions), it's done here.
    * Arithmetic optimizations
        * Example:  times 2 could be optimized with a bit shift left.
    
    