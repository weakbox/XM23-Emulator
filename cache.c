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
enum replacement_policy { WRITE_BACK, WRITE_THROUGH };

int organization_method = DIRECT;
int replacement_policy = WRITE_BACK;

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

// Allows the user to modify the cache organization method and the cache replacement policy.
void cache_config(int org, int pol)
{
	if (org != organization_method || pol != replacement_policy) /* Cache organization method and/or replacement policy have changed. */
	{
		cache_init(); /* Re-initialize the cache to avoid errors. */
		organization_method = org;
		replacement_policy = pol;
		printf("Cache settings have been changed.\n");
		printf("Cache has been re-initialized.\n");
	}
	else
	{
		printf("No settings were changed.\n");
	}
}

// Searches the cache for a targeted address using the direct mapping organization method.
// Returns the index of the cache line that was searched.
// Modifies the hit flag if the targeted address was found.
int cache_search_direct(bool* hit)
{
	// Determine the key (index). Value is a range from 0 to CACHE_SIZE - 1.
	// MAR must be halved as MAR will always be an even number.
	int i = (cpu.mar/2) % CACHE_SIZE;

	if (cache[i].address == cpu.mar)
	{
		*hit = true;
	}
	return i;
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
	return lru_pos;
}

// Searches and updates the cache using the associative organization method.
// Returns the cache line that contains the data at the requested address.
// Modifies the hit flag if the targeted address was found.
int cache_search_associative(unsigned short address, bool* hit)
{
	unsigned short cache_hit_line;

	// Determine if the requested address resides in cache.
	for (int i = 0; i < CACHE_SIZE; i++)
	{
		if (cache[i].address == address)
		{
			*hit = true;
			cache_hit_line = i;
		}
	}
	// Determine cache line to be modified depending on whether the address was found or not.
	if (*hit)
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
void cache_bus(unsigned short mar, unsigned short* mbr, int rw, int wb)
{
	int idx = 0;	  /* Index of the cache line containing the targeted address. Assumed that it will be initialized elsewhere. */
	bool hit = false; /* Bool indicating if the targeted address was found in cache. Assumed false. */

	// Determine the cache organization method that will be used.
	switch (organization_method)
	{
	case DIRECT:
		idx = cache_search_direct(&hit);
		break;

	case ASSOCIATIVE:
		idx = cache_search_associative(mar, &hit);
		break;

	case N_WAY:
		// Bonus.
		break;
	}
	// After this switch case, it is assumed that idx contains the index of the cache line containing the targeted address.
	// Perform modifications to the cache.
	switch (rw)
	{
	case READ:
		if (hit)
		{
			#ifdef VERBOSE
			printf("[CACHE] Read hit detected.\n");
			#endif
			*mbr = cache[idx].data;
		}
		else /* Miss. Have to fetch data from main memory. */
		{
			if (cache[idx].dirty)
			{
				// Should use bus function!
				mem.word[cache[idx].address] = cache[idx].data; /* Contents of evicted cache line are written to main memory. */
			}
			bus(mar, mbr, READ, WORD);
			cache[idx].address = mar;
			cache[idx].data = *mbr;
			cache[idx].dirty = false;
		}
		break;

	case WRITE:
		switch (replacement_policy)
		{
		case WRITE_BACK:
			if (hit)
			{
				#ifdef VERBOSE
				printf("[CACHE] Write hit detected (write-back).\n");
				#endif
				cache[idx].data = *mbr;
				cache[idx].dirty = true;
			}
			else /* Miss. May have to write contents of evicted cache line to main memory. */
			{
				if (cache[idx].dirty)
				{
					// Should use bus function!
					mem.word[cache[idx].address] = cache[idx].data; /* Contents of evicted cache line are written to main memory. */
				}
				cache[idx].address = mar;
				cache[idx].data = *mbr;
				cache[idx].dirty = true;
			}
			break;

		case WRITE_THROUGH:
			if (hit)
			{
				#ifdef VERBOSE
				printf("[CACHE] Write hit detected (write-through).\n");
				#endif
				cache[idx].data = *mbr;
				mem.word[mar] = *mbr; /* Should use bus! */
			}
			else /* Miss. */
			{
				cache[idx].address = mar;
				cache[idx].data = *mbr;
				mem.word[mar] = *mbr; /* Should use bus! */
			}
			break;
		}

		break;
	}
}
