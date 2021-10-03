Intermediate Representation Specification
====

This documents specifies the intermediate representation of the assembly.


# Registers

| RegisterBits | Description |
| --- | ----------- |
| R0-R8 | General purpose registers |
| BP | Base pointer |
| sp | Stack pointer |

# Instructions

| Method | Operand1 | Operand2 | Description |
| --- | --- | --- | --- | --- |
| Store | R0-R8,BP,sp | Immediate immediateValue | Stores the immediateValue of Operand2 into Operand1 |
| Load | R0-R8,BP,sp | Memory Address | Stores the immediateValue of Operand2 into Operand1 |
| ReturnOperandRegister | | | ReturnOperandRegister from function |
