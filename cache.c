/*
  File: cache.c
  Author: Connor McLeod
  Date: July 23, 2023
  Description: Functions related to the CPU cache.
*/

#include <stdio.h>
#include <stdbool.h>

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
	cache[lru_pos].address = address_new;
	return lru_pos;
}

// Searches and updates the cache using the associative organization method.
void cache_search_associative(unsigned short address)
{
	bool hit = false;
	unsigned short hit_cache_line;

	// Determine if the requested address resides in cache.
	for (int i = 0; i < CACHE_SIZE; i++)
	{
		if (cache[i].address == address)
		{
			hit = true;
			hit_cache_line = i;
		}
	}
	// Update cache depending on whether the address was found or not.
	if (hit)
	{
		cache_dec_associative(address);
	}
	else // Miss.
	{
		cache_dec_associative(cache_overwrite_associative(address));
	}
}

// Searches the cache.
// Can utilize a varity of cache organization methods based on the user input.
// The CPU's MAR specifies the address that we are to search for.
void cache_search(unsigned short mar, unsigned short* mbr, int rw, int wb)
{
	// Determines the cache organization method that will be used.
	int cache_org = ASSOCIATIVE;
	switch (cache_org)
	{
	case DIRECT:
		cache_search_direct(mar);
		break;

	case ASSOCIATIVE:
		cache_search_associative(mar);
		break;

	case N_WAY:
		// Bonus.
		break;
	}
}
