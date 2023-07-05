VAL0 equ #FF00
VAL1 equ #0080
VAL2 equ #8000
VAL3 equ #FFFF

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

; Test the BIT instruction:
bit R1,R0	; Expected: 0080 & FF00 No match. Zero bit -> set.
bit R2,R0	; Expected: 8000 & FF00 Match! Negative bit -> set.
bit.b R1,R1	; Expected: 0080 & 0080 Match! Negative bit -> set.

; Test the BIC instruction:
bic $4,R3	; Expected: ? Ask Hughes about this one. 

; Test the BIS instruction:
bis $4,R3	; Expected: ? Ask Hughes about this one. 

end	Start
