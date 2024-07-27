/*
  File: cache.c
  Author: Connor McLeod
  Date: July 23, 2023
  Description: Functions related to the CPU cache.
*/

#include "header.h"

#define LSBYTE(x) ((x) & 0xFF)

enum organization { DIRECT, ASSOCIATIVE, N_WAY };
enum replacement_policy { WRITE_BACK, WRITE_THROUGH };

typedef struct _CacheLine
{
	unsigned short address;
	unsigned short data;
	unsigned short usage;
	bool dirty;
}	CacheLine;

// Create an array of cachelines to represent the full cache.
CacheLine cache[CACHE_SIZE];

int organization_method = DIRECT;	 /* Global to specify the cache organization method. */
int replacement_policy = WRITE_BACK; /* Global to specify the cache replacement policy. */

// Initializes the contents of each cache line to zero.
// -> Assumes initialization was successful.
void cache_init(int cache_size)
{
	for (int i = 0; i < cache_size; i++)
	{
		cache[i].address = 0;
		cache[i].data = 0;
		cache[i].usage = 0;
		cache[i].dirty = false;
	}
}

// Prints the contents of each cache line.
void cache_print(int cache_size)
{
	printf("Printing %i cache lines...\n", cache_size);

	for (int i = 0; i < cache_size; i++)
	{
		printf("%02i: %04x %04x %02i %i\n", i, cache[i].address, cache[i].data, cache[i].usage, cache[i].dirty);
	}
}

// Allows the user to modify the cache organization method and the cache replacement policy settings.
// -> Will reinitialize the cache if these settings have been modified.
void cache_config(int org, int pol)
{
	if ((org != organization_method || pol != replacement_policy) && ((org >= 0 && org <= 1) && (pol >= 0 && pol <= 1))) /* Cache organization method and/or replacement policy have changed. */
	{
		printf("Cache settings were modified.\n");
		cache_init(CACHE_SIZE);    /* Re-initialize the cache to avoid errors. */
		organization_method = org; /* Organization method global settings are changed. */
		replacement_policy = pol;  /* Replacement policy global settings are changed.*/
		printf("Cache has been reinitialized.\n");
	}
	else
	{
		printf("Cache settings were not modified.\n");
		printf("Cache has not been reinitialized.\n");
	}
}

// Searches the cache for a targeted address using the direct mapping organization method.
// -> Returns the index of the cache line that was searched.
// -> Sets the hit flag if the targeted address was found.
int cache_search_direct(unsigned short target_address, bool* hit)
{
	// Determine the key (index). Value is a range from 0 to CACHE_SIZE - 1.
	// -> Target address must be halved as it will always be an even number.
	int i = (target_address/2) % CACHE_SIZE;

	if (cache[i].address == target_address)
	{
		*hit = true;
	}

	return i;
}

// Decrements the usage of all cache lines with usages greater than 0.
// -> Sets the most recently accessed cache line to the maximum usage value.
void cache_dec_associative(int mru)
{
	for (int i = 0; i < CACHE_SIZE; i++)
	{
		if (cache[i].usage > 0)
		{
			cache[i].usage--;
		}
	}

	cache[mru].usage = CACHE_SIZE - 1;
}

// Determines the index of the LRU cache line.
// -> Returns the index of the LRU cache line.
int cache_find_lru_associative()
{
	int lru = CACHE_SIZE;
	int lru_idx = CACHE_SIZE;

	// Performs a linear search through the cache to determine the LRU cache line.
	for (int i = 0; i < CACHE_SIZE; i++)
	{
		if (cache[i].usage < lru)
		{
			lru = cache[i].usage;
			lru_idx = i;
		}
	}

	return lru_idx;
}

// Searches and updates the cache using the associative organization method.
// -> Returns the index of the cache line that either contained the target address or should be overwritten.
// -> Sets the hit flag if the targeted address was found.
int cache_search_associative(unsigned short target_address, bool* hit)
{
	unsigned short target_index;

	// Determine if the requested address resides in cache.
	for (int i = 0; i < CACHE_SIZE; i++)
	{
		if (cache[i].address == target_address)
		{
			*hit = true;
			target_index = i;
		}
	}

	// If a miss occurred, the index of the cache line to overwrite must be determined.
	if (*hit == false)
	{
		target_index = cache_find_lru_associative();
	}

	cache_dec_associative(target_index);
	return target_index;
}

