/*
  File: inst_misc.c
  Author: Connor McLeod
  Date: July 2, 2023
  Description: Provides functions and declarations that enable the use of the register exchange and single register instructions found in chapters 6.2-6.3 of the XM-23 ISA.
*/

#include "header.h"
#include "instructions.h"

// Moves the contents of the source register into the destination register.
// Does not modify the PSW.
unsigned short move(unsigned short dest, unsigned short source, unsigned short wb)
{
	unsigned short result = 0;
	switch (wb)
	{
	case WORD:
		result = source;
		break;

	case BYTE:
		result = combine_bytes(MSBYTE(dest), LSBYTE(source));	// MSB of the destination register is unchanged.
		break;
	}
	return result;
}

// Swaps the contents of the source and destination registers.
// Does not modify the PSW.
void swap_reg(unsigned short* dest, unsigned short* source)
{
	unsigned short temp;	// An internal register that is inaccessible by the programmer.

	temp = *dest;
	*dest = *source;
	*source = temp;
}

// Swaps the most and least significant bytes in the destination register.
// Does not modify the PSW.
unsigned short swap_byte(unsigned short dest)
{
	unsigned short result;
	unsigned short temp;

	temp = MSBYTE(dest);
	result = combine_bytes(LSBYTE(dest), temp);

	return result;
}

// Takes the complement of the destination register.
// Does not modify the PSW.
unsigned short complement(unsigned short dest, unsigned short wb)
{
	unsigned short result = 0;
	switch (wb)
	{
	case WORD:
		result = ~(dest);
		break;

	case BYTE:
		result = combine_bytes(MSBYTE(dest), ~LSBYTE(dest));
		break;
	}
	return result;
}

// Takes the most-significant bit of the least-significant byte in the destination register and sign extends it, forming a 16-bit signed quantity.
// Does not modify the PSW.
unsigned short sign_extend(unsigned short dest)
{
	unsigned short result = 0;
	switch (MSBIT_BYTE(LSBYTE(dest)))
	{
	case 0:
		result = combine_bytes(0x00, LSBYTE(dest));
		break;

	case 1:
		result = combine_bytes(0xFF, LSBYTE(dest));
		break;
	}
	return result;
}
