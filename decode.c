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

// Determines the macro code of the particular instruction currently stored in the instruction register.
// Returns an integer value that corresponds with the given instruction.
int decode(unsigned short ir)
{
    int inst = -1;  // If no instruction is found, this value will not change from -1 and will indicate an error.
    switch (BITS_15_AND_14(ir))
    {
    case 0:
        switch (BIT_13(ir))
        {
        case 0:
            inst = BL;
            break;

        case 1:
            switch (BITS_12_TO_10(ir))
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
        switch (BIT_13(ir))
        {
        case 0:
            switch (BITS_12_TO_10(ir))
            {
            case 0:
                switch (BITS_9_AND_8(ir))
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
                switch (BITS_9_AND_8(ir))
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
                switch (BITS_9_AND_8(ir))
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
                switch (BITS_9_TO_7(ir))
                {
                case 0:
                    inst = MOV;
                    break;

                case 1:
                    inst = SWAP;
                    break;

                case 2:
                    switch (BITS_5_TO_3(ir))
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
                    switch (BITS_6_AND_5(ir))
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
            switch (BITS_12_AND_11(ir))
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
	printf("IR: 0x%04x ", ir);

	switch (decode(ir))
	{
		case BL: // Branch unconditionally. Store return address in link register.
			printf("(BL)\n");
			branch_link(OFFSET_BRANCH_LINK(ir));
			break;

		case BEQBZ: // Branch if PSW's zero bit is set.
			printf("(BEQBZ)\n");
			branch_conditional(psw.zero, 1, OFFSET_BRANCH_COND(ir));
			break;

		case BNEBNZ: // Branch if PSW's zero bit is cleared.
			printf("(BNEBZ)\n");
			branch_conditional(psw.zero, 0, OFFSET_BRANCH_COND(ir));
			break;

		case BCBHS: // Branch if PSW's carry bit is set.
			printf("(BCBHS)\n");
			branch_conditional(psw.carry, 1, OFFSET_BRANCH_COND(ir));
			break;

		case BNCBLO: // Branch if PSW's carry bit is cleared.
			printf("(BNCBLO)\n");
			branch_conditional(psw.carry, 0, OFFSET_BRANCH_COND(ir));
			break;

		case BN: // Branch if PSW's negative bit is set.
			printf("(BN)\n");
			branch_conditional(psw.negative, 1, OFFSET_BRANCH_COND(ir));
			break;

		case BGE: // Branch if PSW's negative and overflow bit is cleared.
			printf("(BGE)\n");
			branch_conditional((psw.negative || psw.overflow), 0, OFFSET_BRANCH_COND(ir));
			break;

		case BLT: // Branch if PSW's negative or overflow bit is set.
			printf("(BLT)\n");
			branch_conditional((psw.negative || psw.overflow), 1, OFFSET_BRANCH_COND(ir));
			break;

		case BRA: // Branch unconditionally.
			printf("(BRA)\n");
			branch_conditional(1, 1, OFFSET_BRANCH_COND(ir));
			break;

		case ADD: // Add source to destination.
			printf("(ADD)\n");
			regfile[0][DEST(ir)] = add(regfile[0][DEST(ir)], regfile[RC(ir)][SOURCE(ir)], 0, WB(ir));
			break;

		case ADDC: // Add source + carry to destination.
			printf("(ADDC)\n");
			regfile[0][DEST(ir)] = add(regfile[0][DEST(ir)], regfile[RC(ir)][SOURCE(ir)], psw.carry, WB(ir));
			break;

		case SUB: // Subtract source from destination (uses two's complement subtraction).
			printf("(SUB)\n");
			regfile[0][DEST(ir)] = add(regfile[0][DEST(ir)], ((~regfile[RC(ir)][SOURCE(ir)]) + 1), 0, WB(ir));
			break;

		case SUBC: // Subtract source + carry from destination (uses two's complement subtraction).
			printf("(SUBC)\n");
			regfile[0][DEST(ir)] = add(regfile[0][DEST(ir)], (~regfile[RC(ir)][SOURCE(ir)]), psw.carry, WB(ir));
			break;

		case DADD: // Decimal-add source + carry to destination.
			printf("(DADD)\n");
			break;

		case CMP: // Compare source with destination.
			printf("(CMP)\n");
			compare(regfile[0][DEST(ir)], regfile[RC(ir)][SOURCE(ir)], WB(ir));
			break;

		case XOR: // XOR's source with destination.
			printf("(XOR)\n");
			regfile[0][DEST(ir)] = xor(regfile[0][DEST(ir)], regfile[RC(ir)][SOURCE(ir)], WB(ir));
			break;

		case AND: // AND's source with destination.
			printf("(AND)\n");
			regfile[0][DEST(ir)] = and(regfile[0][DEST(ir)], regfile[RC(ir)][SOURCE(ir)], WB(ir));
			break;

		case OR: // OR's source with destination.
			printf("(OR)\n");
			regfile[0][DEST(ir)] = or(regfile[0][DEST(ir)], regfile[RC(ir)][SOURCE(ir)], WB(ir));
			break;

		case BIT: // Test if bit set in source is set in destination.
			printf("(BIT)\n");
			compare_bit(regfile[0][DEST(ir)], regfile[RC(ir)][SOURCE(ir)], WB(ir));
			break;

		case BIC: // Clear bit in destination specified by source.
			printf("(BIC)\n");
			regfile[0][DEST(ir)] = clear_bit(regfile[0][DEST(ir)], regfile[RC(ir)][SOURCE(ir)], WB(ir));
			break;

		case BIS: // Set bit in destination specified by source.
			printf("(BIS)\n");
			regfile[0][DEST(ir)] = set_bit(regfile[0][DEST(ir)], regfile[RC(ir)][SOURCE(ir)], WB(ir));
			break;

		case MOV: // Copies source into destination.
			printf("(MOV)\n");
			regfile[0][DEST(ir)] = move(regfile[0][DEST(ir)], regfile[0][SOURCE(ir)], WB(ir));
			break;

		case SWAP: // Swaps source and destination registers.
			printf("(SWAP)\n");
			swap_reg(&regfile[0][DEST(ir)], &regfile[0][SOURCE(ir)]);
			break;

		case SRA:
			printf("(SRA)\n");
			break;

		case RRC:
			printf("(RRC)\n");
			break;

		case COMP: // One's complements destination.
			printf("(COMP)\n");
			regfile[0][DEST(ir)] = complement(regfile[0][DEST(ir)], WB(ir));
			break;

		case SWPB: // Swaps the most and least significant bytes in the destination.
			printf("(SWPB)\n");
			regfile[0][DEST(ir)] = swap_byte(regfile[0][DEST(ir)]);
			break;

		case SXT: // Sign extends destination.
			printf("(SXT)\n");
			regfile[0][DEST(ir)] = sign_extend(regfile[0][DEST(ir)]);
			break;

		case LD: // Loads data from memory into the destination register at an address specified by the source register.
			printf("(LD)\n");
			load(&regfile[0][DEST(ir)], &regfile[0][SOURCE(ir)], PRPO(ir), DEC(ir), INC(ir), WB(ir));
			break;

		case ST: // Stores value of the source register into memory at an address specified by the destination register.
			printf("(ST)\n");
			store(&regfile[0][DEST(ir)], regfile[0][SOURCE(ir)], PRPO(ir), DEC(ir), INC(ir), WB(ir));
			break;

		case MOVL: // Moves a byte into the LSByte of the destination. Does not modify the MSByte.
			printf("(MOVL)\n");
			move_bytes(&regfile[0][DEST(ir)], MSBYTE(regfile[0][DEST(ir)]), BYTE_MOV(ir));
			break;

		case MOVLZ: // Moves a byte into the LSByte of the destination. The MSByte is zeroed.
			printf("(MOVLZ)\n");
			move_bytes(&regfile[0][DEST(ir)], 0x00,BYTE_MOV(ir));
			break;

		case MOVLS: // Moves a byte into the LSByte of the destination. The MSByte is assigned 0xFF.
			printf("(MOVLS)\n");
			move_bytes(&regfile[0][DEST(ir)], 0xFF, BYTE_MOV(ir));
			break;

		case MOVH: // Moves a byte into the MSByte of the destination. Does not modify the LSByte.
			printf("(MOVH)\n");
			move_bytes(&regfile[0][DEST(ir)], BYTE_MOV(ir), LSBYTE(regfile[0][DEST(ir)]));
			break;

		case LDR: // Loads data from memory into the destination register at an address specified by the source register, relative to a specified offset.
			printf("(LDR)\n");
			load_rel(&regfile[0][DEST(ir)], regfile[0][SOURCE(ir)], OFFSET_LOAD_STORE_REL(ir), WB(ir));
			break;

		case STR: // Stores value of the source register into memory at an address specified by the destination register, relative to a specified offset.
			printf("(STR)\n");
			store_rel(regfile[0][DEST(ir)], regfile[0][SOURCE(ir)], OFFSET_LOAD_STORE_REL(ir), WB(ir));
			break;

		default: // Somehow there was an invalid instruction present in the instruction register.
			printf("\n");
			printf("Instruction not found!\n");
			break;
	}
	cpu.clock++;
}
