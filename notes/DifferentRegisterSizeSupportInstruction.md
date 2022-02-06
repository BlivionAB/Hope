How to handle different register size instruction support?
==================================================

Alpha has the design where the transformation is flexible meaing the generated IR 
has a wide support for different kind of instructions.

In ARM and x86 for instance a division is only supported in 32 and 64 bits. What
happens then when you do 1 byte division? 
  * They just convert the 1 byte to 4 byte by zero-extension in x86. (similar in ARM)
  
The transformer has to know what the next instruction has for supported register 
sizes, in order to know how to transform the size.

Every operation consist of load, binop, store. I think we can assume the load and store 
has all register size support, and we just have to look at the binop supported register 
sizes.