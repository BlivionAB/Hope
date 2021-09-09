# Implicit Integer Conversion

* If both operands are of the same type. The type is the common type.
* Else the types are different.
  * If the types have the same signedness (both signed or unsigned). The operand whose 
    type has the lesser conversion rank is implicitly converted to the other type.
  * Else, the unsigned type has conversion rank less than the signed type:
    * If the signed type can represent all values of the unsigned type, then the
      operand with the unsigned type is implicitly converted to the signed type.
    * Else, both operands undergo implicit conversion to the unsigned type 
      counterpart of the signed operand's type.

Conversion Ranking
