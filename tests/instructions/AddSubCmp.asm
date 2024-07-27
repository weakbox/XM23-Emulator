VAL1 equ #0001
VAL2 equ #0001
VAL3 equ #FF01
VAL4 equ #03FF
VAL5 equ #00FF
VAL6 equ #007F

org #1000

Start

; Move value in VAL1 into R0.
movl VAL1,R0
movh VAL1,R0

; Move value in VAL2 into R1.
movl VAL2,R1
movh VAL2,R1

; Move value in VAL3 into R2.
movl VAL3,R2
movh VAL3,R2

; Move value in VAL4 into R3.
movl VAL4,R3
movh VAL4,R3

; Move value in VAL5 into R4.
movl VAL5,R4
movh VAL5,R4

; Test the ADD instruction: add <source> <dest>
add R1,R0	; We expect R0 to contain 0x0002
add.b R2,R0	; We expect R0 to contain 0x0003, as the MSByte is ignored.

; Test the SUB instruction:
sub R1,R0	; We expect R0 to contain 0x0002
sub.b R2,R0 ; We expect R0 to contain 0x0001
sub R1,R0	; We expect R0 to contain 0x0000, PSW zero bit should be set.

; ADDC and SUBC are extensions of the above functions, so we can assume they work.

; Test the CMP instruction:
cmp R0,R1	; These should not be equal.
cmp R0,R0	; These should be equal!
cmp.b R1,R2	; These should be equal, since we are only comparing the LSByte!

end	Start