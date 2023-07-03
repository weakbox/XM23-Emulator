/*
*	Functions and definitions related to miscellaneous tasks and utilities.
*	Written by Connor McLeod, for ECED 3403.
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
