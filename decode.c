/*
  File: decode.c
  Author: Connor McLeod
  Creation Date: July 3, 2023
  Description: Executes and decodes the value of the instruction register.
*/

#include "header.h"
#include "instructions.h"

// Isolates specific bits present in the instruction register.
// These definitions allow the emulator to decode the opcode present in the instruction register.
#define BITS_15_AND_14(x)	((x >> 14) & 0x03)
#define BIT_13(x)			((x >> 13) & 0x01)
#define BITS_12_AND_11(x)	((x >> 11) & 0x03)
#define BITS_12_TO_10(x)	((x >> 10) & 0x07)
#define BIT_10(x)			((x >> 10) & 0x01)
#define BITS_9_AND_8(x)		((x >> 8)  & 0x03)
#define BITS_9_TO_7(x)		((x >> 7)  & 0x07)
#define BITS_6_AND_5(x)		((x >> 5)  & 0x03)
#define BITS_5_TO_3(x)		((x >> 3)  & 0x07)

// Isolates specific bits present in the instruction register.
// These definitions allow the emulator to extract parameters from the instruction register.
#define OFFSET_BRANCH_LINK(x)		((x)       & 0x1FFF)
#define OFFSET_BRANCH_COND(x)		((x)       & 0x03FF)
#define RC(x)						((x >> 7)  & 0x01)
#define WB(x)						((x >> 6)  & 0x01)
#define SOURCE(x)					((x >> 3)  & 0x07)
#define DEST(x)						((x)       & 0x07)
#define PRPO(x)						((x >> 9)  & 0x01)
#define DEC(x)						((x >> 8)  & 0x01)
#define INC(x)						((x >> 7)  & 0x01)
#define BYTE_MOV(x)					((x >> 3)  & 0xFF)
#define OFFSET_LOAD_STORE_REL(x)	((x >> 7)  & 0x7F)

// Reads the contents of a memory location as a 16-bit word and places it into the instruction register.
void fetch()
{
    cpu.mar = PC;
    bus(cpu.mar, &cpu.mbr, READ, WORD);
    cpu.ir = cpu.mbr;
    PC += 2;
    cpu.clock++;
}

