VAL0 equ #5321
VAL1 equ #1234
VAL2 equ #5555
VAL3 equ #0609

org #1C06

Start

movl VAL0,R0
movh VAL0,R0

movl VAL1,R1
movh VAL1,R1

dadd R1,R0	; SRC,DST Expected: R0 -> 6555

movl VAL2,R0
movh VAL2,R0

movl VAL3,R1
movh VAL3,R1

dadd R1,R0	; Expected: R0 -> 6164

dadd.b R1,R0	; Expected: R0 -> 0073

end Start
