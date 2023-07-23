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

// Searches the cache!
void cache_search(unsigned short mar, unsigned short* mbr, int rw, int wb) 
{

}
