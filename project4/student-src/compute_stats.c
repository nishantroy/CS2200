#include "../simulator-src/stats.h"

/** 
 * Use this function to compute the final statistics of the simulation
 *
 * @param stats The struct used for book keeping.
 *
 * Refer the "stats.h" file in the simulator-src
 */
void compute_stats(stats_t *stats)
{
	/********* TODO ************/

	// Use the passed in struct to make sure all the stats are computed
	
	// Think of how many times the tlb, memory and disk are accessed when you have
	// 1) A translation fault
	// 2) A page fault - Disk is read here at least once
	// 3) A dirty page is evicted - Disk is written to here
	// 4) For every access regardless of translation fault and page fault
}
