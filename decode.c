/*
*	Functions and definitions related to the decoding and execution process (Lab 2).
*	Written by Connor McLeod, for ECED 3403.
*/

#include "header.h"

// Macros to determine the integer equivalent macro definitions of instructions:

#define BL		0
#define BEQBZ	1
#define BNEBNZ	2
#define BCBHS	3
#define BNCBLO	4
#define BN		5
#define BGE		6
#define BLT		7
#define BRA		8
#define ADD		9
#define ADDC	10
#define SUB		11
#define SUBC	12 
#define DADD	13
#define CMP		14
#define XOR		15 
#define AND		16
#define OR		17
#define BIT		18
#define BIC		19
#define BIS		20
#define MOV		21
#define SWAP	22
#define SRA		23
#define RRC		24
#define COMP	25
#define SWPB	26
#define SXT		27
#define SETPRI	28
#define SVC		29
#define SETCC	30
#define CLRCC	31
#define CEX		32
#define LD		33
#define ST		34
#define MOVL	35
#define MOVLZ	36
#define MOVLS	37
#define MOVH	38
#define LDR		39
#define STR		40

// Macros to determine the values of certain bits in the instruction register (these can be simplified and reduced):

#define branch_off(x)	(x >> 9 & 0x03FF)
#define branch_pc(x)	(x >> 9 & 0x03FF)

#define arith_rc(x)		(x >> 7 & 0x01)
#define arith_wb(x)		(x >> 6 & 0x01)
#define arith_source(x) (x >> 3 & 0x07)
#define arith_dest(x)	(x      & 0x07)

#define move_byte(x)	(x >> 3 & 0xFF)
#define move_dest(x)	(x      & 0x07)

#define high_byte(x)	(x >> 8 & 0xFF)
#define low_byte(x)		(x      & 0xFF)

#define wb(x)			(x >> 6 & 0x01)
#define source(x)		(x >> 3 & 0x07)
#define dest(x)			(x      & 0x07)

#define prpo(x)			(x >> 9 & 0x01)
#define dec(x)			(x >> 8 & 0x01)
#define inc(x)			(x >> 7 & 0x01)

#define loadrel_off(x)	(x >> 7 & 0x7F)

// Macro definitions for various bits in the ir to allow the get_instruction_code function to determine the correct instruction in the instruction register.

#define bits15_14(x)	(x >> 14 & 0x03)
#define bit13(x)		(x >> 13 & 0x01)
#define bits12_11(x)	(x >> 11 & 0x03)
#define bits12_11_10(x)	(x >> 10 & 0x07)
#define bit10(x)		(x >> 10 & 0x01)
#define bits9_8(x)		(x >> 8  & 0x03)
#define bits9_8_7(x)	(x >> 7  & 0x07)
#define bits6_5(x)		(x >> 5  & 0x03)
#define bits5_4_3(x)	(x >> 3  & 0x07)

// Prints the variables of the branch instruction currently stored in the instruction register.
void print_branch(const char* name, unsigned short ir, unsigned short pc)
{
	printf("Instruction: %s\n", name);
	printf("Encoded offset: 0x%x\n", branch_off(ir));
	printf("Current PC: 0x%x\n", pc);
	printf("Branch PC: 0x%x\n", (pc + branch_pc(ir)));
}

// Prints the variables of the arithmetic instruction currently stored in the instruction register.
void print_arith(const char* name, unsigned short ir)
{
	printf("Instruction: %s\n", name);
	printf("R/C: 0x%x\n", arith_rc(ir));
	printf("W/B: 0x%x\n", arith_wb(ir));
	printf("Source: 0x%02x\n", arith_source(ir));
	printf("Destination: 0x%02x\n", arith_dest(ir));
}

// Prints the variables of the load instruction currently stored in the instruction register.
void print_load(const char* name, unsigned short ir)
{
	printf("Instruction: %s\n", name);
	printf("PR/PO: 0x%x\n", prpo(ir));
	printf("Decrement: 0x%x\n", dec(ir));
	printf("Increment: 0x%x\n", inc(ir));
	printf("W/B: 0x%x\n", wb(ir));
	printf("Source: 0x%02x\n", source(ir));
	printf("Destination: 0x%02x\n", dest(ir));
}

// Prints the variables of the move instruction currently stored in the instruction register.
void print_move(const char* name, unsigned short ir)
{
	printf("Instruction: %s\n", name);
	printf("Byte: 0x%02x\n", move_byte(ir));
	printf("Destination: 0x%02x\n", move_dest(ir));
}

// Prints the variables of the load relative instruction currently stored in the instruction register.
void print_loadrel(const char* name, unsigned short ir)
{
	printf("Instruction: %s\n", name);
	printf("Offset: %i\n", sign_extend_offset(loadrel_off(ir)));
	printf("W/B: 0x%x\n", wb(ir));
	printf("Source: 0x%02x\n", source(ir));
	printf("Destination: 0x%02x\n", dest(ir));
}

// Sign extends the 7 bit offset value of the load relative instruction (can this be done in a more clever way?).
short sign_extend_offset(unsigned short offset)
{
	short sext_offset = offset;
	if (offset >> 6 == 1)	/* If the MSBit is high, then the offset is negative. */
	{
		sext_offset = (offset | 0xFF80);
	}
	return sext_offset;
}

