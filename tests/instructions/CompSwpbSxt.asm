VAL0 equ #00FF
VAL1 equ #CC77
VAL2 equ #FF7F
VAL3 equ #00FA

org #1000

Start

; Move value in VAL0 into R0.
movl VAL0,R0
movh VAL0,R0

; Move value in VAL1 into R1.
movl VAL1,R1
movh VAL1,R1

; Move value in VAL2 into R2.
movl VAL2,R2
movh VAL2,R2

; Move value in VAL3 into R3.
movl VAL3,R3
movh VAL3,R3

; Test the COMP instruction:
comp R0		; Expected: R0 -> FF00
comp.b R0	; Expected: R0 -> FFFF

; Test the SWPB instruction:
swpb R1		; Expected: R1 -> 77CC

; Test the SXT instruction:
sxt R2;		; Expected: R2 -> 007F
sxt R3;		; Expected: R3 -> FFFA

end	Start