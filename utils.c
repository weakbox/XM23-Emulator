/*
  File: utils.c
  Author: Connor McLeod
  Date: July 5, 2023
  Description: Functions and definitions related to miscellaneous tasks and utilities.
*/

#include "header.h"

// Displays a message and keeps the console open on the screen.
void close()
{
	printf("Press enter to exit...\n");
	(void)getchar();
}

// Checks if the buffer ends with a newline character, in the case where it does not, adds a newline character to the output to preserve formatting. 
void appendNewline(char* str)
{
	char lastChar = str[strlen(str) - 1];
	if (lastChar != NEWLINE)
	{
		printf("\n");
	}
}

// Replaces a hanging newline character with the null character for printing.
void swap_newline(char* str)
{
	size_t len = strlen(str);
	if (len > 0 && str[len - 1] == NEWLINE)	/* Need to subtract 1 due do the discrepency between array index and size. */
	{
		str[len - 1] = NUL;
	}
}

// Prints the emulator's controls.
void print_controls()
{
	printf("Emulator Controls:\n");
	printf("1: Run program\n");
	printf("2: Step through program\n");
	printf("3: Set breakpoint\n");
	printf("4: Modify program counter\n");
	printf("5: Print memory contents\n");
	printf("6: Print register values\n");
	printf("7: Print PSW\n");
	printf("8: Print cache\n");
	printf("9: Modify cache organization/policy\n");
	printf("0: Exit emulator\n");
}

// Opens a file provided by the user in the form of a command-line argument.
// -> Fails if no file is given, file is of the wrong type, or if the file cannot be accessed.
int open_xme_file(FILE** file, int args_num, const char* file_name)
{
	if (args_num < 2)
	{
		printf("Error: No file was specified.\n");
		return -1;
	}

	if (strstr(file_name, ".xme") == 0)
	{
		printf("Error: Invalid file format. Please use a .xme file.\n");
		return -1;
	}

	if (fopen_s(file, file_name, "r") != 0)
	{
		printf("Error: Failed to open .xme file.\n");
		return -1;
	}

	return 0;
}

// Removes unwanted characters from buffer.
void flush_buffer()
{
	while ((getchar()) != '\n');
}

// Handler for crtl-c detection.
void sigint_hdlr()
{
	// Invoked when SIGINT (control-C) is detected.
	// -> changes state of waiting_for_signal.
	// -> signal must be reinitialized.

	ctrl_c_fnd = true;
	signal(SIGINT, (_crt_signal_t)sigint_hdlr); /* Reinitialize SIGINT */
}

// Combines the most significant and least significant bytes of two registers into one 2-byte quantity.
unsigned short combine_bytes(unsigned short msb, unsigned short lsb)
{
	return((msb << 8) | lsb);
}
