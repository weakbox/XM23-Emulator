VAL1 equ #0F33
VAL2 equ #0330
VAL3 equ #FF41

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

; Test the XOR instruction:
xor R1,R0	; We expect to see R0 -> 0x0C03;
xor.b R1,R0 ; We expect to see R0 -> 0x0C33;

; Test the AND instruction:
and R1,R0	; Expected: R0 -> 0x0030
and.b R0,R2	; Expected: R2 -> 0xFF00

; Test the OR instruction:
or R2,R0	; Expected: R0 -> 0xFF30
or.b R0,R3	; Expected: R3 -> 0x0030

end	Start