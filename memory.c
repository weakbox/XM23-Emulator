/*
*	Functions and definitions related to reading from and writing to virtual memory (Lab 1).
*	Written by Connor McLeod, for ECED 3403.
*/

#include "header.h"

// Shifts a 16-bit address value to the right by 1 bit, turning it into a 15-bit address. This process effectively divides the value of the address by 2.
#define word(x) x >> 1

// Emulates the CPU bus. Moves data to and from the memory address specified by the MAR.
void bus(unsigned short mar, unsigned short* mbr, int rw, int wb)
{
	switch (rw)
	{
		case READ:
			switch (wb)
			{
				case WORD:
					*mbr = mem.word[word(mar)];
					break;

				case BYTE:
					*mbr = mem.byte[mar];
					break;
			}
			break;

		case WRITE:
			switch (wb)
			{
				case WORD:
					mem.word[word(mar)] = *mbr;
					break;

				case BYTE:
					mem.byte[mar] = (unsigned char)*mbr;
					break;
			}
			break;
	}
}

// Reads data directly from a specified address in memory. Used by the initial S-Record loader.
unsigned int mem_read(unsigned int address)
{
	return mem.byte[address];
}

// Writes data directly to a specified address in memory. Used by the initial S-Record loader.
void mem_write(unsigned int address, unsigned int data)
{
	mem.byte[address] = data;
}

// Prints the contents of virtual memory in a specified range (experiencing some odd crashes every now and then, needs investigating).
void print_mem(int start, int end, int wb)
{
	if (start > end)
	{
		printf("An invalid memory address was chosen. Memory will not be printed.\n");
		return;
	}
	if (wb == WORD) 
	{ 
		if (start > MEMORY_SIZE/2 || end > MEMORY_SIZE/2 || start < 0 || end < 0)
		{
			printf("An invalid memory address was chosen. Memory will not be printed.\n");
			return;
		}
		printf("Printing contents of word-addressed memory from addresses 0x%04x to 0x%04x...\n", start, end);
	}
	else /* wb == BYTE */
	{ 
		if (start > MEMORY_SIZE || end > MEMORY_SIZE || start < 0 || end < 0)
		{
			printf("An invalid memory address was chosen. Memory will not be printed.\n");
			return;
		}
		printf("Printing contents of byte-addressed memory from addresses 0x%04x to 0x%04x...\n", start, end);
	}
	for (int i = start, j = 1 ; i <= end; i++, j++)
	{
		if (j == 1)
		{
			printf("%04x: ", i);
		}
		switch (wb)
		{
		case WORD:		
			printf("%04x ", mem.word[i]);
			break;

		case BYTE:
			printf("%02x ", mem.byte[i]);
			break;
		}
		if (j == 16 && i != end) 
		{
			printf("\n");
			j = 0;
		}
	}
	printf("\n");
}

// Loads contents of S-Record (.xme) file into virtual memory. Returns value of the program's start address (really poorly made, needs refactoring).
unsigned int load_srec(FILE* file)
{
	// S-Record decoding and initial load of memory:
	char str[MAX_RECORD_SIZE];
	unsigned short startAddress = 0;

	while (fgets(str, MAX_RECORD_SIZE, file))
	{
		char type[ID_SIZE];
		unsigned int len = 0;
		unsigned int addr = 0;
		unsigned int addrH = 0;
		unsigned int addrL = 0;
		int addrCount = 0;
		unsigned int data = 0;
		unsigned int sum = 0;
		unsigned int checksum = 0;
		int validRecord = 1;

		/* Determine type of record (fixed-location): */
		if (sscanf(&str[TYPE_OFFSET], "%2s", type) != 1)
		{
			printf("ERROR: Scanning type has failed.\n");
			close();
			return -1;
		}

		/* Determine length of record (fixed-location): */
		if (sscanf(&str[LENGTH_OFFSET], "%2x", &len) != 1)
		{
			printf("ERROR: Scanning length has failed.\n");
			close();
			return -1;
		}

		/* Determine address of record (fixed-location): */
		if (sscanf(&str[ADDRESS_OFFSET], "%2x%2x", &addrH, &addrL) != 2)
		{
			printf("ERROR: Scanning address has failed.\n");
			close();
			return -1;
		}

		// Determine if record is a valid type and meets requirements. A record can have a length no less than 3 bytes.
		if (type[0] != 'S' || (type[1] != '0' && type[1] != '1' && type[1] != '9') || (len < 3 || len > 36))
		{
			validRecord = 0;
		}

		// If the record is a valid type and meets requirements, proceed with processing.
		if (validRecord)
		{
			// Combine the high and low address bytes into one full address.
			addr = (addrH << 8) | addrL;

			/* Determine data in record (variable length): */
			for (int i = ADDRESS_OFFSET; i < ((2 * len) - CHECKSUM_OFFSET); i += 2)	/* Convert length from bytes to nibbles and remove the checksum. */
			{
				if (sscanf(&str[i + (DATA_OFFSET - ADDRESS_OFFSET)], "%2x", &data) != 1)	/* Start iterating through input string at first data byte. */
				{
					printf("ERROR: Scanning data has failed.\n");
					close();
					return -1;
				}

				// Add the current data into the sum to check with the checksum later.
				sum = sum + data;

				if (type[1] == '0')	/* Record is of S0 type, so print the filename. */
				{
					printf("%c", data);
				}

				if (type[1] == '1')
				{
					mem_write(addr + addrCount, data);
					addrCount++;
				}
			}

			if (type[1] == '1' || type[1] == '9')	/* Record is of S1 or S9 type, so print the address. */
			{
				printf("0x%x", addr);
			}
			printf("\n");

			if (type[1] == '9')	/* Record S9 type, so store the starting address. */
			{
				startAddress = addr;
			}

			/* Determine checksum (variable location): */
			if (sscanf(&str[(2 * len) + CHECKSUM_OFFSET], "%2x", &checksum) != 1)
			{
				printf("ERROR: Scanning checksum has failed.\n");
				close();
				return -1;
			}

			/* Calculate sum. Mask off everything but the LSB. */
			sum = (sum + len + addrH + addrL + checksum) & 0xFF;

			/* Check if the sum matches the checksum correctly. */
			if (sum != 0xFF)
			{
				printf("ERROR: Checksum could not be validated.\n");
			}
		}
		// The record was formatted incorrectly or was of an unsupported type.
		else
		{
			swap_newline(str);
			printf("ERROR: The inputted record '%s' could not be initialized.\n", str);
		}
	}
	return startAddress;
}
