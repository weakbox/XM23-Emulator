DATA equ #2121
ADDRW equ #2100
ADDRB equ #2200

org #2000

; Tests how the cache handles writes.

Start

movl DATA,R0
movh DATA,R0

movl ADDRW,R1
movh ADDRW,R1

st R0,R1

movl ADDRB,R1
movh ADDRB,R1

st.b R0,R1

bra Start

end	Start