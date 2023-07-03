;
; Register initialization
; Initialize R4 to #1234 (hex 1234)
; ECED 3403
; 10 May 2023
;
DATA	equ	#6789
;
	org	#100		; ORG sets the assembler origin to #0100
;
; Value is a LABEL (ADDRESS) referring to memory location #0100
; Address #0100 has contents #1234
;
Value	word	#1234		; Memory location #0100 <- #34; #0101 <- #12
;
	org	#1000		; ORG sets the assembler origin to #1000
;
Start				; Start is a label with address #1000
; 
; Use immediate constants
;
	movl	DATA,R4		; R4.LSByte <- #34
	movh	DATA,R4		; R4.MSByte <- #12
;
; Use a memory location
; Takes two steps:
; 1. Move a memory address into a register (R0 in this case)
;
	movl	Value,R0	; R0.LSByte <- #00
	movh	Value,R0	; R0.MSByte <- #01
;
; 2. Load R4 from location specified in R0 (#0100)
;
	ld	R0,R4		; R4 <- mem[R0]
;
	end	Start		; End of program, set PC to address of Start
