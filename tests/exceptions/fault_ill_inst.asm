;	Test if the emulator successfully recognizes an illegal instruction fault.

org #FFC0

; Initialize the instruction vector table.
; We only really care about the illegal instruction (8).

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


org #1004

word #4DA0	; Put an illegal instruction at address 1004.

org #1000

Start

add $0,R0
add $0,R0
; Bad instruction.

; Create a primitive interrupt handler to document an illegal instruction.

org #8000

Interrupt_Handler

sub $1,R0	; When we encounter the illegal instruction we should see #FFFF in R0.
mov R5,R7	; Move LR into PC (interrupt return).

end Start