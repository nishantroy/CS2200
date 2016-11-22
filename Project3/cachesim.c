#include "cachesim.h"

#define TRUE 1
#define FALSE 0

/**
 * The stuct that you may use to store the metadata for each block in the L1 and L2 caches
 */
typedef struct block_t {
    uint64_t tag; // The tag stored in that block
    uint8_t valid; // Valid bit
    uint8_t dirty; // Dirty bit
    int count;

    /**************** TODO ******************/

    /*
        Add another variable here to perform the LRU replacement. Look into using a counter
        variable that will keep track of the oldest block in a set
    */


} block;
typedef block* SetType;
typedef SetType* CacheType;

/**
 * A struct for storing the configuration of both the L1 and L2 caches as passed in the
 * cache_init function. All values represent number of bits. You may add any parameters
 * here, however I strongly suggest not removing anything from the config struct
 */
typedef struct config_t {
    uint64_t C1; /* Size of cache L1 */
    uint64_t C2; /* Size of cache L2 */
    uint64_t S; /* Set associativity of L2 */
    uint64_t B; /* Block size of both caches */
} config;


/****** Do not modify the below function headers ******/
static uint64_t get_tag(uint64_t address, uint64_t C, uint64_t B, uint64_t S);
static uint64_t get_index(uint64_t address, uint64_t C, uint64_t B, uint64_t S);
static uint64_t convert_tag(uint64_t tag, uint64_t index, uint64_t C1, uint64_t C2, uint64_t B, uint64_t S);
static uint64_t convert_index(uint64_t tag, uint64_t index, uint64_t C1, uint64_t C2, uint64_t B, uint64_t S);
static uint64_t convert_tag_l1(uint64_t l2_tag, uint64_t l2_index, uint64_t C1, uint64_t C2, uint64_t B, uint64_t S);
static uint64_t convert_index_l1(uint64_t l2_tag, uint64_t l2_index, uint64_t C1, uint64_t C2, uint64_t B, uint64_t S);

/****** You may add Globals and other function headers that you may need below this line ******/

config cache;
SetType L1;
CacheType L2;
int counter;


/**
 * Subroutine for initializing your cache with the passed in arguments.
 * You may initialize any globals you might need in this subroutine
 *
 * @param C1 The total size of your L1 cache is 2^C1 bytes
 * @param C2 The tatal size of your L2 cache is 2^C2 bytes
 * @param S The total number of blocks in a line/set of your L2 cache are 2^S
 * @param B The size of your blocks is 2^B bytes
 */
void cache_init(uint64_t C1, uint64_t C2, uint64_t S, uint64_t B)
{
    /*
        Initialize the caches here. I strongly suggest using arrays for representing
        meta data stored in the caches. The block_t struct given above maybe useful
    */

    /**************** TODO ******************/
    cache.C1 = C1;
    cache.C2 = C2;
    cache.S = S;
    cache.B = B;
    counter = 1;


    L1 = (block*) calloc((1 << C1)/(1 << B),sizeof(block));
    L2 = (block**) calloc((1 << C2)/(1 << B)/(1 << S),sizeof(block*));

    for (int i = 0; i < (1 << cache.S); i++) {
        L2[i] = calloc((1 << S), sizeof(block));
    }
}


/**
 * Subroutine that simulates one cache event at a time.
 * @param rw The type of access, READ or WRITE
 * @param address The address that is being accessed
 * @param stats The struct that you are supposed to store the stats in
 */
