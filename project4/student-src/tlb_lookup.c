#include "../simulator-src/tlb.h"
#include "../simulator-src/pagetable.h"
#include <assert.h>

/**
 * This function simulates a the TLB lookup, and uses the second chance algorithm
 * to evict an entry
 *
 * @param vpn The virtual page number that has to be translated
 * @param offset The page offset of the virtual address
 * @param rw Specifies if the access is a read or a write
 * @param stats The struct for statistics
 */
uint64_t tlb_lookup(uint64_t vpn, uint64_t offset, char rw, stats_t *stats) {

    // (1) Look for the pfn in the TLB.
    // If you find it here
    // (2) update the frequency count of the page table entry using the
    //     current_pagetable global.
    // (3) Mark the TLB entry as used - for clock sweep
    // (4) Make sure to mark the entry dirty in the TLB if it is a write access
    // (5) return the PFN you just found in the TLB


    /********* TODO ************/
    stats->accesses++;
    if (rw == READ) {
        stats->reads++;
    } else {
        stats->writes++;
    }


    for (int i = 0; i < (1 << tlb_size); i++) {
        if (tlb[i].valid == 1 && tlb[i].vpn == vpn) {
            //printf("FOUND HIT, Using TLB index: %d \n", i);
            current_pagetable[vpn].frequency++;
            tlb[i].used = 1;
            if (rw == WRITE) {
                tlb[i].dirty = 1;
                current_pagetable[vpn].dirty = 1;
            }
            return (tlb[i].pfn << page_size) | offset;
        }
    }

    // The below function is called if it is a TLB miss
    /* DO NOT MODIFY */
    uint64_t pfn = page_lookup(vpn, offset, rw, stats);
    /*****************/

    stats->translation_faults++;

    int flag = 0;

    for (int i = 0; i < (1 << tlb_size); i++) {
        if (tlb[i].valid == 0) {
            //Found invalid entry
            // printf("INVALID, Using TLB index: %d", i);
            flag = 1;
            tlb[i].vpn = vpn;
            tlb[i].pfn = pfn;
            tlb[i].valid = 1;
            tlb[i].used = 1;
            if (rw == WRITE) {
                tlb[i].dirty = 1;
                current_pagetable[vpn].dirty = 1;
            }
            break;
        }
    }

    if (flag == 0) {
        for (int i = 0; i < (1 << tlb_size); i++) {
            if (tlb[i].used == 1) {
                tlb[i].used = 0;
            } else {
                printf("CLKWRK, Using TLB index: %d \n", i);
                flag = 1;
                pfn = tlb[i].pfn;
                if (tlb[i].dirty == 1) {
                    stats->writes_to_disk++;
                    current_pagetable[vpn].dirty = 1;
                }
                if (rw == WRITE) {
                    tlb[i].dirty = 1;
                    current_pagetable[vpn].dirty = 1;
                }
                tlb[i].vpn = vpn;
                tlb[i].pfn = pfn;
                tlb[i].valid = 1;
                tlb[i].used = 1;
                break;
            }
        }

        if (flag == 0) {
            printf("NOTHING FOUND: Using TLB index 0 \n");
            tlb[0].vpn = vpn;
            tlb[0].pfn = pfn;
            tlb[0].valid = 1;
            tlb[0].used = 1;
            if (tlb[0].dirty == 1) {
                stats->writes_to_disk++;
            }
            if (rw == WRITE) {
                tlb[0].dirty = 1;
                current_pagetable[vpn].dirty = 1;
            }
        }
    }



    // (1) Update the relevant stats
    // (2) Update the TLB with this new mapping
    //      (a) Find an invalid block in the TLB - use it
    //      If you cannot find an invalid block
    //      (i) Run the clock sweep algorithm to find a victim
    //      (ii) Update the current_pagetable at that VPN to dirty if
    //           the evicted TLB entry is dirty
    //      (b) Put the new mapping into the TLB - mark it used

    /**** THESE WILL HELP YOU MAKE SURE THAT YOUR CODE IS CORRECT *****/
    assert(current_pagetable[vpn].valid);
    assert(pfn == current_pagetable[vpn].pfn);
    /********* TODO ************/



    /******* TODO *************/
    // Make sure to return the entire address here, this is just a place holder
    return (pfn << page_size) | offset;
}
