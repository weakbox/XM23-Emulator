;	Test if the emulator successfully recognizes an invalid address fault.

BAD_ADDR equ #1001

org #FFC0

; Initialize the instruction vector table.
; We only really care about the invalid address (9).

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

Start

movl BAD_ADDR,R1
movh BAD_ADDR,R1
mov R1,R7	; Move an odd-numbered address into the program counter.

; Create a primitive interrupt handler to document an illegal instruction.

org #9000

Interrupt_Handler

sub $1,R0	; When we encounter the invalid address we should see #FFFF in R0.
mov R5,R7	; Move LR into PC (interrupt return) due to this, we will loop faulting forever!

end Start