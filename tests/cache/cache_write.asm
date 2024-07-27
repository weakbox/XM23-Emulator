; Tests the cache's writing capabilities.
; Performs a small loop which continually performs writes at addresses that will cause
; write-back policy to trigger.

DATA equ #2121
ADDR1 equ #2100
ADDR2 equ #2120

org	#1000

Start

org #1000

movl DATA,R0
movh DATA,R0

movl ADDR1,R1
movh ADDR1,R1

movl ADDR2,R2
movh ADDR2,R2

bra Loop

org #1100

Loop

st R0,R1	; This write will constantly be evicted from the cache in DM mode.
st R0,R2	; This write will stay in the cache.

bra Trigger

org #1200

Trigger

add $1,R0	; This instruction will remove the first write from the cache when in DM mode.
			; It will also change the ascii symbol in memory.
st R0,R2	; This write will stay in the cache.

bra Loop

end Start