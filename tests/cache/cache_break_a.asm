; Demonstrate how associative cache organization can be very ineffective
; for a given situation.

org	#1000

Start

add $1,R0	; Due to there being more instructions than there are cache lines, we will never
add $1,R0	; Experience a hit using this organization.
add $1,R0
add $1,R0
add $1,R0
add $1,R0
add $1,R0
add $1,R0
add $1,R0
add $1,R0

add $1,R0
add $1,R0
add $1,R0
add $1,R0
add $1,R0
add $1,R0
add $1,R0
add $1,R0
add $1,R0
add $1,R0

add $1,R0
add $1,R0
add $1,R0
add $1,R0
add $1,R0
add $1,R0
add $1,R0
add $1,R0
add $1,R0
add $1,R0

add $1,R0
add $1,R0

bra Start

end Start