// Determines the macro code of the particular instruction currently stored in the instruction register.
// Returns an integer value that corresponds with the given instruction.
int decode(unsigned short ir)
{
    int inst = -1;  // If no instruction is found, this value will not change from -1 and will indicate an error.
    switch (BITS_15_AND_14(cpu.ir))
    {
    case 0:
        switch (BIT_13(cpu.ir))
        {
        case 0:
            inst = BL;
            break;

        case 1:
            switch (BITS_12_TO_10(cpu.ir))
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
        switch (BIT_13(cpu.ir))
        {
        case 0:
            switch (BITS_12_TO_10(cpu.ir))
            {
            case 0:
                switch (BITS_9_AND_8(cpu.ir))
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
                switch (BITS_9_AND_8(cpu.ir))
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
                switch (BITS_9_AND_8(cpu.ir))
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

            case 3:
                switch (BITS_9_TO_7(cpu.ir))
                {
                case 0:
                    inst = MOV;
                    break;

                case 1:
                    inst = SWAP;
                    break;

                case 2:
                    switch (BITS_5_TO_3(cpu.ir))
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
                    switch (BITS_6_AND_5(cpu.ir))
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

        case 1:
            switch (BITS_12_AND_11(cpu.ir))
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
    cpu.clock++;
    return inst;
}

// Executes the instruction stored in the instruction register.
void execute(unsigned short ir, unsigned short pc)
{
    #ifdef VERBOSE
	printf("IR: %04x ", cpu.ir);
    #endif
	switch (decode(cpu.ir))
	{
		case BL: // Branch unconditionally. Store return address in link register.
            #ifdef VERBOSE
			printf("(BL)\n");
            #endif
			branch_link(OFFSET_BRANCH_LINK(cpu.ir));
			break;

		case BEQBZ: // Branch if PSW's zero bit is set.
            #ifdef VERBOSE
			printf("(BEQBZ)\n");
            #endif
			branch_conditional(psw.zero, 1, OFFSET_BRANCH_COND(cpu.ir));
			break;

		case BNEBNZ: // Branch if PSW's zero bit is cleared.
            #ifdef VERBOSE
			printf("(BNEBZ)\n");
            #endif
			branch_conditional(psw.zero, 0, OFFSET_BRANCH_COND(cpu.ir));
			break;

		case BCBHS: // Branch if PSW's carry bit is set.
            #ifdef VERBOSE
			printf("(BCBHS)\n");
            #endif
			branch_conditional(psw.carry, 1, OFFSET_BRANCH_COND(cpu.ir));
			break;

		case BNCBLO: // Branch if PSW's carry bit is cleared.
            #ifdef VERBOSE
			printf("(BNCBLO)\n");
            #endif
			branch_conditional(psw.carry, 0, OFFSET_BRANCH_COND(cpu.ir));
			break;

		case BN: // Branch if PSW's negative bit is set.
            #ifdef VERBOSE
			printf("(BN)\n");
            #endif
			branch_conditional(psw.negative, 1, OFFSET_BRANCH_COND(cpu.ir));
			break;

		case BGE: // Branch if PSW's negative and overflow bit is cleared.
            #ifdef VERBOSE
			printf("(BGE)\n");
            #endif
			branch_conditional((psw.negative || psw.overflow), 0, OFFSET_BRANCH_COND(cpu.ir));
			break;

		case BLT: // Branch if PSW's negative or overflow bit is set.
            #ifdef VERBOSE
			printf("(BLT)\n");
            #endif
			branch_conditional((psw.negative || psw.overflow), 1, OFFSET_BRANCH_COND(cpu.ir));
			break;

		case BRA: // Branch unconditionally.
            #ifdef VERBOSE
			printf("(BRA)\n");
            #endif
			branch_conditional(1, 1, OFFSET_BRANCH_COND(cpu.ir));
			break;

		case ADD: // Add source to destination.
            #ifdef VERBOSE
			printf("(ADD)\n");
            #endif
			regfile[0][DEST(cpu.ir)] = add(regfile[0][DEST(cpu.ir)], regfile[RC(cpu.ir)][SOURCE(cpu.ir)], 0, WB(cpu.ir));
			break;

		case ADDC: // Add source + carry to destination.
            #ifdef VERBOSE
			printf("(ADDC)\n");
            #endif
			regfile[0][DEST(cpu.ir)] = add(regfile[0][DEST(cpu.ir)], regfile[RC(cpu.ir)][SOURCE(cpu.ir)], psw.carry, WB(cpu.ir));
			break;

		case SUB: // Subtract source from destination (uses two's complement subtraction).
            #ifdef VERBOSE
			printf("(SUB)\n");
            #endif
			regfile[0][DEST(cpu.ir)] = add(regfile[0][DEST(cpu.ir)], ((~regfile[RC(cpu.ir)][SOURCE(cpu.ir)]) + 1), 0, WB(cpu.ir));
			break;

		case SUBC: // Subtract source + carry from destination (uses two's complement subtraction).
            #ifdef VERBOSE
			printf("(SUBC)\n");
            #endif
			regfile[0][DEST(cpu.ir)] = add(regfile[0][DEST(cpu.ir)], (~regfile[RC(cpu.ir)][SOURCE(cpu.ir)]), psw.carry, WB(cpu.ir));
			break;

		case DADD: // Decimal-add source + carry to destination.
            #ifdef VERBOSE
			printf("(DADD)\n");
            #endif
			break;

		case CMP: // Compare source with destination.
            #ifdef VERBOSE
			printf("(CMP)\n");
            #endif
			compare(regfile[0][DEST(cpu.ir)], regfile[RC(cpu.ir)][SOURCE(cpu.ir)], WB(cpu.ir));
			break;

		case XOR: // XOR's source with destination.
            #ifdef VERBOSE
			printf("(XOR)\n");
            #endif
			regfile[0][DEST(cpu.ir)] = xor(regfile[0][DEST(cpu.ir)], regfile[RC(cpu.ir)][SOURCE(cpu.ir)], WB(cpu.ir));
			break;

		case AND: // AND's source with destination.
            #ifdef VERBOSE
			printf("(AND)\n");
            #endif
			regfile[0][DEST(cpu.ir)] = and(regfile[0][DEST(cpu.ir)], regfile[RC(cpu.ir)][SOURCE(cpu.ir)], WB(cpu.ir));
			break;

		case OR: // OR's source with destination.
            #ifdef VERBOSE
			printf("(OR)\n");
            #endif
			regfile[0][DEST(cpu.ir)] = or(regfile[0][DEST(cpu.ir)], regfile[RC(cpu.ir)][SOURCE(cpu.ir)], WB(cpu.ir));
			break;

		case BIT: // Test if bit set in source is set in destination.
            #ifdef VERBOSE
			printf("(BIT)\n");
            #endif
			compare_bit(regfile[0][DEST(cpu.ir)], regfile[RC(cpu.ir)][SOURCE(cpu.ir)], WB(cpu.ir));
			break;

		case BIC: // Clear bit in destination specified by source.
            #ifdef VERBOSE
			printf("(BIC)\n");
            #endif
			regfile[0][DEST(cpu.ir)] = clear_bit(regfile[0][DEST(cpu.ir)], regfile[RC(cpu.ir)][SOURCE(cpu.ir)], WB(cpu.ir));
			break;

		case BIS: // Set bit in destination specified by source.
            #ifdef VERBOSE
			printf("(BIS)\n");
            #endif
			regfile[0][DEST(cpu.ir)] = set_bit(regfile[0][DEST(cpu.ir)], regfile[RC(cpu.ir)][SOURCE(cpu.ir)], WB(cpu.ir));
			break;

		case MOV: // Copies source into destination.
            #ifdef VERBOSE
			printf("(MOV)\n");
            #endif
			regfile[0][DEST(cpu.ir)] = move(regfile[0][DEST(cpu.ir)], regfile[0][SOURCE(cpu.ir)], WB(cpu.ir));
			break;

		case SWAP: // Swaps source and destination registers.
            #ifdef VERBOSE
			printf("(SWAP)\n");
            #endif
			swap_reg(&regfile[0][DEST(cpu.ir)], &regfile[0][SOURCE(cpu.ir)]);
			break;

		case SRA:
            #ifdef VERBOSE
			printf("(SRA)\n");
            #endif
			break;

		case RRC:
            #ifdef VERBOSE
			printf("(RRC)\n");
            #endif
			break;

		case COMP: // One's complements destination.
            #ifdef VERBOSE
			printf("(COMP)\n");
            #endif
			regfile[0][DEST(cpu.ir)] = complement(regfile[0][DEST(cpu.ir)], WB(cpu.ir));
			break;

		case SWPB: // Swaps the most and least significant bytes in the destination.
            #ifdef VERBOSE
			printf("(SWPB)\n");
            #endif
			regfile[0][DEST(cpu.ir)] = swap_byte(regfile[0][DEST(cpu.ir)]);
			break;

		case SXT: // Sign extends destination.
            #ifdef VERBOSE
			printf("(SXT)\n");
            #endif
			regfile[0][DEST(cpu.ir)] = sign_extend(regfile[0][DEST(cpu.ir)]);
			break;

		case LD: // Loads data from memory into the destination register at an address specified by the source register.
            #ifdef VERBOSE
			printf("(LD)\n");
            #endif
			load(&regfile[0][DEST(cpu.ir)], &regfile[0][SOURCE(cpu.ir)], PRPO(cpu.ir), DEC(cpu.ir), INC(cpu.ir), WB(cpu.ir));
			break;

		case ST: // Stores value of the source register into memory at an address specified by the destination register.
            #ifdef VERBOSE
			printf("(ST)\n");
            #endif
			store(&regfile[0][DEST(cpu.ir)], regfile[0][SOURCE(cpu.ir)], PRPO(cpu.ir), DEC(cpu.ir), INC(cpu.ir), WB(cpu.ir));
			break;

		case MOVL: // Moves a byte into the LSByte of the destination. Does not modify the MSByte.
            #ifdef VERBOSE
			printf("(MOVL)\n");
            #endif
			move_bytes(&regfile[0][DEST(cpu.ir)], MSBYTE(regfile[0][DEST(cpu.ir)]), BYTE_MOV(cpu.ir));
			break;

		case MOVLZ: // Moves a byte into the LSByte of the destination. The MSByte is zeroed.
            #ifdef VERBOSE
			printf("(MOVLZ)\n");
            #endif
			move_bytes(&regfile[0][DEST(cpu.ir)], 0x00,BYTE_MOV(cpu.ir));
			break;

		case MOVLS: // Moves a byte into the LSByte of the destination. The MSByte is assigned 0xFF.
            #ifdef VERBOSE
			printf("(MOVLS)\n");
            #endif
			move_bytes(&regfile[0][DEST(cpu.ir)], 0xFF, BYTE_MOV(cpu.ir));
			break;

		case MOVH: // Moves a byte into the MSByte of the destination. Does not modify the LSByte.
            #ifdef VERBOSE
			printf("(MOVH)\n");
            #endif
			move_bytes(&regfile[0][DEST(cpu.ir)], BYTE_MOV(cpu.ir), LSBYTE(regfile[0][DEST(cpu.ir)]));
			break;

		case LDR: // Loads data from memory into the destination register at an address specified by the source register, relative to a specified offset.
            #ifdef VERBOSE
			printf("(LDR)\n");
            #endif
			load_rel(&regfile[0][DEST(cpu.ir)], regfile[0][SOURCE(cpu.ir)], OFFSET_LOAD_STORE_REL(cpu.ir), WB(cpu.ir));
			break;

		case STR: // Stores value of the source register into memory at an address specified by the destination register, relative to a specified offset.
            #ifdef VERBOSE
			printf("(STR)\n");
            #endif
			store_rel(regfile[0][DEST(cpu.ir)], regfile[0][SOURCE(cpu.ir)], OFFSET_LOAD_STORE_REL(cpu.ir), WB(cpu.ir));
			break;

		default: // Somehow there was an invalid instruction present in the instruction register.
            #ifdef VERBOSE
			printf("\n");
			printf("Instruction not found!\n");
            #endif
			break;
	}
	cpu.clock++;
}
