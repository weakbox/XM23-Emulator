/*
*	Functions and definitions related to loading and moving register values (Lab 3).
*	Written by Connor McLeod, for ECED 3403.
*/

#include "header.h"
#include "instructions.h"

// Prints the contents of the register file.
// Includes both 8 registers and 8 constants, and labels the special registers.
void print_reg(unsigned short regfile[2][8])
{
	printf("Printing register file...\n");
	for (int c = 0; c < 8; c++)
	{
		for (int r = 0; r < 2; r++)
		{
			switch (r)
			{
			case 0:
				printf("R%i: ", c);
				printf("%04x", regfile[r][c]);
				switch (c)
				{
				case 4:
					printf(" (BP)");
					break;

				case 5:
					printf(" (LR)");
					break;

				case 6:
					printf(" (SP)");
					break;

				case 7:
					printf(" (PC)");
					break;

				default:
					break;
				}
				printf("\t");
				break;

			case 1:
				printf("C%i: ", c);
				printf("%04x", regfile[r][c]);
				break;
			}
		}
		printf("\n");
	}
}

// Initializes the fields of the CPU struct. 
void initialize_cpu(CPU *cpu)
{
	cpu->clock = 0;
	cpu->mar = 0;
	cpu->mbr = 0;
	cpu->cr = 0;
	cpu->ir = 0;
}

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
