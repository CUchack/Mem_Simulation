#ifndef SIMULATOR_H_INCLUDED
#define SIMULATOR_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <strings.h>
#include <limits.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
// Always use a 64-bit variable to hold memory addresses
typedef unsigned long long int mem_addr_t;

#define ADDR_LEN        48


// a struct that groups cache parameters together
//typedef struct
//{
//   int s;                       /* 2**s cache sets */
//   int b;                       /* cacheline block size 2**b bytes */
//   int E;                       /* number of cachelines per set */
//   int S;                       /* number of sets, derived from S = 2**s */
//   int B;                       /* cacheline block size (bytes), derived from B = 2**b */
//} cache_param_t;

// A struct that defines the L1 cache parameters
typedef struct
{
    unsigned short block_size;
    unsigned int cache_size;
    unsigned short assoc;
    unsigned short hit_time;
    unsigned short miss_time;
    unsigned short bus_width;
    unsigned int numRowsL1;
    unsigned int numOffsetL1;
    unsigned int numIndexL1;

} L1_params;

//A struct that defines the L2 cache parameters
typedef struct
{
    unsigned short block_size;
    unsigned int cache_size;
    unsigned short assoc;
    unsigned short hit_time;
    unsigned short miss_time;
    unsigned short transfer_time;
    unsigned short bus_width;
    unsigned int numRowsL2;
    unsigned int numOffsetL2;
    unsigned int numIndexL2;
}L2_params;

//A struct to define main memory parameters
typedef struct
{
    unsigned short sendAddr;
    unsigned short ready;
    unsigned short chunkTime;
    unsigned short chunkSize;
}main_mem;

//Struct to define what the memory parameters are
typedef struct
{
    L1_params L1;
    L2_params L2;
    main_mem mmem;
    char setName[15];
}mem_params;

//Struct that will hold the tracefile parameters
typedef struct
{
    char refType;
    mem_addr_t address;
    int numBytes;
    unsigned int L1_index;
    unsigned int L2_index;
    unsigned int L1_offset;
    unsigned int L2_offset;
    mem_addr_t L1_tag;
    mem_addr_t L2_tag;

}traceData;

//Structure for a block
typedef struct
{
    bool dirty;
    mem_addr_t tag;
    int timestamp;
} block_st;
//Structure for a cache set; a pointer to an array of blocks
typedef struct
{
    bool valid;
    block_st *col;
}cache_set;

//Structure for a cache; a pointer to an array of sets
typedef struct
{
    unsigned short assoc;
    cache_set *row;
} cache_t;

typedef struct
{
    cache_t L1_I;
    cache_t L1_D;
    cache_t L2;
} cache_sys;

//Structure that keeps track of the statistical variables for the simulation run
typedef struct
{
	long total_hit;
	long flushes;
	long L1i_hit;
	long L1d_hit;
	long L2_hit;
	long total_miss;
	long L1i_miss;
	long L1d_miss;
	long L2_miss;
	long total_kickouts;
	long L1i_kickouts;
	long L1d_kickouts;
	long L2_kickouts;
	long total_dirty_kickouts;
	long L1i_dirty_kickouts;
	long L1d_dirty_kickouts;
	long L2_dirty_kickouts;
	long L1i_transfers;
	long L1d_transfers;
	long L2_transfers;
    long long exec_time;
    long instruction_references;
    long number_reads;
    long number_writes;
    long flush_time;
    long long read_cycles;
    long long write_cycles;
    long long instruction_cycles;
    long num_
} cacheVals;

/**********************************************************************
Simulate function.
***********************************************************************/
void simulate(mem_params params);
void checkCache(cache_sys *caches, mem_params params, traceData trace);
void readCache(cache_t *l1, cache_t *l2, traceData trace);
void readCache(cache_t *l1, cache_t *l2, traceData trace);

void init_sim(mem_params *params);

void printResultsToFile(cache_t *l1i, cache_t *l1d, cache_t *l2,mem_params params);
//unsigned int getIndex(mem_addr_t address, int indexSize, int offset);
//unsigned long getTag(mem_addr_t address, int tagSize, int offset);


#endif // SIMULATOR_H_INCLUDED
