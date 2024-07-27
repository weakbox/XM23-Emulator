VAL0 equ #4000
VAL1 equ #4002
VAL2 equ #FFFF

org #4000

word #4345
word #4445	; Put "ECED" into memory starting at address 4000.

org #2000

Start

movl VAL0,R0
movh VAL0,R0

movl VAL1,R1
movh VAL1,R1

ld R0,R2	; Expected: R2 -> 4345
ld R1,R3	; Expected: R3 -> 4445

movl VAL2,R1
movh VAL2,R1

st R1,R0	; Expected: Memory address 4000 -> FFFF

end Start
