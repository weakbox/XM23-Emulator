/*
*	Functions and definitions related to loading and moving register values (Lab 3).
*	Written by Connor McLeod, for ECED 3403.
*/

#include "header.h"

// Shifts address to the right by 1 bit, effectively dividing it by 2. Converts a byte addressed address into a word addressed address.
#define word(x) (x>>1)

// Prints the contents of the register file. Includes both 8 registers and 8 constants.
void print_reg(unsigned short regfile[2][8])
{
	printf("Printing the current contents of the register file...\n");
	for (int r = 0; r < 2; r++)
	{
		switch (r) 
		{
		case 0:
			printf("Registers: ");
			break;

		case 1:
			printf("Constants: ");
			break;
		}	
		for (int c = 0; c < 8; c++)
		{
			printf("%04x ", regfile[r][c]);
		}
		printf("\n");
	}
}

// Loads from memory at the address stored in the source register, and stores into the destination register.
void load(unsigned short* dest, unsigned short *source, unsigned short prpo, unsigned short dec, unsigned short inc, unsigned short wb)
{
	switch (prpo)	/* Determine whether to dec/inc the source register pre/post load: */
	{
	case PRE:
		if (wb == WORD)
		{
			*source -= 2*dec;
			*source += 2*inc;
			mar = *source;
			bus(mar, &mbr, READ, WORD);
			*dest = mbr;
		}
		else /* wb == BYTE */
		{
			*source -= dec;
			*source += inc;
			mar = *source;
			bus(mar, &mbr, READ, BYTE);
			*dest = mbr;
		}
		break;

	case POST:
		if (wb == WORD)
		{
			mar = *source;
			bus(mar, &mbr, READ, WORD);
			*dest = mbr;
			*source -= 2*dec;
			*source += 2*inc;
		}
		else /* wb == BYTE */
		{
			mar = *source;
			bus(mar, &mbr, READ, BYTE);
			*dest = mbr;
			*source -= dec;
			*source += inc;
		}
		break;
	}
}

// Takes a high and low byte and combines them into a 2-byte (16-bit) value which is inserted into the given register.
void move_bytes(unsigned short *reg, unsigned char high, unsigned char low)
{
	*reg = (high << 8) | low;
}

// Loads memory from the address specified by the source register, but with an added offset.
void load_rel(unsigned short *dest, unsigned short source, short offset, unsigned short wb)
{
	switch (wb)
	{
	case WORD:
		mar = source + offset;
		bus(mar, &mbr, READ, WORD);
		*dest = mbr;
		break;

	case BYTE:
		mar = source + offset;
		bus(mar, &mbr, READ, BYTE);
		*dest = mbr;
		break;
	}
}
