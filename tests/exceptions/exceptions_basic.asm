; Test if the instruction vector is correctly intialized.
; Test operation of supervisory calls.
; Test return of nested supervisory calls.

org #FFC0

; Initialize the worst instruction vector table ever.

word #0000	; PSW
word #0000	; Addr

word #0020	
word #1000

word #0040
word #2000

word #0060
word #3000

word #0080
word #4000

word #00A0
word #5000

word #00C0
word #6000

word #00E0
word #7000

word #00E0
word #8000

word #00E0
word #9000

word #00E0
word #A000

word #00E0
word #B000

word #00E0
word #C000

word #00E0
word #D000

word #00E0
word #E000

word #00E0
word #F000

org #1000

; Try to execute an interrupt from the interrupt vector table.

Start

svc $2		; We should move to address #2000.

bra Start

; Create a primitive interrupt handler.

org #2000

Interrupt_Handler

add $1,R0
add $1,R0
add $1,R0
svc $4		; We should move to address #4000.
mov R5,R7	; Move LR into PC (interrupt return).

org #4000

add $1,R1
add $1,R1
add $1,R1
mov R5,R7	; Move LR into PC (interrupt return).

end Start
