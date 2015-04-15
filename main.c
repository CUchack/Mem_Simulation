#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <strings.h>
#include <limits.h>

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

//Initialize structure that holds default memory parameters
mem_params Default_params = {.L1.block_size = 32, .L1.cache_size = 8192, .L1.assoc = 1,
.L1.hit_time = 1, .L1.miss_time = 1, .L2.block_size = 64, .L2.cache_size = 32768,
.L2.assoc = 1, .L2.hit_time = 5, .L2.miss_time = 7, .L2.transfer_time = 5,
.L2.bus_width = 16, .mmem.sendAddr = 10, .mmem.ready = 30, .mmem.chunkTime = 15,
.mmem.chunkSize = 8};


int verbosity;

// printUsage - Print usage info
void printUsage( char *argv[] )
{
   printf( "Usage: %s [-f <file>] [-h]\n", argv[0] );
   printf( "Options:\n" );
   printf( "  -f <file>  Optional configuration file.\n" );
   printf( "  -h         Print this help message.\n" );
   exit( 0 );
}

void printSummary( int hit_count, int miss_count, int eviction_count )
{
   printf( "hits: %d   misses: %d   evictions: %d\n", hit_count, miss_count, eviction_count );
}



int main(int argc, char **argv)
{
    char *config_file;
    char c;
    while ( ( c = getopt( argc, argv, "f:h" ) ) != -1 )
        {
            switch ( c )
            {
            case 'f':
                config_file = optarg;
                printf("config file = %s\n", config_file);
                break;
            case 'h':
                printUsage(argv);
                exit(1);
                break;
            }
        }

//    printUsage(argv);
    return 0;
}
