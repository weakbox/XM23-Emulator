/*
  File: exceptions.c
  Author: Connor McLeod
  Date: July 30, 2023
  Description: Functions related to interrupt and exception handling.
*/

#include "header.h"

// Isolates specific bits in the PSW.
#define CARRY(x)	((x) & 0x01)
#define ZERO(x)		((x >> 01) & 0x01)
#define NEGATIVE(x)	((x >> 02) & 0x01)
#define SLEEP(x)	((x >> 03) & 0x01)
#define OVERFLOW(x) ((x >> 04) & 0x01)
#define CURRENT(x)  ((x >> 05) & 0x07)
#define PREVIOUS(x) ((x >> 13) & 0x07)

#define I_VEC_SIZE 16
#define STACK_SIZE 8
#define I_VEC_BASE_ADDR 0x7FE0

// Struct to store interrupt vectors loaded from main memory on system startup.
typedef struct _I_Vector
{
	unsigned short psw;
	unsigned short address;
}	I_Vector;

// Lazy implementation of a stack (Monday Session 10).
// Avoids using a linked list since we know the stack can only be of fixed-length.
typedef struct _Stack
{
	unsigned short pc;
	unsigned short lr;
	struct _PSW psw;
	struct _CEX cex;
}	Stack;

I_Vector i_vec_table[I_VEC_SIZE];

// We know the maximum size of the stack will be 8 as that is the maximum
// number of priority levels.
Stack stack[STACK_SIZE];

unsigned int stack_idx = 0;

// Pushes onto the lazy stack.
void stack_push(unsigned short pc, unsigned short lr, PSW psw, CEX cex)
{
	stack[stack_idx].pc = pc;
	stack[stack_idx].lr = lr;
	stack[stack_idx].psw = psw;
	stack[stack_idx].cex = cex;
	stack_idx++;
}

// Pulls from the lazy stack.
void stack_pull(unsigned short* pc, unsigned short* lr, PSW* psw, CEX* cex)
{
	stack_idx--;
	*pc = stack[stack_idx].pc;
	*lr = stack[stack_idx].lr;
	*psw = stack[stack_idx].psw;
	*cex = stack[stack_idx].cex;
}

// Initialize the interrupt vector table.
// System assumes this table will not be changed during execution.
void i_vector_init()
{
	for (int i = 0; i < I_VEC_SIZE; i++)
	{
		i_vec_table[i].psw =     mem.word[(I_VEC_BASE_ADDR + (2 * i))];
		i_vec_table[i].address = mem.word[(I_VEC_BASE_ADDR + (2 * i)) + 1];
	}
}

// Prints contents of interrupt vector table.
void i_vector_print()
{
	printf("Printing interrupt vector table...\n");
	for (int i = 0; i < I_VEC_SIZE; i++)
	{
		printf("I-Vec %02i: %04x %04x\n", i, i_vec_table[i].psw, i_vec_table[i].address);
	}
}

// Decodes a unsigned short representation of a PSW into a PSW struct.
// Useful for converting the PSW in the interrupt vector into a usable structure.
PSW psw_decode(unsigned short psw_unsigned)
{
	PSW converted_psw;

	converted_psw.carry = CARRY(psw_unsigned);
	converted_psw.zero = ZERO(psw_unsigned);
	converted_psw.negative = NEGATIVE(psw_unsigned);
	converted_psw.sleep = SLEEP(psw_unsigned);
	converted_psw.overflow = OVERFLOW(psw_unsigned);
	converted_psw.current = CURRENT(psw_unsigned);
	converted_psw.previous = PREVIOUS(psw_unsigned);

	return converted_psw;
}

// Sets CPU priority in PSW.
// Throws a priority fault if trying to set to a higher priority.
void set_priority(int new_pri)
{
	if (new_pri > psw.current)
	{
		// Throw priority fault.
	}
	psw.previous = psw.current;
	psw.current = new_pri;
}

// Pushes current CPU state onto stack, then sets CPU state stored in the interrupt vector.
// Modifies PC to the address of the interrupt handler.
// Throws a priority fault if the SVC was called by a process with a higher priority than the interrupt handler.
void supervisory_call(int vector_index)
{
	// Push current CPU state:
	stack_push(PC, LR, psw, cex);

	// Get new CPU state from interrupt vector:
	psw = psw_decode(i_vec_table[vector_index].psw);
	PC = i_vec_table[vector_index].address;
	LR = 0xFFFF; /* Link register is set to -1 to indicate to MOV instruction that we must pull the new link register off the stack. */
}
