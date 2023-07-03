/*
*	Functions and definitions related to instructions that were not covered in previous labs (Assignment 1).
*	Written by Connor McLeod, for ECED 3403.
*/

#include "header.h"
#include "instructions.h"

#define OFFSET_SIGN_BIT_LOAD_STORE(x) ((x >> 13) & 1)

// Combines the most significant and least significant bytes of two registers into one 2-byte quantity.
unsigned short combine_bytes(unsigned short msb, unsigned short lsb)
{
	return((msb << 8) | lsb);
}

// Stores the value of the source register into memory at the address value located in the destination register.
// Can decrement/increment the destination register to allow for indexed addressing. 
void store(unsigned short* dest, unsigned short source, unsigned short prpo, unsigned short dec, unsigned short inc, unsigned short wb)
{
	switch (prpo)	/* Determine whether to dec/inc the source register pre/post load: */
	{
	case PRE:
		switch (wb)
		{
			case WORD:
				*dest -= (2 * dec);
				*dest += (2 * inc);
				mar = *dest;
				mbr = source;
				bus(mar, &mbr, WRITE, WORD);
				break;

			case BYTE:
				*dest -= dec;
				*dest += inc;
				mar = *dest;
				mbr = source;
				bus(mar, &mbr, WRITE, BYTE);
				break;
		}
		break;

	case POST:
		switch (wb)
		{
			case WORD:
				mar = *dest;
				mbr = source;
				bus(mar, &mbr, WRITE, WORD);
				*dest -= (2 * dec);
				*dest += (2 * inc);
				break;

			case BYTE:
				mar = *dest;
				mbr = source;
				bus(mar, &mbr, WRITE, BYTE);
				*dest -= dec;
				*dest += inc;
				break;
		}
		break;
	}
}

// Stores the value of the source register in memory at the address value specified by the destination register, but with a provided offset.
// This offset is sign extended to ensure correct operation.
void store_rel(unsigned short dest, unsigned short source, short offset, unsigned short wb)
{
	if (OFFSET_SIGN_BIT_LOAD_STORE(offset)) // Sign bit it set, we have to sign extend the most significant 9 bits of the offset.
	{
		offset |= 0xFF80;
	}
	offset = offset << 1;

	switch (wb)
	{
		case WORD:
			mar = dest + offset;
			mbr = source;
			bus(mar, &mbr, WRITE, WORD);
			break;

		case BYTE:
			mar = dest + offset;
			mbr = source;
			bus(mar, &mbr, WRITE, BYTE);
			break;
	}
}
