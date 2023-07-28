/*
  File: main.c
  Author: Connor McLeod
  Date: May 21, 2023
  Description: Main header file holding all general stucts, externs, and function prototypes.
*/

#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>

// #define VERBOSE	// Can be enabled to show visual indications of events within the emulator.

#define NEWLINE '\n'
#define NUL '\0'

#define READ 0
#define WRITE 1

#define WORD 0
#define BYTE 1

#define MEMORY_SIZE 65536	// Memory size in bytes.

#define CACHE_SIZE 32

#define BP regfile[0][4]	// R4 -> Base Pointer
#define LR regfile[0][5]	// R5 -> Link Register
#define SP regfile[0][6]	// R6 -> Stack Pointer
#define PC regfile[0][7]	// R7 -> Program Counter

// Stores data relevent to the operation of the CPU.
// -> Current CPU clock.
// -> Internal CPU registers.
typedef struct _CPU
{
	unsigned int   clock;
	unsigned short mar;
	unsigned short mbr;
	unsigned short cr;
	unsigned short ir;
}	CPU;

// Stores data relevent to the operation of the PSW (retrieved from Dr. Larry Hughes).
typedef struct _PSW
{
	unsigned short carry : 1;
	unsigned short zero : 1;
	unsigned short negative : 1;
	unsigned short sleep : 1;
	unsigned short overflow : 1;
	unsigned short current : 3; /* Current priority */
	unsigned short faulting : 1; /* 0 - No fault; 1 - Active fault */
	unsigned short reserved : 4;
	unsigned short previous : 3; /* Previous priority */
}	PSW;

// Combines word and byte addressing into a single union, allowing data to be written to a shared space in memory.
typedef union _Memory {
	unsigned char  byte[MEMORY_SIZE];		// Each memory address contains 1 byte.
	unsigned short word[MEMORY_SIZE / 2];	// Each memory address contains 1 word (2 bytes).
}	Memory;

// Breaks a word into its four independent nibbles.
typedef struct _Nibbles 
{
	unsigned short n0 : 4;
	unsigned short n1 : 4;
	unsigned short n2 : 4;
	unsigned short n3 : 4;
}	Nibbles;

// Combines nibbles and word into a single union, allowing data to be written to a shared space in memory.
typedef union _WordNib
{
	struct _Nibbles nibbles;
	unsigned short word;
}	WordNib;

// Stores conditional execution counts together in a struct.
typedef struct _CEX
{
	unsigned short false_count:3;
	unsigned short true_count:3;
}	CEX;

extern CPU cpu;								// Global emulator CPU.
extern PSW psw;								// Global emulator PSW.
extern Memory mem;							// Global emulator memory.
extern CEX cex;
extern unsigned short regfile[2][8];		// Global emulator register file.
extern volatile sig_atomic_t ctrl_c_fnd;	// Ctrl+C signal indicator.

// Functions defined in "CPU.c":
extern void				initialize_cpu(CPU* cpu);
extern void				update_psw(unsigned short dest, unsigned short source, unsigned short result, unsigned short wb);
extern void				print_psw();
extern void				print_reg(unsigned short regfile[2][8]);
extern void				fetch();
extern int				decode(unsigned short ir);
extern void				execute(unsigned short ir, unsigned short pc);

// Functions defined in "memory.c":
extern void				bus(unsigned short mar, unsigned short* mbr, int rw, int wb);
extern unsigned int		mem_read(unsigned int address);
extern void				mem_write(unsigned int address, unsigned int data);
extern void				print_mem(int start, int end);
extern unsigned int		load_srec(FILE* file);

// Functions defined in "instructions.c":
extern void				branch_conditional(unsigned short condition, unsigned short expected, unsigned short offset);
extern void				branch_link(unsigned short offset);
extern unsigned short	add(unsigned short dest, unsigned short source, unsigned short carry, unsigned short wb);
extern unsigned short	add_decimal(unsigned short dest, unsigned short source, unsigned short wb);
extern void				compare(unsigned short dest, unsigned short source, unsigned short wb);
extern unsigned short	xor(unsigned short dest, unsigned short source, unsigned short wb);
extern unsigned short	and(unsigned short dest, unsigned short source, unsigned short wb);
extern unsigned short	or(unsigned short dest, unsigned short source, unsigned short wb);
extern void				compare_bit(unsigned short dest, unsigned short source, unsigned short wb);
extern unsigned short	clear_bit(unsigned short dest, unsigned short source, unsigned short wb);
extern unsigned short	set_bit(unsigned short dest, unsigned short source, unsigned short wb);
extern void				move_bytes(unsigned short* reg, unsigned char high, unsigned char low);
extern void				swap_reg(unsigned short* dest, unsigned short* source);
unsigned short			shift(unsigned short dest, unsigned short wb);
extern unsigned short	rotate(unsigned short dest, unsigned short wb);
extern unsigned short	complement(unsigned short dest, unsigned short wb);
extern unsigned short	swap_byte(unsigned short dest);
extern unsigned short	sign_extend(unsigned short dest);
extern void				psw_mod(unsigned short psw_bits, bool clear);
extern void				exec_conditional(unsigned short f_count, unsigned short t_count, unsigned short code);
extern bool				cex_blocking();
extern void				load(unsigned short* dest, unsigned short* source, unsigned short prpo, unsigned short dec, unsigned short inc, unsigned short wb);
extern void				store(unsigned short* dest, unsigned short source, unsigned short prpo, unsigned short dec, unsigned short inc, unsigned short wb);
extern unsigned short	move(unsigned short dest, unsigned short source, unsigned short wb);
extern void				load_rel(unsigned short* dest, unsigned short source, short offset, unsigned short wb);
extern void				store_rel(unsigned short dest, unsigned short source, short offset, unsigned short wb);

// Functions defined in "utils.c":
extern void				close();
extern void				appendNewline(char* str);
extern void				swap_newline(char* str);
extern int				open_xme_file(FILE** file, int args_num, const char* file_name);
extern void				flush_buffer();
extern void				sigint_hdlr();
extern void				print_controls();
extern unsigned short	combine_bytes(unsigned short msb, unsigned short lsb);

// Functions defined in "cache.c"

// Initializes the contents of each cache line to zero.
// Assumes initialization was successful.
extern void cache_init(int cache_size);

// Prints the contents of each cache line.
extern void cache_print(int cache_size);

// Allows the user to modify the cache organization method and the cache replacement policy settings.
// Will reinitialize the cache if these settings have been modified.
extern void cache_config(int org, int pol);

// Searches the cache.
// Can utilize a varity of cache organization methods based on the user input.
// The CPU's MAR specifies the address that we are to search for.
extern void cache_bus(unsigned short mar, unsigned short* mbr, int rw, int wb);
