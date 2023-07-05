/*
  File: inst_memory.c
  Author: Connor McLeod
  Date: July 2, 2023
  Description: Provides functions and declarations that enable the use of the memory access instructions found in chaper 6.1 of the XM-23 ISA.
*/

#include "header.h"
#include "instructions.h"

#define OFFSET_SIGN_BIT_LOAD_STORE(x) ((x >> 13) & 1)

// Combines the most significant and least significant bytes of two registers into one 2-byte quantity.
unsigned short combine_bytes(unsigned short msb, unsigned short lsb)
{
	return((msb << 8) | lsb);
}

// Loads memory from the address specified by the source register into the destination register.
// Can decrement/increment the destination register to allow for indexed addressing. 
void load(unsigned short* dest, unsigned short* source, unsigned short prpo, unsigned short dec, unsigned short inc, unsigned short wb)
{
	switch (prpo)	/* Determine whether to dec/inc the source register pre/post load: */
	{
	case PRE:
		if (wb == WORD)
		{
			*source -= 2 * dec;
			*source += 2 * inc;
			cpu.mar = *source;
			bus(cpu.mar, &cpu.mbr, READ, WORD);
			*dest = cpu.mbr;
		}
		else /* wb == BYTE */
		{
			*source -= dec;
			*source += inc;
			cpu.mar = *source;
			bus(cpu.mar, &cpu.mbr, READ, BYTE);
			*dest = cpu.mbr;
		}
		break;

	case POST:
		if (wb == WORD)
		{
			cpu.mar = *source;
			bus(cpu.mar, &cpu.mbr, READ, WORD);
			*dest = cpu.mbr;
			*source -= 2 * dec;
			*source += 2 * inc;
		}
		else /* wb == BYTE */
		{
			cpu.mar = *source;
			bus(cpu.mar, &cpu.mbr, READ, BYTE);
			*dest = cpu.mbr;
			*source -= dec;
			*source += inc;
		}
		break;
	}
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
				cpu.mar = *dest;
				cpu.mbr = source;
				bus(cpu.mar, &cpu.mbr, WRITE, WORD);
				break;

			case BYTE:
				*dest -= dec;
				*dest += inc;
				cpu.mar = *dest;
				cpu.mbr = source;
				bus(cpu.mar, &cpu.mbr, WRITE, BYTE);
				break;
		}
		break;

	case POST:
		switch (wb)
		{
			case WORD:
				cpu.mar = *dest;
				cpu.mbr = source;
				bus(cpu.mar, &cpu.mbr, WRITE, WORD);
				*dest -= (2 * dec);
				*dest += (2 * inc);
				break;

			case BYTE:
				cpu.mar = *dest;
				cpu.mbr = source;
				bus(cpu.mar, &cpu.mbr, WRITE, BYTE);
				*dest -= dec;
				*dest += inc;
				break;
		}
		break;
	}
}

// Takes a high and low byte and combines them into a 2-byte (16-bit) value which is inserted into the given register.
void move_bytes(unsigned short* reg, unsigned char high, unsigned char low)
{
	*reg = (high << 8) | low;
}

// Loads memory from the address specified by the source register into the destination register, but with a provided offset.
// This offset is sign extended to ensure correct operation.
void load_rel(unsigned short* dest, unsigned short source, short offset, unsigned short wb)
{
	if (OFFSET_SIGN_BIT_LOAD_STORE(offset)) // Sign bit it set, we have to sign extend the most significant 9 bits of the offset.
	{
		offset |= 0xFF80;
	}
	offset = offset << 1;

	switch (wb)
	{
	case WORD:
		cpu.mar = source + offset;
		bus(cpu.mar, &cpu.mbr, READ, WORD);
		*dest = cpu.mbr;
		break;

	case BYTE:
		cpu.mar = source + offset;
		bus(cpu.mar, &cpu.mbr, READ, BYTE);
		*dest = cpu.mbr;
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
		cpu.mar = dest + offset;
		cpu.mbr = source;
		bus(cpu.mar, &cpu.mbr, WRITE, WORD);
		break;

	case BYTE:
		cpu.mar = dest + offset;
		cpu.mbr = source;
		bus(cpu.mar, &cpu.mbr, WRITE, BYTE);
		break;
	}
}
