VAL0 equ #0001

org #2000

; Tests the general operation of the cache in a very simple example.

Start

movl VAL0,R0
movh VAL0,R0

add $1,R0

bra Start

end	Start
