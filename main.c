/*
*	XM-23 Emulation Software.
*	Written by Connor McLeod, for ECED 3403.
*/

#include "header.h"

FILE* file;
Memory mem;
PSW psw;

// CPU Registers:
unsigned short mar =	0;
unsigned short mbr =	0;
unsigned short cr =		0;
unsigned short ir =		0;

unsigned short pc = 0;

unsigned short regfile[2][8] = 
{ 
	{ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },	/* Registers. */
	{ 0x0000, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0xFFFF }	/* Constants. */
};

// Reads the contents of a memory location as a 16-bit word and places it into the instruction register.
void fetch()
{
	mar = pc;
	bus(mar, &mbr, READ, WORD);
	ir = mbr;
	pc += 2;
}

int main(int argv, char* argc[])
{
	// File opening handling:
	if (argv < 2)
	{
		printf("No file was inputted!\n");
		close();
		return -1;
	}

	if (fopen_s(&file, argc[1], "r") != 0)
	{
		printf("There was a problem opening the file...\n");
		close();
		return -1;
	}

	printf("File %s was opened successfully!\n", argc[1]);

	// Load contents of S-Record file into virtual memory:
	int start_addr = load_srec(file);

	// All instructions have been loaded into virtual memory, the emulator can now begin execution.
	printf("Start Address: 0x%x\n", start_addr);

	pc = start_addr;
	int input;
	int input_mod1 = 0;
	int input_mod2 = 0;
	int running = TRUE;

	while (running)
	{
		printf("What would you like to do next? (Current PC = 0x%04x)\n", pc);
		printf("1: Continue execution\n");
		printf("2: Change the program counter\n");
		printf("3: Print memory\n");
		printf("4: Print registers\n");
		printf("5: Print PSW\n");
		printf("0: Exit\n");
		(void)scanf("%i", &input);
		(void)getchar();

		// Handles all possibilities of user input.
		switch (input)
		{
			case 1:		/* Proceed: */
				fetch();
				execute(ir, pc);
				break;

			case 2:		/* Change the program counter: */
				printf("Enter a new value for the program counter (4-digit hex):\n");
				(void)scanf("%4x", &input_mod1);
				(void)getchar();
				pc = input_mod1;
				break;

			case 3:		/* Print memory: */
				printf("Enter the range of addresses you would like to print from memory <start> <end>\n");
				(void)scanf("%4x %4x", &input_mod1, &input_mod2);
				(void)getchar();
				print_mem(input_mod1, input_mod2, BYTE);
				break;

			case 4:		/* Print registers: */
				print_reg(regfile);
				break;

			case 5:	// Print contents of PSW.
				print_psw();
				break;

			default:	/* Close the emulator: */
				running = FALSE;
				break;
		}
	}
	// Debugging space!
	if (0)
	{
		printf("Debug: Test code!\n");
		close();
	}

	fclose(file);
	return 0;
}
