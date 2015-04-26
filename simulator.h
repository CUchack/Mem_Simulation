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
    int valid;
    mem_addr_t tag;
    mem_addr_t offset;
    mem_addr_t index;
    int timestamp;
} block_st;
//Structure for a cache set; a pointer to an array of blocks
typedef struct
{
    block_st *numRows;
}cache_set;

//Structure for a cache; a pointer to an array of sets
typedef struct
{
    cache_set *sets;
} cache_t;


//Structure that keeps track of the statistical variables for the simulation run
typedef struct
{
	int total_hit;
	int L1i_hit;
	int L1d_hit;
	int L2_hit;
	int total_miss;
	int L1i_miss;
	int L1d_miss;
	int L2_miss;
	int total_kickouts;
	int L1i_kickouts;
	int L1d_kickouts;
	int L2_kickouts;
	int total_dirty_kickouts;
	int L1i_dirty_kickouts;
	int L1d_dirty_kickouts;
	int L2_dirty_kickouts;
	int transfers;
    long long exec_time;
    long instruction_references;
    long number_reads;
    long number_writes;
    long flush_time;
} cacheVals;

/**********************************************************************
Simulate function.
***********************************************************************/
void simulate(mem_params params);
void checkCache(mem_params params, traceData trace);

void init_sim(mem_params *params);

void printResultsToFile(mem_params params);
//unsigned int getIndex(mem_addr_t address, int indexSize, int offset);
//unsigned long getTag(mem_addr_t address, int tagSize, int offset);


#endif // SIMULATOR_H_INCLUDED
