/*
*	Functions and definitions related to the decoding and execution process (Lab 2).
*	Written by Connor McLeod, for ECED 3403.
*/

#include "header.h"
#include "instructions.h"

// Macros to determine the values of certain bits in the instruction register (these can be simplified and reduced):

#define branch_off(x)	(x >> 9 & 0x03FF)
#define OFFSET_BRANCH_COND(x) (x >> 9 & 0x03FF)
#define OFFSET_BRANCH_LINK(x) ((x >> 12) & 0x1FFF)

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

#define WB(x)			(x >> 6 & 0x01)
#define SOURCE(x)		(x >> 3 & 0x07)
#define DEST(x)			(x      & 0x07)
#define RC(x)			(x >> 7 & 0x01)

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
		case BL:	// Branch unconditionally. Store return address in link register.
			branch_link(OFFSET_BRANCH_LINK(ir));
			break;

		case BEQBZ:		// Branch if PSW's zero bit is set.
			print_branch("BEQBZ", ir, pc);
			branch_conditional(psw.zero, 1, OFFSET_BRANCH_COND(ir));
			break;

		case BNEBNZ:	// Branch if PSW's zero bit is cleared.
			print_branch("BNEBNZ", ir, pc);
			branch_conditional(psw.zero, 0, OFFSET_BRANCH_COND(ir));
			break;

		case BCBHS:		// Branch if PSW's carry bit is set.
			print_branch("BCBHS", ir, pc);
			branch_conditional(psw.carry, 1, OFFSET_BRANCH_COND(ir));
			break;

		case BNCBLO:	// Branch if PSW's carry bit is cleared.
			print_branch("BNCBLO", ir, pc);
			branch_conditional(psw.carry, 0, OFFSET_BRANCH_COND(ir));
			break;

		case BN:		// Branch if PSW's negative bit is set.
			print_branch("BN", ir, pc);
			branch_conditional(psw.negative, 1, OFFSET_BRANCH_COND(ir));
			break;

		case BGE:		// Branch if PSW's negative and overflow bit is cleared.
			print_branch("BGE", ir, pc);
			branch_conditional((psw.negative || psw.overflow), 0, OFFSET_BRANCH_COND(ir));
			break;

		case BLT:		// Branch if PSW's negative or overflow bit is set.
			print_branch("BLT", ir, pc);
			branch_conditional((psw.negative || psw.overflow), 1, OFFSET_BRANCH_COND(ir));
			break;

		case BRA:		// Branch unconditionally.
			print_branch("BRA", ir, pc);
			branch_conditional(1, 1, OFFSET_BRANCH_COND(ir));
			break;

		case ADD:	// Add source to destination.
			print_arith("ADD", ir);
			regfile[0][DEST(ir)] = add(regfile[0][DEST(ir)], regfile[RC(ir)][SOURCE(ir)], 0, WB(ir));
			break;

		case ADDC:	// Add source + carry to destination.
			print_arith("ADDC", ir);
			regfile[0][DEST(ir)] = add(regfile[0][DEST(ir)], regfile[RC(ir)][SOURCE(ir)], psw.carry, WB(ir));
			break;

		case SUB:	// Subtract source from destination (uses two's complement subtraction).
			print_arith("SUB", ir);
			regfile[0][DEST(ir)] = add(regfile[0][DEST(ir)], (~regfile[RC(ir)][SOURCE(ir)] + 1), 0, WB(ir));
			break;

		case SUBC:	// Subtract source + carry from destination (uses two's complement subtraction).
			print_arith("SUBC", ir);
			regfile[0][DEST(ir)] = add(regfile[0][DEST(ir)], (~regfile[RC(ir)][SOURCE(ir)]), psw.carry, WB(ir));
			break;

		case DADD:	// Decimal-add source + carry to destination.
			print_arith("DADD", ir);
			break;

		case CMP:	// Compare source with destination.
			print_arith("CMP", ir);
			compare(regfile[0][DEST(ir)], regfile[RC(ir)][SOURCE(ir)], WB(ir));
			break;

		case XOR:	// XOR's source with destination.
			print_arith("XOR", ir);
			regfile[0][DEST(ir)] = xor(regfile[0][DEST(ir)], regfile[RC(ir)][SOURCE(ir)], WB(ir));
			break;

		case AND:	// AND's source with destination.
			print_arith("AND", ir);
			regfile[0][DEST(ir)] = and(regfile[0][DEST(ir)], regfile[RC(ir)][SOURCE(ir)], WB(ir));
			break;

		case OR:	// OR's source with destination.
			print_arith("OR", ir);
			regfile[0][DEST(ir)] = or(regfile[0][DEST(ir)], regfile[RC(ir)][SOURCE(ir)], WB(ir));
			break;

		case BIT:	// Test if bit set in source is set in destination.
			print_arith("BIT", ir);
			compare_bit(regfile[0][DEST(ir)], regfile[RC(ir)][SOURCE(ir)], WB(ir));
			break;

		case BIC:	// Clear bit in destination specified by source.
			print_arith("BIC", ir);
			regfile[0][DEST(ir)] = clear_bit(regfile[0][DEST(ir)], regfile[RC(ir)][SOURCE(ir)], WB(ir));
			break;

		case BIS:	// Set bit in destination specified by source.
			print_arith("BIS", ir);
			regfile[0][DEST(ir)] = set_bit(regfile[0][DEST(ir)], regfile[RC(ir)][SOURCE(ir)], WB(ir));
			break;

		case MOV:	// Copies source into destination.
			regfile[0][DEST(ir)] = move(regfile[0][DEST(ir)], regfile[0][SOURCE(ir)], WB(ir));
			break;

		case SWAP:	// Swaps source and destination registers.
			swap_reg(&regfile[0][DEST(ir)], &regfile[0][SOURCE(ir)]);
			break;

		case SRA:
			break;

		case RRC:
			break;

		case COMP:	// One's complements destination.
			regfile[0][DEST(ir)] = complement(regfile[0][DEST(ir)], WB(ir));
			break;

		case SWPB:
			break;

		case SXT:	// Sign extends destination.
			regfile[0][DEST(ir)] = sign_extend(regfile[0][DEST(ir)]);
			break;

		case LD:
			print_load("LD", ir);
			load(&regfile[0][dest(ir)], &regfile[0][source(ir)], prpo(ir), dec(ir), inc(ir), wb(ir));
			break;

		case ST:
			print_load("ST", ir);

		case MOVL:
			print_move("MOVL", ir);
			move_bytes(&regfile[0][DEST(ir)], high_byte(regfile[0][move_dest(ir)]), move_byte(ir));
			break;

		case MOVLZ:
			print_move("MOVLZ", ir);
			move_bytes(&regfile[0][DEST(ir)], 0x00, move_byte(ir));
			break;

		case MOVLS:
			print_move("MOVLS", ir);
			move_bytes(&regfile[0][DEST(ir)], 0xFF, move_byte(ir));
			break;

		case MOVH:
			print_move("MOVH", ir);
			move_bytes(&regfile[0][DEST(ir)], move_byte(ir), low_byte(regfile[0][DEST(ir)]));
			break;

		case LDR:
			print_loadrel("LDR", ir);
			load_rel(&regfile[0][DEST(ir)], regfile[0][SOURCE(ir)], sign_extend_offset(loadrel_off(ir)), wb(ir));
			break;

		case STR:
			print_loadrel("STR", ir);
			break;
	}
}
