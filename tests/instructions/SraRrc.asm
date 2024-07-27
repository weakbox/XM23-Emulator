VAL0 equ #0800
VAL1 equ #8001

org #1000

Start

movl VAL0,R0
movh VAL0,R0

; Test SRA instruction.

sra R0		; Expected: R0 -> 0400
sra.b R0 	; Expected: R0 -> 0400

movl VAL1,R1
movh VAL1,R1

; Test RRC instruction.

rrc R1		; Expected: R1 -> 4000 PSW.C -> 1

; The carry bit will be "rotated" into the msbit of the lsbyte of the dest.
rrc.b R1	; Expected: R1 -> 4080 PSW.C -> 0

end	Start
