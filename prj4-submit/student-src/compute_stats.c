#include "stats.h"

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


    //Get necessary values from stats struct
    double writesToDisk = (double) stats->writes_to_disk;
    double access = (double) stats->accesses;
    double pageFaults = (double) stats->page_faults;
    double transFaults = (double) stats->translation_faults;
    double TLBREAD = stats->TLB_READ_TIME;
    double DISKREAD = stats->DISK_READ_TIME;
    double DISKWRITE = stats->DISK_WRITE_TIME;
    double MEMREAD = stats->MEMORY_READ_TIME;


    //Calculate AAT. First find AAT for Disk, use that to find AAT for PageTable (mem), use that to find total AAT.
    double diskAccessTime = DISKREAD + (writesToDisk/pageFaults) * DISKWRITE;
    double memAccessTime = MEMREAD + (pageFaults / transFaults) * diskAccessTime;
    double tlbAccessTime = TLBREAD + MEMREAD + (transFaults/access) * memAccessTime;

    stats->AAT = tlbAccessTime;

}
