#include "reverselookup.h"
#include "pagetable.h"
#include "stats.h"
#include "tlb.h"

static uint64_t find_free_frame(stats_t *stats);

/**
 * This function is used to resolve a page fault. It finds a free frame, and returns
 * the PFN after doing some book keeping
 *
 * @param vpn The virtual page number that has to be looked up
 * @param rw Specifies if an access is a read or a write
 * @param stats The struct used to store stats
 *
 */
uint64_t page_fault_handler(uint64_t vpn, char rw, stats_t *stats) {
    /* DO NOT MODIFY BELOW LINE */
    uint64_t victim_pfn = find_free_frame(stats);
    /****************************/

    // The above call finds a free frame using the below function
    // Implement that first

    // Now that you have a free frame you can use:

    // (a) Update RLT
    // 		(1) Set the PCB of the RLT at victim_pfn
    // 		(2) Set the RLT at victim_pfn as valid
    // 		(3) Set the VPN in the RLT entry at victim_pfn
    // (b) Update the page table of the page faulting procees using the current_pagetable
    //		(1) Set it valid
    // 		(2) Set the correct VPN to PFN mapping


    /********* TODO ************/

    //PageFault: Increment reads_from_disk AND page_fault
    stats->reads_from_disk++;
    stats->page_faults++;

    //Set PCB, VPN and Valid Bit in reverseLookupTable
    rlt[victim_pfn].task_struct = current_process;
    rlt[victim_pfn].vpn = vpn;
    rlt[victim_pfn].valid = 1;

    //Update page table valid bit, dirty bit and reset frequency
    current_pagetable[vpn].valid = 1;
    current_pagetable[vpn].dirty = 0;
    current_pagetable[vpn].frequency = 0;

    //If writing, set dirty bit
    if (rw == WRITE) {
        current_pagetable[vpn].dirty = 1;
    }

    //Update page table PFN
    current_pagetable[vpn].pfn = victim_pfn;


    // return victim page frame number
    return victim_pfn;
}

/**
 * This functions finds a free frame by using the Least Frequently Used algorithm
 *
 * @param stats The struct used for keeping track of statistics
 *
 * @return The physical frame number that the page fault handler can use
 */
static uint64_t find_free_frame(stats_t *stats) {
    // (1) Look for an invalid frame in the RLT - Return that frame
    // (2) Use Least frequently used to identify victim frame
    //		(a) Update the victim page table
    // 			(i) Use the RLT to find the page table of the victim process
    //			(ii) Mark it invalid
    //			(iii) Increment writebacks if the victim frame is dirty
    //		(b) Return the victim frame

    uint64_t victim_pfn = 0;

    /********* TODO ************/

    //Search for invalid block and return. (index of ReverseLookupTable is PFN)
    for (uint64_t i = 0; i < (1 << rlt_size); i++) {
        if (rlt[i].valid == 0) {
            return i;
        }
    }


    //Assume first index is the LFU
    victim_pfn = 0;
    uint64_t minFreq = rlt[0].task_struct->pagetable[rlt[0].vpn].frequency;

    //Loop through rest of the ReverseLookupTable to find LFU
    for (uint64_t i = 1; i < (1 << rlt_size); i++) {
        uint64_t thisFreq = rlt[i].task_struct->pagetable[rlt[i].vpn].frequency;
        if (thisFreq < minFreq) {
            minFreq = thisFreq;
            victim_pfn = i;
        }
    }


    //ReverseLookupTable: Reset valid bit and frequency
    rlt[victim_pfn].task_struct->pagetable[rlt[victim_pfn].vpn].valid = 0;
    rlt[victim_pfn].task_struct->pagetable[rlt[victim_pfn].vpn].frequency = 0;

    //Write to disk if dirty when evicting
    if (rlt[victim_pfn].task_struct->pagetable[rlt[victim_pfn].vpn].dirty) {
        stats->writes_to_disk++;
    }

    //Reset dirty bit in ReverseLookupTable
    rlt[victim_pfn].task_struct->pagetable[rlt[victim_pfn].vpn].dirty = 0;


    // If the victim page table entry belongs to the current process, there is a chance
    // that it also resides in the TLB. So to avoid fake valid mappings from the TLB,
    // we clear that particular TLB entry.
    /******************* DO NOT MODIFY CODE BELOW THIS LINE **************/
    if (rlt[victim_pfn].task_struct == current_process) {
        tlb_clearOne(rlt[victim_pfn].vpn);
    }
    return victim_pfn;
}
