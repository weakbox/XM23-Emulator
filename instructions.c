/*
*	Functions and definitions related to instructions that were not covered in previous labs (Assignment 1).
*	Written by Connor McLeod, for ECED 3403.
*/

#include "header.h"
#include "instructions.h"

// Combines the most significant and least significant bytes of two registers into one 2-byte quantity.
unsigned short combine_bytes(unsigned short msb, unsigned short lsb)
{
	return((msb << 8) | lsb);
}

// Stores the value of the source register into memory at the address value located in the destination register.
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

// Stores the value of the source register into memory at the address value located in the destination register, but with a provided offset.
void store_rel(unsigned short dest, unsigned short source, short offset, unsigned short wb)
{
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
