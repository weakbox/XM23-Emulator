; Tests the cache's integration with previously tested instructions.

DATA equ #2121
ADDR1 equ #2100
ADDR2 equ #2120

org	#1000

Start

movl DATA,R0
movh DATA,R0

movl ADDR1,R1
movh ADDR1,R1

movl ADDR2,R2
movh ADDR2,R2

; Test ST instruction.

st R0,R1	; #2121 should be stored at memory address #2100.
st.b R0,R2	; #0021 should be stored at memory address #2120.

; Test LD instruction.

ld R1,R1	; #2121 should be loaded from memory address #2100.
ld.b R2,R2	; #0021 should be loaded from memory address #2120.

bra Start

end Start
