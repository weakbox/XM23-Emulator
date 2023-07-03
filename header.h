/*
*	Header file for the XM-23 Emulator.
*	Written by Connor McLeod, for ECED 3403.
*/

#pragma once

#include <stdio.h>
#include <string.h>

#define NEWLINE '\n'
#define NUL '\0'

#define FALSE 0
#define TRUE 1

#define PRE	 0
#define POST 1

#define MEMORY_SIZE 65536	/* Memory size in bytes. */

#define READ 0
#define WRITE 1
#define WORD 0
#define BYTE 1

#define MAX_RECORD_SIZE 128
#define ID_SIZE 4
#define INST_SIZE 8

#define TYPE_OFFSET 0
#define LENGTH_OFFSET 2
#define ADDRESS_OFFSET 4
#define DATA_OFFSET 8
#define CHECKSUM_OFFSET 2

// Registers and variables relevent to the operation of the CPU (needs updating).
typedef struct _CPU
{
	unsigned int clock;
}	CPU;

// Combines word and byte addressing into one union, where data will be written to a shared space in memory.
typedef union _Memory {
	unsigned char byte[MEMORY_SIZE];		/* Each address of memory contains 1 byte. */
	unsigned short word[MEMORY_SIZE / 2];	/* Each address of memory contains 1 word (2 bytes). */
}	Memory;

// PSW (Retrived from Dr. Larry Hughes).
typedef struct _PSW
{
	unsigned short carry:1;
	unsigned short zero:1;
	unsigned short negative:1;
	unsigned short sleep:1;
	unsigned short overflow:1;
	unsigned short current:3; /* Current priority */
	unsigned short faulting:1; /* 0 - No fault; 1 - Active fault */
	unsigned short reserved:4;
	unsigned short previous:3; /* Previous priority */
}	PSW;

// Emulated system memory is defined as a global that is initialized in main.c
extern Memory mem;
extern PSW psw;
extern CPU cpu;

extern unsigned short regfile[2][8];

extern unsigned short pc;
extern unsigned short lr;

extern unsigned short mar;
extern unsigned short mbr;
extern unsigned short cr;
extern unsigned short ir;

// Functions present in "utility.c"
extern void close();
extern void appendNewline(char* str);
extern void swap_newline(char* str);

// Functions present in "memory.c":
extern void bus(unsigned short mar, unsigned short* mbr, int rw, int wb);
extern unsigned int mem_read(unsigned int address);
extern void mem_write(unsigned int address, unsigned int data);
extern void print_mem(int start, int end, int wb);
extern unsigned int load_srec(FILE* file);

// Functions present in "decode.c":
extern void print_arith(const char* name, unsigned short ir);
extern void print_branch(const char* name, unsigned short ir, unsigned short pc);
extern void print_loadrel(const char* name, unsigned short ir);
extern short sign_extend_offset(unsigned short offset);
extern int decode(unsigned short ir);
extern void execute(unsigned short ir, unsigned short pc);

// Functions present in "registers.c":
extern void print_reg(unsigned short regfile[2][8]);
extern void load(unsigned short* dest, unsigned short* source, unsigned short prpo, unsigned short dec, unsigned short inc, unsigned short wb);
extern void move_bytes(unsigned short* reg, unsigned char high, unsigned char low);
extern void load_rel(unsigned short* dest, unsigned short source, short offset, unsigned short wb);

extern void update_psw(unsigned short dest, unsigned short source, unsigned short result, unsigned short wb);
extern unsigned short combine_bytes(unsigned short msb, unsigned short lsb);
extern void print_psw();



// Instruction functions:

extern void branch_conditional(unsigned short condition, unsigned short expected, unsigned short offset);
extern void branch_link(unsigned short offset);
extern unsigned short add(unsigned short dest, unsigned short source, unsigned short carry, unsigned short wb);
extern void compare(unsigned short dest, unsigned short source, unsigned short wb);
extern unsigned short xor(unsigned short dest, unsigned short source, unsigned short wb);
extern unsigned short and(unsigned short dest, unsigned short source, unsigned short wb);
extern unsigned short or (unsigned short dest, unsigned short source, unsigned short wb);
extern void compare_bit(unsigned short dest, unsigned short source, unsigned short wb);
extern unsigned short set_bit(unsigned short dest, unsigned short source, unsigned short wb);
extern unsigned short clear_bit(unsigned short dest, unsigned short source, unsigned short wb);
extern void store(unsigned short* dest, unsigned short source, unsigned short prpo, unsigned short dec, unsigned short inc, unsigned short wb);
extern void store_rel(unsigned short dest, unsigned short source, short offset, unsigned short wb);
extern unsigned short move(unsigned short dest, unsigned short source, unsigned short wb);
extern void swap_reg(unsigned short* dest, unsigned short* source);
extern unsigned short complement(unsigned short dest, unsigned short wb);
extern unsigned short swap_byte(unsigned short dest);
extern unsigned short sign_extend(unsigned short dest);
extern void store(unsigned short* dest, unsigned short source, unsigned short prpo, unsigned short dec, unsigned short inc, unsigned short wb);
extern void store_rel(unsigned short dest, unsigned short source, short offset, unsigned short wb);
