/*
  File: transfer_of_control_instructions.c
  Author: Connor McLeod
  Date: July 2, 2023
  Description: Provides functions and declarations that enable the use of the transfer of control instructions found in chapter 6.4 of the XM-23 ISA.
*/

#include "header.h"

// Sets the program counter equal to the current program counter plus a provided offset. Stores return address in the link register. 
void branch(unsigned short offset)
{
	
}

// Sets the program counter equal to the current program counter plus a provided offset if the condition is equal to the expected value.
void branch_cond(unsigned short condition, unsigned short expected, unsigned short offset)
{
	if (condition == expected)
	{
		
	}
}
