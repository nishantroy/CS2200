#include "pagetable.h"
#include <assert.h>
/**
 * This function checks the page table of the current process to find
 * the VPN to PFN mapping.
 * 
 * @param vpn The virtual page number that has to be translated
 * @param offset The page offset of the address that is being translated
 * @param rw Specifies if access is read or a write
 * @param stats The struct used for storing statistics
 *
 * @return The page frame number (aka physical frame number) PFN linked to the VPN
 */
uint64_t page_lookup(uint64_t vpn, uint64_t offset, char rw, stats_t *stats)
{
	uint64_t pfn = 0;

	// (1) Check the "current_pagetable" at the param VPN to find the mapping
	// (2) If the mapping does not exist, call the "page_fault_handler" function
	// (3) Make sure to increment stats
	// (4) Make sure to mark the entry dirty if access is a write
	// (5) Make sure to increment the frequency count of the VPN that has been accessed

	/********* TODO ************/

    //Incremement frequency for accessed index in pagetable
    current_pagetable[vpn].frequency++;


	if (current_pagetable[vpn].valid) {
        //PageTable hit
		pfn = current_pagetable[vpn].pfn;
 	} else {
        //PageTable miss, search disk for page

		pfn = page_fault_handler(vpn, rw, stats);
		current_pagetable[vpn].pfn = pfn;
	}

	if (rw == WRITE) {
        //Set dirty bit when writing to PageTable
		current_pagetable[vpn].dirty = 1;
	}

	return pfn;
}
