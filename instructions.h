/*
  File: instructions.h
  Author: Connor McLeod
  Creation Date: July 2, 2023
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

#define LSBYTE(x)     ((x) & 0xFF)
#define MSBYTE(x)     (((x) >> 8) & 0xFF)
#define MSBIT_WORD(x) (((x) >> 15) & 0x0001)
#define MSBIT_BYTE(x) (((x) >> 7) & 0x0001)
