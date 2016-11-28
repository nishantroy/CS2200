#include "../simulator-src/reverselookup.h"
#include "../simulator-src/pagetable.h"
#include "../simulator-src/stats.h"
#include "../simulator-src/tlb.h"

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

    stats->reads_from_disk++;

    rlte_t* rlte = rlt + victim_pfn;
    rlte->task_struct = current_process;
    rlte->vpn = vpn;
    rlte->valid = 1;

    pte_t * pte = current_pagetable + vpn;
//    pte->pfn = victim_pfn;
    pte->valid = 1;
    pte->dirty = 0;
    pte->frequency = 0;

//
//    rlt[victim_pfn].valid = 1;
//    rlt[victim_pfn].vpn = vpn;
//    rlt[victim_pfn].task_struct = current_process;
//
//    current_pagetable[vpn].pfn = victim_pfn;
//    current_pagetable[vpn].valid = 1;
//    current_pagetable[vpn].frequency++;

    if (rw == WRITE) {
        pte->dirty = 1;
    }

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
    for (uint64_t i = 0; i < (1 << rlt_size); i++) {
        if (rlt[i].valid == 0) {
            return i;
        } else if (rlt[i].task_struct->pagetable[rlt[i].vpn].frequency
                   < rlt[victim_pfn].task_struct->pagetable[rlt[victim_pfn].vpn].frequency) {
            victim_pfn = i;
        }
    }
//
//    int minIndex = 0;
//    for (int i = 1; i < (1 << rlt_size); i++) {
////    for (int i = 1; i < (rlt_size); i++) {
//        if (rlt[i].task_struct->pagetable[rlt[i].vpn].frequency
//            < rlt[minIndex].task_struct->pagetable[rlt[minIndex].vpn].frequency) {
//            minIndex = i;
//        }
//    }
//
//    rlt[minIndex].task_struct->pagetable->valid = 0;
//    if (rlt[minIndex].task_struct->pagetable->dirty == 1) {
//        stats->writes_to_disk++;
//    }
//
//    victim_pfn = (uint64_t) minIndex;

    pte_t* pte = rlt[victim_pfn].task_struct->pagetable + rlt[victim_pfn].vpn;
    pte->valid = 0;
    if (pte->dirty) {
        stats->writes_to_disk++;
        pte->dirty = 0;
    }



    // If the victim page table entry belongs to the current process, there is a chance
    // that it also resides in the TLB. So to avoid fake valid mappings from the TLB,
    // we clear that particular TLB entry.
    /******************* DO NOT MODIFY CODE BELOW THIS LINE **************/
    if (rlt[victim_pfn].task_struct == current_process) {
        tlb_clearOne(rlt[victim_pfn].vpn);
    }
    return victim_pfn;
}
