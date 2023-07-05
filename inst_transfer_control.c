/*
  File: inst_transfer_control.c
  Author: Connor McLeod
  Date: July 2, 2023
  Description: Provides functions and declarations that enable the use of the transfer of control instructions found in chapter 6.4 of the XM-23 ISA.
*/

#include "header.h"
#include "instructions.h"

#define OFFSET_SIGN_BIT_LINK(x) ((x >> 12) & 1)
#define OFFSET_SIGN_BIT_COND(x) ((x >> 9 ) & 1)

// Adds an offset to the CPU's program counter. Stores the return address in the link register. 
// A return can be initiated by moving the value stored in the link register to the program counter.
void branch_link(unsigned short offset)
{
	if (OFFSET_SIGN_BIT_LINK(offset)) // Sign bit it set, we have to sign extend the most significant 3 bits of the offset.
	{
		offset |= 0xE000;
	}
	offset = offset << 1;

	LR = PC;
	PC += offset;
}

// Adds an offset to the CPU's program counter if the condition is equal to the expected value.
// The condition can be modified to fit all of the XM-23's conditional branch instructions.
void branch_conditional(unsigned short condition, unsigned short expected, unsigned short offset)
{
	if (condition == expected)
	{
		if (OFFSET_SIGN_BIT_COND(offset)) // Sign bit it set, we have to sign extend the most significant 6 bits of the offset.
		{
			offset |= 0xFC00;
		}
		offset = offset << 1;

		PC += offset;
	}
}
