/*
  File: main.c
  Author: Connor McLeod
  Date: May 21, 2023
  Description: Emulator for the XM-23 RISC processor.
*/

#include "header.h"

CPU cpu;
PSW psw;
Memory mem;
CEX cex;

unsigned short regfile[2][8] = 
{ 
	{ 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 },	// Registers.
	{ 0x0000, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0xFFFF }	// Constants.
};

volatile sig_atomic_t ctrl_c_fnd;

// Checks if an instruction is blocked by a CEX instruction.
// Returns 1 if unblocked, 0 if blocked.
bool cex_not_blocked()
{
	// Execute only occurs if it is not blocked by a CEX instruction.
	if (cex.true_count <= 0 && cex.false_count > 0)
	{
		#ifdef VERBOSE
		printf("CEX Block!\n");
		#endif
		cex.false_count--;
		return false;
	}
	else
	{
		if (cex.true_count > 0)
		{
			cex.true_count--;
		}
		return true;
	}
}

// Please note that the code is heavily commented for ease of marking.
int main(int argv, char* argc[])
{
	// Open .xme program file.
	// Fails if no file, wrong type, or inaccessible.
	FILE* file;
	if (open_xme_file(&file, argv, argc[1]) != 0)
	{
		close();
		return -1;	// Program file failed to open.
	}
	printf("File opened successfully: %s\n", argc[1]);

	// Load contents of .xme program file into virtual memory.
	// This function also returns the start address of the PC.
	PC = load_srec(file);

	// Instructions have been loaded into virtual memory.
	// Initialize the CPU's internal registers.
	initialize_cpu(&cpu);
	cache_init(CACHE_SIZE);

	// Initialize local variables for user input.
	bool running   = true;
	int input	   = 0;
	int input_mod1 = 0;
	int input_mod2 = 0;
	int breakpoint = 0;
	ctrl_c_fnd	   = false;

	signal(SIGINT, (_crt_signal_t)sigint_hdlr);

	// Prints the emulator's controls.
	print_controls();

	// Setup is complete, the execution can begin.
	while (running)
	{
		// Prompts the user for input and reads the value.
		printf("Clock: %i\n", cpu.clock);
		printf("Input: ");
		(void)scanf("%i", &input);
		flush_buffer();

		ctrl_c_fnd = false;	// Resets ctrl-c handler.

		switch (input)
		{
			case 1: // Run program.
				printf("Program execution in progress. Press ctrl+c to halt.\n");
				while (PC != breakpoint && !ctrl_c_fnd)
				{
					fetch();
					if (cex_not_blocked())
					{
						execute(cpu.ir, PC);
					}
				}

				break;

			case 2: // Step through program.
				fetch();
				if (cex_not_blocked())
				{
					execute(cpu.ir, PC);
				}
				break;

			case 3: // Set breakpoint.
				printf("Enter breakpoint:\n");
				printf("Input: ");
				(void)scanf("%4x", &input_mod1);
				flush_buffer();
				breakpoint = input_mod1;
				break;

			case 4: // Modify program counter.
				printf("Enter new program counter:\n");
				printf("Input: ");
				(void)scanf("%4x", &input_mod1);
				flush_buffer();
				PC = input_mod1;
				break;

			case 5: // Print memory contents.
				printf("Enter addresses to print: <start> <end>\n");
				printf("Input: ");
				(void)scanf("%4x %4x", &input_mod1, &input_mod2);
				flush_buffer();
				print_mem(input_mod1, input_mod2);
				break;

			case 6: // Print register values.
				print_reg(regfile);
				break;

			case 7:	// Print PSW.
				print_psw();
				break;

			case 8: // Print cache.
				cache_print(CACHE_SIZE);
				break;

			case 9:
				printf("Enter new cache settings: <org> <pol>\n");
				printf("Input: ");
				(void)scanf("%i %i", &input_mod1, &input_mod2);
				flush_buffer();
				cache_config(input_mod1, input_mod2);
				break;

			default: // Exit emulator.
				running = false;
				break;
		}
	}
	fclose(file);
	return 0;
}
