Intermediate Representation Specification
====

This documents specifies the intermediate representation of the assembly.


# Registers

| OpCodeRegister | Description |
| --- | ----------- |
| R0-R8 | General purpose registers |
| BP | Base pointer |
| SP | Stack pointer |

# Instructions

| Method | Operand1 | Operand2 | Description |
| --- | --- | --- | --- | --- |
| Store | R0-R8,BP,SP | Immediate value | Stores the value of Operand2 into Operand1 |
| Load | R0-R8,BP,SP | Memory Address | Stores the value of Operand2 into Operand1 |
| Return | | | Return from function |
