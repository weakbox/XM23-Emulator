;
; Tests for Lab 3
; Tests MOVL, MOVH, MOVLZ, MOVLS, LD, and LDR
; ECED 3403
; 15 June 2023
;
DATA	equ	#9876
;
	org	#100		; ORG sets the assembler origin to #0100
;
; Value is a LABEL (ADDRESS) referring to memory location #0100
; Address #0100 has contents #9876
;
Value	word	#1234		; Memory location #0100 <- #34; #0101 <- #12
Value2	word	#5678

	org	#104
	
Value3	word	#1337

	org #00FC
	
Value4	word	#9979

;
	org	#1000		; ORG sets the assembler origin to #1000
;
Start				; Start is a label with address #1000
; 
; Use immediate constants
;
	movl	DATA,R4		; R4.LSByte <- #34
	movh	DATA,R4		; R4.MSByte <- #12
	movlz	DATA,R5		; R5.LSByte <- #34	R5.MSByte <- #FF
	movls	DATA,R6		; R6.LSByte <- #34, R6.MSByte <- #00
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

;
; Test LD instruction without increment/decrement:
;
	ld		R0,R4		; R4 <- mem[R0]
;
; Test LD instruction with increment/decrement:
;	
	ld 		R0+,R1		; R1 <- mem[R0+] (#5678)
	ld		R0-,R2		; R2 <- mem[R0-] (#1234)
;
; Test LD instruction with increment/decrement with byte addressing:
;
	ld.b	R0+,R3		; R1 <- mem[R0+] (#12)
	ld.b	R0-,R7		; R2 <- mem[R0-] (#34)
;
; Test LDR instruction with each possible type of offset:
;
	ldr		R0,$4,R1
	ldr		R0,$-4,R2
	ldr.b	R0,$4,R3
	ldr.b	R0,$-4,R4

	end	Start		; End of program, set PC to address of Start
