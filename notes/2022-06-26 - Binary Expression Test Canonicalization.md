Binary Expression Tests Canonicalization and Taxanomization
====

# Problem
How should we canonicalize and taxonomize tests in binary expression? I.e. how to group and determine their order?

# Taxanomization & Canonicalization:
* Group Integer:
  * Group by operand mix (immediate with immediate, register with immediate, register with register).
    * For each operand mix:
      * Canonicalize by operand (Addition, Subtraction, Multiplication, Division, Remainder, BitwiseAnd, BitwiseOr, BitwiseXor).
        * For each operand:
          * Canonicalize by int type (s32, s64), except for division and remainder where we include u32 and u64 i.e. (s32, s64, u32, u64).
  * Group by overflow and underflow checks:
      * For each overflow check:
        * Canonicalize by operand (Addition, Subtraction, Multiplication, Division, Remainder, And, Or, Xor).
          * For each operand:
            * Canonicalize by int type (s32, s64), except for division and remainder where we include u32 and u64 i.e. (s32, s64, u32, u64).
  * Group: Incompatibility 
    * Integer with non Integer.
    * Non-compatible operator.
* Group Boolean:
  * Group by operand mix (immediate with immediate, register with immediate, register with register):
    * For each operand mix
      * Canonicalize by operand (And, Or, Xor).
        * For each operand:
          * Check True * True, True * False, False * True, False * False.
  * Group: Incompatibility
    * Integer with non Integer.
    * Non-compatible operator.
* Group: Incompatibility
  * Missing 1st operand.
  * Missing 2nd operand.
* Group: Precedence:
  * Group: BinaryExpression Induction:
    1. highest * lowest // parseBinaryExpression
    2. lowest * highest
    3. lowest * middle * highest // parseRHS
    4. lowest * highest * middle
  * From lowest to highest precedence operator:
    * Check next >= lower.