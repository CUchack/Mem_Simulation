#ifndef SIMULATOR_H_INCLUDED
#define SIMULATOR_H_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <strings.h>
#include <limits.h>
#include <stdbool.h>

// Always use a 64-bit variable to hold memory addresses
typedef unsigned long long int mem_addr_t;


// a struct that groups cache parameters together
typedef struct
{
   int s;                       /* 2**s cache sets */
   int b;                       /* cacheline block size 2**b bytes */
   int E;                       /* number of cachelines per set */
   int S;                       /* number of sets, derived from S = 2**s */
   int B;                       /* cacheline block size (bytes), derived from B = 2**b */
} cache_param_t;

// A struct that defines the L1 cache parameters
typedef struct
{
    unsigned short block_size;
    unsigned int cache_size;
    unsigned short assoc;
    unsigned short hit_time;
    unsigned short miss_time;
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
}mem_params;


/**********************************************************************
Simulate function.
***********************************************************************/
void simulate (mem_params params);

#endif // SIMULATOR_H_INCLUDED
