/*
  File: two_operand_instructions.c
  Author: Connor McLeod
  Creation Date: July 2, 2023
  Description: Provides functions and declarations that enable the use of the two operand instructions found in chapter 6.1 of the XM-23 ISA.
*/

#include "header.h"

#define LSBYTE(x)     ((x) & 0x0003)
#define MSBYTE(x)     (((x) >> 8) & 0x0003)
#define MSBIT_WORD(x) (((x) >> 15) & 0x0001)
#define MSBIT_BYTE(x) (((x) >> 7) & 0x0001)

// Updates the Program Status Word (PSW) based on the result of execution for a particular instruction.
// Retrieved from Dr. Larry Hughes (2023).
void update_psw(unsigned short dest, unsigned short source, unsigned short result, unsigned short wb)
{
	/*
	 - Update the PSW bits (V, N, Z, C)
	 - Using source, dest, and result values and whether word or byte
	 - ADD, ADDC, SUB, and SUBC
	*/
	unsigned short carry[2][2][2] = { 0, 0, 1, 0, 1, 0, 1, 1 };
	unsigned short overflow[2][2][2] = { 0, 1, 0, 0, 0, 0, 1, 0 };

	unsigned short mss, msd, msr; /* Most significant source, dest, and result bits */

	if (wb == 0)
	{
		mss = MSBIT_WORD(source);
		msd = MSBIT_WORD(dest);
		msr = MSBIT_WORD(result);
	}
	else /* Byte */
	{
		mss = MSBIT_BYTE(source);
		msd = MSBIT_BYTE(dest);
		msr = MSBIT_BYTE(result);
		result &= 0x00FF;	/* Mask high byte for 'z' check */
	}

	/* Carry */
	psw.carry = carry[mss][msd][msr];
	/* Zero */
	psw.zero = (result == 0);
	/* Negative */
	psw.negative = (msr == 1);
	/* oVerflow */
	psw.overflow = overflow[mss][msd][msr];
}

// Prints the current contents of the PSW.
void print_psw()
{
	printf("Printing the status of the PSW:\n");
	printf("Carry:    %i\n", psw.carry);
	printf("Zero:	  %i\n", psw.zero);
	printf("Negative: %i\n", psw.negative);
	printf("Overflow: %i\n", psw.overflow);
}

// Adds the destination, source, and carry bit together.
// Returns the result of this addition, and sets the PSW bits depending on the result.
unsigned short add(unsigned short dest, unsigned short source, unsigned short carry, unsigned short wb)
{
	unsigned short result = 0;
	switch (wb)
	{
	case WORD:
		result = dest + source + carry;
		break;

	case BYTE:
		// Byte operations affect the least-significant byte only. The most-significant byte is untouched.
		result = combine_bytes(MSBYTE(dest), LSBYTE(dest) + LSBYTE(source) + carry);
		break;
	}
	update_psw(dest, source, result, wb);
	return result;
}

// Compares the values of the destination and source registers to determine if they are equal.
// Sets the PSW bits depending on the result.
void compare(unsigned short dest, unsigned short source, unsigned short wb)
{
	unsigned short result = 0;
	switch (wb)
	{
	case WORD:
		result = dest + ((~source) + 1);	// Uses two's complement subtraction.
		break;

	case BYTE:
		result = LSBYTE(dest) + ((~LSBYTE(source)) + 1);
		break;
	}
	update_psw(dest, source, result, wb);
}

// Performs a XOR operation between the values of the desination and source registers.
// Returns the result of this operation, and sets the PSW bits depending on the result.
unsigned short xor(unsigned short dest, unsigned short source, unsigned short wb)
{
	unsigned short result = 0;
	switch (wb)
	{
	case WORD:
		result = dest ^ source;
		break;

	case BYTE:
		result = combine_bytes(MSBYTE(dest), (LSBYTE(dest) ^ LSBYTE(source)));
		break;
	}
	update_psw(dest, source, result, wb);
	return result;
}

// Performs an AND operation between the the values of the desination and source registers.
// Returns the result of this operation, and sets the PSW bits depending on the result.
unsigned short and(unsigned short dest, unsigned short source, unsigned short wb)
{
	unsigned short result = 0;
	switch (wb)
	{
	case WORD:
		result = dest & source;
		break;

	case BYTE:
		result = combine_bytes(MSBYTE(dest), (LSBYTE(dest) & LSBYTE(source)));
		break;
	}
	update_psw(dest, source, result, wb);
	return result;
}

// Performs an OR operation between the the values of the desination and source registers.
// Returns the result of this operation, and sets the PSW bits depending on the result.
unsigned short or(unsigned short dest, unsigned short source, unsigned short wb)
{
	unsigned short result = 0;
	switch (wb)
	{
	case WORD:
		result = dest | source;
		break;

	case BYTE:
		result = combine_bytes(MSBYTE(dest), (LSBYTE(dest) | LSBYTE(source)));
		break;
	}
	update_psw(dest, source, result, wb);
	return result;
}

// Determines whether a specific bit in the destination register is set high or low.
// Value of the source register must indicate the bit to be compared.
// Sets the PSW bits depending on the result.
void compare_bit(unsigned short dest, unsigned short source, unsigned short wb)
{
	if (dest & source)
	{
		psw.zero = 0;	// Clear PSW zero bit to indicate there was a bit-match.
		switch (wb)
		{
		case WORD:
			psw.negative = (MSBIT_WORD(dest & source)) ? 1 : 0;
			break;

		case BYTE:
			psw.negative = (MSBIT_BYTE(dest & source)) ? 1 : 0;
			break;
		}
	}
	else
	{
		psw.zero = 1;
	}
}

// Sets a bit in the destination register, determined by the position indicated in the source register.
// Returns the result of this operation, and clears the PSW's zero bit.
unsigned short set_bit(unsigned short dest, unsigned short source, unsigned short wb)
{
	unsigned short result = 0;
	switch (wb)
	{
	case WORD:
		result = dest | (1 << source);
		break;

	case BYTE:
		result = dest | (1 << source);
		break;
	}
	psw.zero = 0; // Clear the PSW's zero bit.
	return result;
}

// Clears a bit in the destination register, determined by the position indicated in the source register.
// Returns the result of this operation, and sets the PSW's zero bit if the result of the operation was zero.
unsigned short clear_bit(unsigned short dest, unsigned short source, unsigned short wb)
{
	unsigned short result = 0;
	switch (wb)
	{
	case WORD:
		result = dest & ~(1 << source);
		break;

	case BYTE:
		result = dest & ~(1 << source);
		break;
	}
	psw.zero = (result == 0) ? 1 : 0;
	return result;
}
