VAL1 equ #AAAA
VAL2 equ #BBBB
VAL3 equ #0000

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

; Test the MOV instruction:


; Test the SWAP instruction:
swap R1,R0

; Test the SRA instruction:


; Test the RRC instruction:

end	Start
