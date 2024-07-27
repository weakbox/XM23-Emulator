/*
  File: instructions.h
  Author: Connor McLeod
  Date: July 2, 2023
  Description: Provides useful macro definitions for the instruction functions.
*/

#pragma once

#define BL 0
#define BEQBZ 1
#define BNEBNZ 2
#define BCBHS 3
#define BNCBLO 4
#define BN 5
#define BGE 6
#define BLT 7
#define BRA 8
#define ADD 9
#define ADDC 10
#define SUB 11
#define SUBC 12 
#define DADD 13
#define CMP 14
#define XOR 15 
#define AND 16
#define OR 17
#define BIT 18
#define BIC 19
#define BIS 20
#define MOV 21
#define SWAP 22
#define SRA 23
#define RRC 24
#define COMP 25
#define SWPB 26
#define SXT 27
#define SETPRI 28
#define SVC 29
#define SETCC 30
#define CLRCC 31
#define CEX 32
#define LD 33
#define ST 34
#define MOVL 35
#define MOVLZ 36
#define MOVLS 37
#define MOVH 38
#define LDR 39
#define STR 40

// Isolates specific bits present in the instruction register.
// -> These definitions allow the emulator to decode the opcode present in the instruction register.
#define BITS_15_AND_14(x)	((x >> 14) & 0x03)
#define BIT_13(x)			((x >> 13) & 0x01)
#define BITS_12_AND_11(x)	((x >> 11) & 0x03)
#define BITS_12_TO_10(x)	((x >> 10) & 0x07)
#define BIT_10(x)			((x >> 10) & 0x01)
#define BITS_9_AND_8(x)		((x >> 8)  & 0x03)
#define BITS_9_TO_7(x)		((x >> 7)  & 0x07)
#define BITS_6_AND_5(x)		((x >> 5)  & 0x03)
#define BITS_5_TO_3(x)		((x >> 3)  & 0x07)
#define BIT_4(x)			((x >> 4)  & 0x01)

// Isolates specific bits present in the instruction register.
// -> These definitions allow the emulator to extract parameters from the instruction register.
#define OFFSET_BRANCH_LINK(x)		((x)       & 0x1FFF)
#define OFFSET_BRANCH_COND(x)		((x)       & 0x03FF)
#define RC(x)						((x >> 7)  & 0x01)
#define WB(x)						((x >> 6)  & 0x01)
#define SOURCE(x)					((x >> 3)  & 0x07)
#define DEST(x)						((x)       & 0x07)
#define PSW_BITS(x)					((x)       & 0x1F)
#define PRPO(x)						((x >> 9)  & 0x01)
#define DEC(x)						((x >> 8)  & 0x01)
#define INC(x)						((x >> 7)  & 0x01)
#define BYTE_MOV(x)					((x >> 3)  & 0xFF)
#define OFFSET_LOAD_STORE_REL(x)	((x >> 7)  & 0x7F)
#define FALSE(x)					((x)       & 0x07)
#define TRUE(x)					    ((x >> 3)  & 0x07)
#define CODE(x)						((x >> 6)  & 0x0F)
#define PRIORITY(x)					((x)       & 0x07)
#define INTERRUPT_VECTOR(x)			((x)       & 0x0F)
#define LSBYTE(x)					((x)       & 0xFF)
#define MSBYTE(x)					((x >> 8)  & 0xFF)
#define MSBIT_WORD(x)				((x >> 15) & 0x01)
#define MSBIT_BYTE(x)				((x >> 7)  & 0x01)
#define LSBIT_WORD(x)				((x)       & 0x01)
#define LSBIT_BYTE(x)				((x)       & 0x01)

// Isolates specific bits in the PSW for the SETCC and CLRCC instrctions.
#define CARRY(x)	((x) & 0x01)
#define ZERO(x)		((x >> 1) & 0x01)
#define NEGATIVE(x)	((x >> 2) & 0x01)
#define SLEEP(x)	((x >> 3) & 0x01)
#define OVERFLOW(x) ((x >> 4) & 0x01)

#define PRE	 0
#define POST 1
