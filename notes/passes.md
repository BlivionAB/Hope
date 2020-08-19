Compilations Steps
====

The steps that is involved on the compiler are:

1. [Parsing/Scanning](#Parsing/Scanning)
1. [Binding](#Binding)
1. [Checking](#Checking)
1. [Transformation](#Transformation)
1. [Writing](#Writing)  

# Parsing/Scanning

We tokenize a stream of characters into tokens, which the parser then combines into the structure that represesents the grammar of the Elet programming language in an Abstract Syntax Tree(AST).

## Symbol
In the parsing step we also resolve symbol names for all declarations. We separate `name` from `identifier`, name is non-unique string used for lookup of all overloads of a function and identifier of a function is a unique string representing a specific overload(including generic resolution). Generic resolution is done in the checking step, where all the types are resolved.

# Binding 

The binder binds references to their respective declarations. For easy, declaration lookup in a different pass.

## Overload binding

We don't do the binding of call expressions in this stage. It's on the checking stage where we can do overload resolution, where we actually do the real binding.

_Note 1, the binding step cannot happen in the same step as in parsing and scanning, since we support hoisted references(you don't need to declare first before reference)_


# Checking

Checking checks for assignibility, comparability, accessability. Assignibility / comparability refers to whether a type can be assigned / compared to a different type. Accessability checks whether a property is accessible or not.


# Transformation 

Here, the AST is transformed into a _Control Flow Graph (CFG)_ in an [_Intermediate Representation (IR)_](ir.md), that is similar to any CPU instruction set architecture or shortly CPU architecture. An abstraction is made on the IR to accomodate all the different variations of CPU architectures such as aarch64, x86_64, IA-32 etc.

## Size calculation
In the transformation step, we also do the size calculation for each type. Size calculation needs to be calculated after symbols been resolved(binding step), since we need to know the size of other types, and the only way to do that is to do a lookup of the type. Size calculation also needs to be done after the checking to ensure the correct type is being used.


# Writing
The writing step writes the machine code, for the targeted OS and CPU architecture. 