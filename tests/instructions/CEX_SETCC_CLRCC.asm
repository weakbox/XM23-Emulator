GOOD equ #0001
BAD  equ #FFFF

org #2000 

Start

; Test SETCC and CLRCC instructions:

SETCC CZSN	; Expected: All PSW Bits except for V should be set.
CLRCC NVS	; Expected: All PSW bits except for C and Z should be cleared.

; Test CEX instruction:

CEX PL,$2,$2	; Expected: Successful comparasion.

movl GOOD,R0
movh GOOD,R0

movl BAD,R0
movh BAD,R0

CEX LT,$2,$2	; Expected: Unsuccessful comparasion.

movl BAD,R1
movh BAD,R1

movl GOOD,R1
movh GOOD,R1

end Start
