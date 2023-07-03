/*
*	Functions and definitions related to instructions that were not covered in previous labs (Assignment 1).
*	Written by Connor McLeod, for ECED 3403.
*/

#include "header.h"

// Function-like macros to isolate various bits in the instruction register.
#define offset_br(x)		((x)       & 0x1FFF)
#define offset_br_cond(x)	((x)       & 0x03FF)
#define rc(x)				((x >> 7)  & 0x0001)
#define wb(x)				((x >> 6)  & 0x0001)
#define source(x)			((x >> 3)  & 0x0007)
#define dest(x)				((x)       & 0x0007)
#define prpo(x)				((x >> 9)  & 0x0001)
#define dec(x)				((x >> 8)  & 0x0001)
#define inc(x)				((x >> 7)  & 0x0001)
#define offset_str(x)		((x >> 7)  & 0x007F)
#define lsbyte(x)			((x)	   & 0x0003)
#define msbyte(x)			((x >> 8)  & 0x0003)
#define msbit8(x)			((x >> 7)  & 0x0001)
#define msbit16(x)			((x >> 15) & 0x0001)

unsigned short lr;	// These need to be restructured.
extern unsigned short pc;

// Combines the most significant and least significant bytes of two registers into one 2-byte quantity.
unsigned short combine_bytes(unsigned short msb, unsigned short lsb)
{
	return((msb << 8) | lsb);
}

// Moves the contents of the source register into the destination register. Does not modify the PSW.
void move(unsigned short *dest, unsigned short source, unsigned short wb)
{
	switch (wb)
	{
		case WORD:
			*dest = source;
			break;

		case BYTE:
			*dest = combine_bytes(msbyte(*dest), lsbyte(source));	// MSB of the destination register is unchanged.
			break;
	}
}

// Swaps the contents of the source register and destination register. Does not modify the PSW.
void swap_reg(unsigned short *dest, unsigned short *source)
{
	unsigned int temp;	// An internal register that is inaccessible by the programmer.
	temp = *dest;
	*dest = *source;
	*source = temp;
}

// Swaps the most and least significant bytes in the destination register.
void swap_byte(unsigned short *dest)
{
	unsigned int temp;	// An internal register that is inaccessible by the programmer.
	temp = msbyte(*dest);
	*dest = combine_bytes(lsbyte(*dest), temp);
}

// Takes the complement of the destination register.
void complement(unsigned short *dest, unsigned short wb)
{
	switch (wb)
	{
		case WORD:
			*dest = ~(*dest);
			break;

		case BYTE:
			*dest = combine_bytes(msbyte(*dest), ~lsbyte(*dest));
			break;
	}
}

// Takes the most-significant bit of the least significant byte in the destination register and sign extends it, forming a 16-bit signed quantity.
void sign_extend(unsigned short*dest)
{
	switch (msbit8(lsbyte(*dest)))
	{
		case 0:
			*dest = combine_bytes(0x00, lsbyte(*dest));
			break;

		case 1:
			*dest = combine_bytes(0xFF, lsbyte(*dest));
			break;
	}
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