// Reads cache data from the provided cache index.
// -> Replaces address and data in the targeted cache line if a miss has been specified.
void cache_read(unsigned short target_address, int wb, int idx, bool hit)
{
	if (hit)
	{
		#ifdef VERBOSE
		printf("[CACHE] Read hit!\n");
		#endif

		cpu.mbr = (wb == WORD) ? cache[idx].data : LSBYTE(cache[idx].data);
	}
	else /* Miss. Have to fetch data from main memory. */
	{
		if (cache[idx].dirty)
		{
			// Write contents of cache line to main memory.
			cpu.mar = cache[idx].address;
			cpu.mbr = cache[idx].data;
			bus(cpu.mar, &cpu.mbr, WRITE, WORD);
		}

		// Read contents of memory address from main memory.
		cpu.mar = target_address;
		bus(cpu.mar, &cpu.mbr, READ, (wb == WORD) ? WORD : BYTE);

		// Store contents of memory address into cache line.
		cache[idx].address = cpu.mar;
		cache[idx].data = cpu.mbr;
		cache[idx].dirty = false;
	}
}

// Writes cache data to the provided cache index.
// -> Replaces address and data in the targeted cache line if a miss has been specified.
// -> Can use write-back or write-through replacement policies (defaults to write-back).
void cache_write(unsigned short target_address, unsigned short target_data, int wb, int idx, bool hit)
{
	// Policy determines how main memory will be updated when a write occurs.
	switch (replacement_policy)
	{
	case WRITE_BACK:
		if (hit)
		{
			#ifdef VERBOSE
			printf("[CACHE] Write hit!\n");
			#endif	
		}
		else /* Miss. If dirty bit is set, contents of evicted cache line are written to main memory. */
		{
			if (cache[idx].dirty)
			{
				// Write contents of cache line to main memory.
				cpu.mar = cache[idx].address;
				cpu.mbr = cache[idx].data;
				bus(cpu.mar, &cpu.mbr, WRITE, WORD);
			}

			// Return target address and buffer to the CPU.
			cpu.mar = target_address;
			cpu.mbr = target_data;
		}

		// A write using the write-back policy always results in the dirty bit being set.
		cache[idx].dirty = true;
		break;

	case WRITE_THROUGH:
		if (hit)
		{
			#ifdef VERBOSE
			printf("[CACHE] Write hit!\n");
			#endif			
		}

		// A write using the write-through policy always results in the data being written to main memory.
		bus(cpu.mar, &cpu.mbr, WRITE, (wb == WORD) ? WORD : BYTE);
		break;
	}

	// Regardless of policy, the cache line specified by the index is always written to.
	cache[idx].address = cpu.mar;
	cache[idx].data = (wb == WORD) ? cpu.mbr : LSBYTE(cpu.mbr);
}

// Searches the cache.
// -> Can utilize a varity of cache organization methods based on the user input.
// -> The CPU's MAR specifies the address that we are to search for.
void cache_bus(unsigned short mar, unsigned short* mbr, int rw, int wb)
{
	unsigned short target_address = mar; /* Address to be searched for in cache. */
	unsigned short target_data = *mbr; /* Contents of data to be written. Only used during a write operation. */

	int idx = 0;	  /* Index of the cache line containing the targeted address. Assumed that it will be initialized elsewhere. */
	bool hit = false; /* Flag indicating if the targeted address was found in cache. Assumed false. */

	// Determine the cache organization method that will be used.
	// -> Assume that a valid organization method was chosen.
	switch (organization_method)
	{
	case DIRECT:
		idx = cache_search_direct(target_address, &hit);
		break;

	case ASSOCIATIVE:
		idx = cache_search_associative(target_address, &hit);
		break;

	case N_WAY:
		// Bonus.
		break;
	}

	// Perform modifications to the cache.
	switch (rw)
	{
	case READ:
		cache_read(target_address, wb, idx, hit);
		break;

	case WRITE:
		cache_write(target_address, target_data, wb, idx, hit);
		break;
	}
}
