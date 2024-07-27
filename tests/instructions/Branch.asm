VAL0 equ #0001
VAL1 equ #FFFF

org #2000

Start

Branch_BEQBZ

; Test the BEQBZ instruction:
movl VAL0,R0
movh VAL0,R0
cmp $2,R0	; Instruciton will cause PSW's Z bit to be cleared.
beq Branch_BNEBNZ	; Expected: Failed branch.

cmp $1,R0	; Instruciton will cause PSW's Z bit to be set.
beq Branch_BNEBNZ	; Expected: Successful branch.

org #2100

Branch_BNEBNZ

; Test the BNEBNZ instruction:
bne Branch_BCBHS	; Expected: Failed branch.

cmp $2,R0	; Instruciton will cause PSW's Z bit to be cleared.
bne Branch_BCBHS	; Expected: Successful branch.

org #2200

Branch_BCBHS

; Test the BCBHS instruciton:
bc Branch_BNCBLO	; Expected: Failed branch.

movl VAL1,R1
movh VAL1,R1
add $4,R1		; Cause the PSW's carry bit to set.
 
bc Branch_BNCBLO	; Expected: Successful branch.

org #1F00

Branch_BNCBLO

; Test the BNCBLO instruciton:
bnc Branch_BRA	; Expected: Failed branch.

add $1,R0	; Clears the PSW's carry bit.

bnc Branch_BRA	; Expected: Successful branch.

org #22CC

Branch_BRA

bra Branch_BNEBNZ

end	Start
