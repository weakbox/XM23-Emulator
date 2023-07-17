/*
  File: instructions.c
  Author: Connor McLeod
  Date: July 2, 2023
  Description: Provides functions and declarations that enable the use of the instructions found in chapter 6.1-6.4 of the XM-23 ISA.
*/

#include "header.h"
#include "instructions.h"

#define OFFSET_SIGN_BIT_LINK(x) ((x >> 12) & 1)
#define OFFSET_SIGN_BIT_COND(x) ((x >> 9 ) & 1)
#define OFFSET_SIGN_BIT_LOAD_STORE(x) ((x >> 13) & 1)

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
		result = combine_bytes(MSBYTE(dest), LSBYTE(dest + source + carry));
		break;
	}
	update_psw(dest, source, result, wb);
	return result;
}

// BCD arithmetic function used in the DADD instruction.
unsigned short bcd_arith(unsigned short nib_d, unsigned short nib_s, unsigned short *hc)
{
	unsigned short result = 0;

	result = nib_s + nib_d + *hc;

	if (result > 9)
	{
		*hc = 1;
		result -= 10;
	}
	else
	{
		*hc = 0;
	}
	return result;
}

// Performs BCD arithmetic between the destination and the source.
// Returns the result of this addition, and sets the PSW bits depending on the result.
unsigned short add_decimal(unsigned short dest, unsigned short source, unsigned short wb)
{
	WordNib d;
	WordNib s;
	WordNib r;

	unsigned short half_carry = 0;

	d.word = dest;
	s.word = source;
	r.word = 0000;

	#ifdef VERBOSE
	printf("DST: %i %i %i %i\n", d.nibbles.n3, d.nibbles.n2, d.nibbles.n1, d.nibbles.n0);
	printf("SRC: %i %i %i %i\n", s.nibbles.n3, s.nibbles.n2, s.nibbles.n1, s.nibbles.n0);
	#endif

	switch (wb)
	{
	case WORD:
		r.nibbles.n0 = bcd_arith(d.nibbles.n0, s.nibbles.n0, &half_carry);
		r.nibbles.n1 = bcd_arith(d.nibbles.n1, s.nibbles.n1, &half_carry);
		r.nibbles.n2 = bcd_arith(d.nibbles.n2, s.nibbles.n2, &half_carry);
		r.nibbles.n3 = bcd_arith(d.nibbles.n3, s.nibbles.n3, &half_carry);
		break;

	case BYTE:
		r.nibbles.n0 = bcd_arith(d.nibbles.n0, s.nibbles.n0, &half_carry);
		r.nibbles.n1 = bcd_arith(d.nibbles.n1, s.nibbles.n1, &half_carry);
		break;
	}

	update_psw(dest, source, r.word, wb);
	return r.word;
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
unsigned short xor (unsigned short dest, unsigned short source, unsigned short wb)
{
	unsigned short result = 0;
	switch (wb)
	{
	case WORD:
		result = dest ^ source;
		break;

	case BYTE:
		result = combine_bytes(MSBYTE(dest), LSBYTE(dest ^ source));
		break;
	}
	update_psw(dest, source, result, wb);
	return result;
}

// Performs an AND operation between the the values of the desination and source registers.
// Returns the result of this operation, and sets the PSW bits depending on the result.
unsigned short and (unsigned short dest, unsigned short source, unsigned short wb)
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
unsigned short or (unsigned short dest, unsigned short source, unsigned short wb)
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
	else { psw.zero = 1; }
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

// Shifts the contents of the destination register right by one bit.
// Does not modify the value of the sign bit.
unsigned short shift(unsigned short dest, unsigned short wb)
{
	unsigned short result = 0;

	switch (wb)
	{
	case WORD:
		result = dest >> 1;
		if (MSBIT_WORD(dest))
		{
			result |= 0x8000;	// Destination was initially negative, need to preserve MSBit.
		}
		break;

	case BYTE:
		result = combine_bytes(MSBYTE(dest), (LSBYTE(dest) >> 1));
		if (MSBIT_BYTE(dest))
		{
			result |= 0x0080;	// Destination was initially negative, need to preserve MSBit.
		}
		break;
	}
	return result;
}

// Shifts the contents of the destination register right by one bit.
// Stores the current carry bit into the MSBit of the destination.
// Stores the LSB of the destination in the carry bit.
unsigned short rotate(unsigned short dest, unsigned short wb)
{
	unsigned short result = 0;

	switch (wb)
	{
	case WORD:

		result = dest >> 1;				// Shift register right by 1 bit.
		result |= psw.carry << 15;		// Move PSW's carry bit into the MSB of the register.
		psw.carry = LSBIT_WORD(dest);	// Store LSBit of register in PSW's carry bit.
		break;

	case BYTE:
		result = combine_bytes(MSBYTE(dest), (LSBYTE(dest) >> 1));
		result |= psw.carry << 7;
		psw.carry = LSBIT_BYTE(dest);
		break;
	}
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

// Sets or clears bits based on the provided PSW chunk.
void psw_mod(unsigned short psw_bits, bool clear)
{
	// Keep an original 'key' of the bits to modify, in case we need to adjust the bit values.
	unsigned short key = psw_bits;

	// Complement psw_bits if the instruction is a clear.
	if (clear)
	{
		psw_bits = ~psw_bits;
	}

	// Modify the PSW bits based on the input.
	if (CARRY(key))
	{
		psw.carry = CARRY(psw_bits);
	}
	if (ZERO(key))
	{
		psw.zero = ZERO(psw_bits);
	}
	if (NEGATIVE(key))
	{
		psw.negative = NEGATIVE(psw_bits);
	}
	if (SLEEP(key))
	{
		psw.sleep = SLEEP(psw_bits);
	}
	if (OVERFLOW(key))
	{
		psw.overflow = OVERFLOW(psw_bits);
	}
}

// Sets ignored instruction flags based on the result of the evaluated condition.
// Does not modify the PSW.
void exec_conditional(unsigned short f_count, unsigned short t_count, unsigned short code)
{
	unsigned short condition = -1; // -1 is uninitialized.
	unsigned short expected = -1;

	// Read the inputted code to determine the condition parameters. 
	switch (code)
	{
	case 0: // EQ (equals zero):
		condition = psw.zero;
		expected = 1;
		break;
	
	case 1: // NE (does not equal zero):
		condition = psw.zero;
		expected = 0;
		break;

	case 2: // CS/HS (carry set/unsigned higher or same):
		condition = psw.carry;
		expected = 1;
		break;

	case 3: // CC/LO (carry clear/unsigned lower):
		condition = psw.carry;
		expected = 0;
		break;

	case 4: // MI (minus/negative):
		condition = psw.negative;
		expected = 1;
		break;

	case 5: // PL (plus/positive or zero):
		condition = psw.negative;
		expected = 0;
		break;

	case 6: // VS (overflow):
		condition = psw.overflow;
		expected = 1;
		break;

	case 7: // VC (no overflow):
		condition = psw.overflow;
		expected = 0;
		break;

	case 8: // HI (unsigned higher):
		condition = (psw.carry == 1) && (psw.zero == 0);
		expected = 1;
		break;

	case 9: // LS (unsigned lower or same):
		condition = (psw.carry == 0) || (psw.zero == 1);
		expected = 1;
		break;

	case 10: // GE (signed greater than or equal):
		condition = (psw.negative == psw.overflow);
		expected = 1;
		break;

	case 11: // LT (signed less than):
		condition = (psw.negative != psw.overflow);
		expected = 1;
		break;

	case 12: // GT (signed greater than):
		condition = ((psw.zero == 0) && (psw.negative == psw.overflow));
		expected = 1;
		break;

	case 13: // LE (signed less than or equal):
		condition = ((psw.zero ==10) && (psw.negative != psw.overflow));
		expected = 1;
		break;

	case 14: // TR (true part always executed):
		condition = 1;
		expected = 1;
		break;

	case 15: // FL (false part always executed):
		condition = 0;
		expected = 1;
		break;
	}

	// The condition and expected parameters are now found, determine the result:
	if (condition == expected)
	{
		cex.true_count = t_count;
		cex.false_count = f_count;
	}
	else // (condition != expected)
	{
		cex.false_count = f_count;
	}
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
