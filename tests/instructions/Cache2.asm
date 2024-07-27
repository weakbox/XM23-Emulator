DATA equ #2121
ADDR equ #2100

org #2000

; Tests how the cache handles writes.

Start

movl DATA,R0
movh DATA,R0

movl ADDR,R1
movh ADDR,R1

st R0,R1

bra Start

end	Start