// Determines the macro code of the particular instruction currently stored in the instruction register. Returns an integer value that corresponds with the given instruction.
int decode(unsigned short ir)
{
	int inst = 0;
	switch (bits15_14(ir))
	{
		case 0:
			switch (bit13(ir))
			{
				case 0:
					inst = BL;
					break;

				case 1:	/* Branch block: */
					switch (bits12_11_10(ir))
					{
						case 0:
							inst = BEQBZ;
							break;

						case 1:
							inst = BNEBNZ;
							break;

						case 2:
							inst = BCBHS;
							break;

						case 3:
							inst = BNCBLO;
							break;

						case 4:
							inst = BN;
							break;

						case 5:
							inst = BGE;
							break;

						case 6:
							inst = BLT;
							break;

						case 7:
							inst = BRA;
							break;
					}
					break;
			}
			break;

		case 1:
			switch (bit13(ir))
			{
			case 0:
				switch (bits12_11_10(ir))
				{
					case 0:
						switch (bits9_8(ir))
						{
							case 0:
								inst = ADD;
								break;

							case 1:
								inst = ADDC;
								break;

							case 2:
								inst = SUB;
								break;

							case 3:
								inst = SUBC;
								break;
						}
						break;

					case 1:
						switch (bits9_8(ir))
						{
							case 0:
								inst = DADD;
								break;

							case 1:
								inst = CMP;
								break;

							case 2:
								inst = XOR;
								break;

							case 3:
								inst = AND;
								break;
						}
						break;

					case 2:
						switch (bits9_8(ir))
						{
							case 0:
								inst = OR;
								break;

							case 1:
								inst = BIT;
								break;

							case 2:
								inst = BIC;
								break;

							case 3:
								inst = BIS;
								break;
						}
						break;

					case 3:	/* Strange instruction block: */
						switch (bits9_8_7(ir))
						{
							case 0:
								inst = MOV;
								break;

							case 1:
								inst = SWAP;
								break;

							case 2:
								switch (bits5_4_3(ir))
								{
									case 0:
										inst = SRA;
										break;

									case 1:
										inst = RRC;
										break;

									case 2:
										inst = COMP;
										break;

									case 3:
										inst = SWPB;
										break;

									case 4:
										inst = SXT;
										break;
								}
								break;

							case 3:
								switch (bits6_5(ir))
								{
									case 0:
										inst = SETPRI;
										break;

									case 1:
										inst = SVC;
										break;

									case 2:
										inst = SETCC;
										break;

									case 3:
										inst = CLRCC;
										break;
								}
								break;
						}
						break;

					case 4:
						inst = CEX;
						break;

					case 6:
						inst = LD;
						break;

					case 7:
						inst = ST;
						break;
				}
				break;

			case 1:	/* Move byte instruction block: */
				switch(bits12_11(ir))
				{
					case 0:
						inst = MOVL;
						break;

					case 1:
						inst = MOVLZ;
						break;

					case 2:
						inst = MOVLS;
						break;

					case 3:
						inst = MOVH;
						break;
				}
				break;
			}
			break;

		case 2:
			inst = LDR;
			break;

		case 3:
			inst = STR;
			break;
	}
	return inst;
}

// Executes the instruction corrently stored in the instruction register.
void execute(unsigned short ir, unsigned short pc)
{
	printf("Instruction Register: %04x\n", ir);

	switch (decode(ir))
	{
		// Branch cases:
		case BL:
			break;

		case BEQBZ:
			print_branch("BEQBZ", ir, pc);
			break;

		case BNEBNZ:
			print_branch("BNEBNZ", ir, pc);
			break;

		case BCBHS:
			print_branch("BCBHS", ir, pc);
			break;

		case BNCBLO:
			print_branch("BNCBLO", ir, pc);
			break;

		case BN:
			print_branch("BN", ir, pc);
			break;

		case BGE:
			print_branch("BGE", ir, pc);
			break;

		case BLT:
			print_branch("BLT", ir, pc);
			break;

		case BRA:
			print_branch("BRA", ir, pc);
			break;

		// Arithmetic cases:
		case ADD:
			print_arith("ADD", ir);
			break;

		case ADDC:
			print_arith("ADDC", ir);
			break;

		case SUB:
			print_arith("SUB", ir);
			break;

		case SUBC:
			print_arith("SUBC", ir);
			break;

		case DADD:
			print_arith("DADD", ir);
			break;

		case CMP:
			print_arith("CMP", ir);
			break;

		case XOR:
			print_arith("XOR", ir);
			break;

		case AND:
			print_arith("AND", ir);
			break;

		case OR:
			print_arith("OR", ir);
			break;

		case BIT:
			print_arith("BIT", ir);
			break;

		case BIC:
			print_arith("BIC", ir);
			break;

		case BIS:
			print_arith("BIS", ir);
			break;

		// Load/store cases:
		case LD:
			print_load("LD", ir);
			load(&regfile[0][dest(ir)], &regfile[0][source(ir)], prpo(ir), dec(ir), inc(ir), wb(ir));
			break;

		case ST:
			print_load("ST", ir);

		// Move byte cases:
		case MOVL:
			print_move("MOVL", ir);
			move_bytes(&regfile[0][move_dest(ir)], high_byte(regfile[0][move_dest(ir)]), move_byte(ir));
			break;

		case MOVLZ:
			print_move("MOVLZ", ir);
			move_bytes(&regfile[0][move_dest(ir)], 0x00, move_byte(ir));
			break;

		case MOVLS:
			print_move("MOVLS", ir);
			move_bytes(&regfile[0][move_dest(ir)], 0xFF, move_byte(ir));
			break;

		case MOVH:
			print_move("MOVH", ir);
			move_bytes(&regfile[0][move_dest(ir)], move_byte(ir), low_byte(regfile[0][move_dest(ir)]));
			break;

		// Load/store relative cases:
		case LDR:
			print_loadrel("LDR", ir);
			load_rel(&regfile[0][dest(ir)], regfile[0][source(ir)], sign_extend_offset(loadrel_off(ir)), wb(ir));
			break;

		case STR:
			print_loadrel("STR", ir);
			break;
	}
}
