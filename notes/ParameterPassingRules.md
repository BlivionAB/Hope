Parameterized Calling Convention
====

The following parameters are used for the code generator to generate the right code for different calling conventions:

* Stack Alignment
    * Need to produce right kind of alignment of stacks.
* Parameter Transfer Registers
    * Rest is passed through the stack.
* Parameter Transfer StackOrder
    * Need to specify in which order stacks are passed. First arg first or last.
* Scratch Registers
* Callee Saved Registers
