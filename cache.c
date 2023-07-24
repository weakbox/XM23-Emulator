/*
  File: cache.c
  Author: Connor McLeod
  Date: July 23, 2023
  Description: Functions related to the CPU cache.
*/

#include "header.h"

#define CACHE_SIZE 32

typedef struct _CacheLine
{
	unsigned short address;
	unsigned short data;
	unsigned short usage;
	bool dirty;
}	CacheLine;

// Create an array of cachelines to represent the full cache.
CacheLine cache[CACHE_SIZE];

enum organization { DIRECT, ASSOCIATIVE, N_WAY };

// Initializes the cache to zero.
void cache_init()
{
	for (int i = 0; i < CACHE_SIZE; i++)
	{
		cache[i].address = 0;
		cache[i].data = 0;
		cache[i].usage = 0;
		cache[i].dirty = false;
	}
}

// Prints the contents of the cache.
void cache_print()
{
	printf("Printing cache...\n");
	for (int i = 0; i < CACHE_SIZE; i++)
	{
		printf("%02i: %04x %04x %02i %i\n", i, cache[i].address, cache[i].data, cache[i].usage, cache[i].dirty);
	}
}

// Searches and updates the cache using the direct mapping organization method.
void cache_search_direct(unsigned short address)
{

}

// Decrements the usage value of all cache lines with usages greater than 0.
// Sets the most recently accessed cache line to the maximum usage value.
void cache_dec_associative(int recent)
{
	for (int i = 0; i < CACHE_SIZE; i++)
	{
		if (cache[i].usage > 0)
		{
			cache[i].usage--;
		}
	}
	cache[recent].usage = CACHE_SIZE - 1;
}

// Overwrites the least recently used cache line.
// Returns the position of the overwritten cache line.
int cache_overwrite_associative(int address_new)
{
	int lru = CACHE_SIZE;
	int lru_pos;
	for (int i = 0; i < CACHE_SIZE; i++)
	{
		if (cache[i].usage < lru)
		{
			lru = cache[i].usage;
			lru_pos = i;
		}
	}
	// Need to use the CPU bus to retrive the data at the new specified address.
	cpu.mar = address_new;
	bus(cpu.mar, &cpu.mbr, READ, WORD);
	cache[lru_pos].address = cpu.mar; /* New cache line address. */
	cache[lru_pos].data = cpu.mbr;	  /* New cache line data. */
	return lru_pos;
}

// Searches and updates the cache using the associative organization method.
// Returns the cache line that contains the data at the requested address.
int cache_search_associative(unsigned short address)
{
	bool cache_hit = false;
	unsigned short cache_hit_line;

	// Determine if the requested address resides in cache.
	for (int i = 0; i < CACHE_SIZE; i++)
	{
		if (cache[i].address == address)
		{
			cache_hit = true;
			cache_hit_line = i;
		}
	}
	// Update cache depending on whether the address was found or not.
	if (cache_hit)
	{
		cache_dec_associative(cache_hit_line); /* Sets hit as maximum usage. Decrements cache usage. */
	}
	else // Miss.
	{
		cache_hit_line = cache_overwrite_associative(address); /* Overwrites the LRU cache line. */
		cache_dec_associative(cache_hit_line);  /* Sets "hit" as maximum usage. Decrements cache usage. */
	}
	return cache_hit_line;
}

// Searches the cache.
// Can utilize a varity of cache organization methods based on the user input.
// The CPU's MAR specifies the address that we are to search for.
void cache_search(unsigned short mar, unsigned short* mbr, int rw, int wb)
{
	// Determines the cache organization method that will be used.
	int cache_org = ASSOCIATIVE;
	int cache_hit_line;
	switch (cache_org)
	{
	case DIRECT:
		cache_search_direct(mar);
		break;

	case ASSOCIATIVE:
		cache_hit_line = cache_search_associative(mar);
		break;

	case N_WAY:
		// Bonus.
		break;
	}

	// Perform the read/write operation using the cache.
	switch (rw)
	{
	case READ:
		switch (wb)
		{
		case WORD:
			cpu.mbr = cache[cache_hit_line].data;
			break;

		case BYTE:
			cpu.mbr = cache[cache_hit_line].data; /* Needs to be conveted to the LSByte. */
			break;
		}
		break;

	case WRITE:
		switch (wb)
		{
		case WORD:
			cache[cache_hit_line].data = cpu.mbr;
			break;

		case BYTE:
			cache[cache_hit_line].data = cpu.mbr; /* Needs to be conveted to the LSByte. */
			break;
		}
		break;
	}

}