void cache_access (char rw, uint64_t address, struct cache_stats_t *stats)
{
    /*
        Suggested approach:
            -> Find the L1 tag and index of the address that is being passed in to the function
            -> Check if there is a hit in the L1 cache
            -> If L1 misses, check the L2 cache for a hit (Hint: If L2 hits, update L1 with new values)
            -> If L2 misses, need to get values from memory, and update L2 and L1 caches

            * We will leave it upto you to decide what must be updated and when
     */

    /**************** TODO ******************/
    uint64_t tag1 = get_tag(address, cache.C1, cache.B, 0);
    uint64_t index1 = get_index(address, cache.C1, cache.B, 0);
    uint64_t tag2 = convert_tag(tag1, index1, cache.C1, cache.C2, cache.B, cache.S);
    uint64_t index2 = convert_index(tag1, index1, cache.C1, cache.C2, cache.B, cache.S);

    stats -> accesses++;
    counter++;
    //L1 hit
    if (L1[index1].tag == tag1 && L1[index1].valid == TRUE) {
        if (rw =='w') {
            L1[index1].dirty = 1;
            stats -> writes++;
        } else {
            stats -> reads++;
        }

        block* setL2 = L2[index2];
        for (int i = 0; i < (1<<cache.S); i++) {
            if (setL2[i].tag == tag2) {
                setL2[i].count = counter;
                break;
            }
        }
        return;
    }

    //L2 Hit L1 Miss
    uint64_t L2Hit = 0;
    for(uint64_t i = 0; i < (1<<cache.S); i++) {
        if((L2[index2][i]).tag == tag2 && (L2[index2][i]).valid == TRUE) {
            L2Hit = 1;
            stats -> misses++;
            if (rw == 'w') {
                L2[index2][i].dirty = 1;
                stats -> writes++;
                stats -> write_misses++;
                stats -> l1_write_misses++;
            } else {
                stats -> reads++;
                stats -> read_misses++;
                stats -> l1_read_misses++;
            }
        }
    }
    if (L2Hit) {
        //Evict L1
        if (L1[index1].dirty == TRUE && L1[index1].valid == TRUE) {
            uint64_t eTag1 = convert_tag(L1[index1].tag, index1, cache.C1, cache.C2, cache.B, cache.S);
            block* setL2 = L2[index2];
            for (int i = 0; i < (1<<cache.S); i++) {
                if (setL2[i].tag == eTag1) {
                    setL2[i].dirty = 1;
                    setL2[i].valid  = 1;
                    break;
                }
            }
        }

        L1[index1].tag = convert_tag_l1(tag2, index2, cache.C1, cache.C2, cache.B, cache.S);
        L1[index1].valid = 1;
        L1[index1].dirty = 0;

    } else {
        //Miss/Miss
        int wroteBack = 0;
        stats -> misses++;
        if (rw == 'w') {
            stats -> writes++;
            stats -> write_misses++;
            stats -> l1_write_misses++;
            stats -> l2_write_misses++;
        } else {
            stats -> reads++;
            stats -> read_misses++;
            stats -> l1_read_misses++;
            stats -> l2_read_misses++;
        }
        block* setL2 = L2[index2];
        block LRUL2 = setL2[0];

        for (int i = 0; i < (1<<cache.S); i++) {
            if(setL2[i].valid == FALSE) {
                //update stats
                setL2[i].valid = 1;
                setL2[i].dirty = (rw == 'w');
                setL2[i].count = counter;
                LRUL2 = setL2[i];
                return;
            }
            if (setL2[i].count < LRUL2.count) {
                LRUL2 = setL2[i];
                if (i == ((1 << cache.S) - 1)) {
                    setL2[i].valid = 0;
                    stats -> write_backs++;
                    wroteBack = 1;
                }
            }
        }


        uint64_t newL1Tag = convert_tag_l1(LRUL2.tag, index2, cache.C1, cache.C2, cache.B, cache.S);
        uint64_t newL1Index = convert_index_l1(LRUL2.tag, index2, cache.C1, cache.C2, cache.B, cache.S);

        if ((L1[newL1Index].tag == LRUL2.tag) && L1[newL1Index].valid == TRUE) {
            L1[newL1Index].valid = FALSE;
            L1[newL1Index].tag = newL1Tag;
            if (L1[newL1Index].dirty == TRUE) {
                if (wroteBack != 1) {
                    stats -> write_backs++;
                }
            }
        }
        for (int i = 0; i < (1<<cache.S); i++) {
            if(LRUL2.tag == L2[index2][i].tag) {
                L2[index2][i].valid = 1;
                L2[index2][i].dirty = 0;
                L2[index2][i].count = counter;

            }
        }

        stats -> l1_miss_rate = (double) (((stats -> l1_read_misses) + (stats -> l1_write_misses)) / (stats -> accesses);
        stats -> l2_miss_rate = (double) (((stats -> l2_read_misses) + (stats -> l2_write_misses)) / (stats -> accesses);
        stats -> miss_rate = (double) (((stats -> read_misses) + (stats -> write_misses)) / (stats -> accesses);


    }



}



/**
 * Subroutine for freeing up memory, and performing any final calculations before the statistics
 * are outputed by the driver
 */
void cache_cleanup (struct cache_stats_t *stats)
{
    /*
        Make sure to free up all the memory you malloc'ed here. To check if you have freed up the
        the memory, run valgrind. For more information, google how to use valgrind.
    */

    /**************** TODO ******************/
    stats -> accesses = 0;
    stats -> reads = 0;
    stats -> read_misses = 0;
    stats -> writes = 0;
    stats -> write_misses = 0;
    stats -> misses = 0;
    stats -> write_backs = 0;

    stats -> l1_read_misses = 0;
    stats -> l1_write_misses = 0;

    stats -> l2_read_misses = 0;
    stats -> l2_write_misses = 0;

    stats -> l1_access_time = 0;
    stats -> l2_access_time = 0;
    stats -> memory_access_time = 0;
    free(L1);
    free(L2);
}

/**
 * Subroutine to compute the Tag of a given address based on the parameters passed in
 *
 * @param address The address whose tag is to be computed
 * @param C The size of the cache in bits (i.e. Size of cache is 2^C)
 * @param B The size of the cache block in bits (i.e. Size of block is 2^B)
 * @param S The set associativity of the cache in bits (i.e. Set-Associativity is 2^S)
 *
 * @return The computed tag
 */
static uint64_t get_tag(uint64_t address, uint64_t C, uint64_t B, uint64_t S)
{
    /**************** TODO ******************/
    uint64_t offset = B;
    uint64_t index = C - (B + S);
    uint64_t tag = address >> (offset + index);
    return tag;
}

/**
 * Subroutine to compute the Index of a given address based on the parameters passed in
 *
 * @param address The address whose tag is to be computed
 * @param C The size of the cache in bits (i.e. Size of cache is 2^C)
 * @param B The size of the cache block in bits (i.e. Size of block is 2^B)
 * @param S The set associativity of the cache in bits (i.e. Set-Associativity is 2^S)
 *
 * @return The computed index
 */
static uint64_t get_index(uint64_t address, uint64_t C, uint64_t B, uint64_t S)
{
    /**************** TODO ******************/
    uint64_t index = (unsigned) address >> B;
    return (uint64_t) (index && (1 << (C - B - S)));
    // uint64_t offset = B;
    // uint64_t index = C - (B + S);
    // uint64_t add = index + offset;
    // uint64_t tagMask = address & ((1 << add) - 1);
    // index = tagMask >> offset;
    // return index;
}


/**** DO NOT MODIFY CODE BELOW THIS LINE UNLESS YOU ARE ABSOLUTELY SURE OF WHAT YOU ARE DOING ****/

/*
    Note:   The below functions will be useful in converting the L1 tag and index into corresponding L2
            tag and index. These should be used when you are evicitng a block from the L1 cache, and
            you need to update the block in L2 cache that corresponds to the evicted block.

            The newly added functions will be useful for converting L2 indecies ang tags into the corresponding
            L1 index and tags. Make sure to understand how they are working.
*/

/**
 * This function converts the tag stored in an L1 block and the index of that L1 block into corresponding
 * tag of the L2 block
 *
 * @param tag The tag that needs to be converted (i.e. L1 tag)
 * @param index The index of the L1 cache (i.e. The index from which the tag was found)
 * @param C1 The size of the L1 cache in bits
 * @param C2 The size of the l2 cache in bits
 * @param B The size of the block in bits
 * @param S The set associativity of the L2 cache
 */
static uint64_t convert_tag(uint64_t tag, uint64_t index, uint64_t C1, uint64_t C2, uint64_t B, uint64_t S)
{
    uint64_t reconstructed_address = (tag << (C1 - B)) | index;
    return reconstructed_address >> (C2 - B - S);
}

/**
 * This function converts the tag stored in an L1 block and the index of that L1 block into corresponding
 * index of the L2 block
 *
 * @param tag The tag stored in the L1 index
 * @param index The index of the L1 cache (i.e. The index from which the tag was found)
 * @param C1 The size of the L1 cache in bits
 * @param C2 The size of the l2 cache in bits
 * @param B The size of the block in bits
 * @param S The set associativity of the L2 cache
 */
static uint64_t convert_index(uint64_t tag, uint64_t index, uint64_t C1, uint64_t C2, uint64_t B,  uint64_t S)
{
    // Reconstructed address without the block offset bits
    uint64_t reconstructed_address = (tag << (C1 - B)) | index;
    // Create index mask for L2 without including the block offset bits
    return reconstructed_address & ((1 << (C2 - S - B)) - 1);
}

/**
 * This function converts the tag stored in an L2 block and the index of that L2 block into corresponding
 * tag of the L1 cache
 *
 * @param l2_tag The L2 tag
 * @param l2_index The index of the L2 block
 * @param C1 The size of the L1 cache in bits
 * @param C2 The size of the l2 cache in bits
 * @param B The size of the block in bits
 * @param S The set associativity of the L2 cache
 * @return The L1 tag linked to the L2 index and tag
 */
static uint64_t convert_tag_l1(uint64_t l2_tag, uint64_t l2_index, uint64_t C1, uint64_t C2, uint64_t B, uint64_t S) {
    uint64_t reconstructed_address = (l2_tag << (C2 - B - S)) | l2_index;
    return reconstructed_address >> (C1 - B);
}

/**
 * This function converts the tag stored in an L2 block and the index of that L2 block into corresponding
 * index of the L1 block
 *
 * @param l2_tag The L2 tag
 * @param l2_index The index of the L2 block
 * @param C1 The size of the L1 cache in bits
 * @param C2 The size of the l2 cache in bits
 * @param B The size of the block in bits
 * @param S The set associativity of the L2 cache
 * @return The L1 index of the L2 block
 */
static uint64_t convert_index_l1(uint64_t l2_tag, uint64_t l2_index, uint64_t C1, uint64_t C2, uint64_t B, uint64_t S) {
    uint64_t reconstructed_address = (l2_tag << (C2 - B - S)) | l2_index;
    return reconstructed_address & ((1 << (C1 - B)) - 1);
}
