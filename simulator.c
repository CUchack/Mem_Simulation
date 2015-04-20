#include "simulator.h"


void simulate (mem_params params){
    rewind(stdin);

    printf("L1 Parameters:\nCache size: %d \nAssociativity: %hu\n", params.L1.cache_size,params.L1.assoc);
    printf("L2 Parameters:\nCache size: %d \nAssociativity: %hu\n", params.L2.cache_size,params.L2.assoc);
    mem_addr_t address;
    unsigned int bytesize;
//    unsigned long long Tag;
    char op;

           /* TODO: Initialize a cache */

   //Structure for a block
   typedef struct
   {
      int valid;
      mem_addr_t tag;
      int timestamp;
   } block_st;

   //Structure for a set; a pointer to an array of blocks
   typedef struct
   {
      block_st *lines;
   } cache_set;

   //Structure for a cache; a pointer to an array of sets
   typedef struct
   {
      cache_set *sets;
   } cache_t;

   //allocate space for sets and for lines
//   cache_t cache;

//   cache.sets = malloc( par.S * sizeof ( cache_set ) );
//   for ( int i = 0; i < par.S; i++ )
//   {
//      cache.sets[i].lines = malloc( sizeof ( line_st ) * par.E );
//   }
        while (scanf("%c %Lx %d\n",&op,&address,&bytesize) == 3) {
                printf("Reference type: %c\nAddress: %Lx\nNumber of Bytes: %d\n", op, address, bytesize);
        }

}
