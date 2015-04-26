#include "simulator.h"



///*****************************************************/
//This function returns the index from a given address
//******************************************************/
//unsigned int getIndex(mem_addr_t address, int indexSize, int offset){
//
//}
//
///*****************************************************/
//This function returns the tag from a given address
//*****************************************************/
//unsigned long getTag(mem_addr_t address, int tagSize, int offset){
//
//}

cacheVals vals;
extern char *config_file, *trace_file;

/*****************************************
This function initializes the tracking variables for an iteration of the
simulation.
*******************************************/
void init_sim(mem_params *params){

 	vals.total_hit = 0;
	vals.L1i_hit = 0;
	vals.L1d_hit = 0;
	vals.L2_hit = 0;
	vals.total_miss = 0;
	vals.L1i_miss = 0;
	vals.L1d_miss = 0;
	vals.L2_miss = 0;
	vals.total_kickouts = 0;
	vals.L1i_kickouts = 0;
	vals.L1d_kickouts = 0;
	vals.L2_kickouts = 0;
	vals.L1i_dirty_kickouts = 0;
	vals.L1d_dirty_kickouts = 0;
	vals.L2_dirty_kickouts = 0;
	vals.total_dirty_kickouts = 0;
	vals.transfers = 0;
    vals.exec_time = 0;
    vals.instruction_references = 0;
    vals.number_reads = 0;
    vals.number_writes = 0;
}

/****************************************************************
This function runs the simulation for a given input file and set of
parameters.  The result will change depending on the cache sizes,
the data passed in and the associativity of each of the memory levels
The simulation writes log files which track the execution of the simulation.
After the simulation is complete, the function also calls printResultsToFile()
which prints the output of the simulation to a file named after the
trace file and the memory configuration.
****************************************************************/

void simulate (mem_params params){
    rewind(stdin);

    //Create and open log file for session
    static char log[30];
    strcpy(log,"log.");
    strcat(log, trace_file);
    strcat(log, ".");
    strcat(log, params.setName);
    strcat(log, ".txt");
    FILE *logfile = fopen(log,"w");


    fprintf(logfile, "Set name: %s\n", &params.setName);
    fprintf(logfile, "L1 Parameters:\nCache size: %d \nAssociativity: %hu\n", params.L1.cache_size,params.L1.assoc);
    fprintf(logfile, "L2 Parameters:\nCache size: %d \nAssociativity: %hu\n", params.L2.cache_size,params.L2.assoc);

//    mem_addr_t address;
//    unsigned int bytesize;
////    unsigned long long Tag;
//    char op;
    traceData Trace;
    unsigned int tagSize_L1, tagSize_L2;//addrSize, tag, offset,
//    unsigned int L1_index, L2_index, L1_offset, L2_offset;
//    mem_addr_t L1_tag, L2_tag;

    unsigned long long TIMESTAMP = 0;

           /* TODO: Initialize a cache */
    //calculate number of rows, index bits, and offset bits for each cache level
    unsigned int numRowsL1 = params.L1.cache_size/(params.L1.assoc* params.L1.block_size);
    unsigned int numOffsetL1 = floor(log2(params.L1.block_size));
    unsigned int numIndexL1 = floor(log2(numRowsL1));
    unsigned int numRowsL2 = params.L2.cache_size/(params.L2.assoc* params.L2.block_size);
    unsigned int numOffsetL2 = floor(log2(params.L2.block_size));
    unsigned int numIndexL2 = floor(log2(numRowsL2));

    fprintf(logfile, "Number of L1 offset bits: %d\n", numOffsetL1);
    fprintf(logfile, "number of L1 rows: %d\n", numRowsL1);
    fprintf(logfile, "Number of L2 offset bits: %d\n", numOffsetL2);
    fprintf(logfile, "number of L2 rows: %d\n", numRowsL2);
    tagSize_L1 = ADDR_LEN-numIndexL1-numOffsetL1;                   //find the number of bits used to represent a tag
    tagSize_L2 = ADDR_LEN-numIndexL2-numOffsetL2;                   //find the number of bits used to represent a tag
    fprintf(logfile, "Number of L1 tag bits: %d\n", tagSize_L1);
    fprintf(logfile, "Number of L2 tag bits: %d\n", tagSize_L2);
    fprintf(logfile, "Number of L1 Index bits: %d\n", numIndexL1);
    fprintf(logfile, "Number of L2 Index bits: %d\n", numIndexL2);
    init_sim(&params);



   //allocate space for sets and for lines

   cache_sys cache;

   cache.L1_D.sets = malloc( params.L1.assoc * sizeof(cache_set));
   cache.L1_I.sets = malloc( params.L1.assoc * sizeof(cache_set));
   cache.L2.sets = malloc( params.L2.assoc * sizeof(cache_set));


   for ( int i = 0; i < params.L1.assoc; i++ )
   {
      cache.L1_D.sets[i].numRows = malloc( sizeof ( block_st ) * numRowsL1 );
      cache.L1_I.sets[i].numRows = malloc( sizeof ( block_st ) * numRowsL1 );
      cache.L2.sets[i].numRows = malloc( sizeof ( block_st ) * numRowsL2 );
   }

    while (scanf("%c %llx %d\n",&Trace.refType,&Trace.address,&Trace.numBytes) == 3)
    {
        fprintf(logfile, "Reference type: %c\nAddress: %llx\nNumber of Bytes: %d\n", Trace.refType, Trace.address, Trace.numBytes);
        //calculate index values for caches
        Trace.L1_index = ((Trace.address << (tagSize_L1 + 16)) >> (tagSize_L1 + 16 + numOffsetL1));
        fprintf(logfile, "extracted L1 index: %x\n", Trace.L1_index);
        Trace.L2_index = ((Trace.address << (tagSize_L2 + 16)) >> (tagSize_L2 + 16 + numOffsetL2));
        fprintf(logfile, "extracted L2 index: %x\n", Trace.L2_index);

        Trace.L1_offset = ((Trace.address << (tagSize_L1 + 16 + numIndexL1)) >> (tagSize_L1 + 16 + numIndexL1));
        fprintf(logfile, "L1 offset: %d \n", Trace.L1_offset);
        Trace.L2_offset = ((Trace.address << (tagSize_L2 + 16 + numIndexL2)) >> (tagSize_L2 + 16 + numIndexL2));
        fprintf(logfile, "L2 offset: %d \n", Trace.L2_offset);
       //calculate tag values for caches
        Trace.L1_tag = Trace.address >> (numOffsetL1 + numIndexL1);
        Trace.L2_tag = Trace.address >> (numOffsetL2 + numIndexL2);
        fprintf(logfile, "extracted L1 tag: %llx\n", Trace.L1_tag);
        fprintf(logfile, "extracted L2 tag: %llx\n", Trace.L2_tag);
        checkCache(&cache, params, Trace);



    }
    fclose(logfile);
    printResultsToFile(params);
}

/***************************************************************************
This function checks the cache levels for a hit, if it's not there it checks
the next level until it gets to main memory.  It also tracks the statistics
for the main result printout
***************************************************************************/
void checkCache(cache_sys *cache, mem_params params, traceData trace){
    //
        switch (trace.refType){
        case 'I':

            break;
        case 'R':
            vals.number_reads++;
            break;
        case 'W':
            vals.number_writes++;
            break;
        }

}

void readCache(cache_t *cache, mem_params params, traceData trace) {
    vals.instruction_references++;
        if (cache.L1_I.sets[L1_Dtrace.L1_index].valid) {
            if (params.L1.assoc == 1) {
                if (cache.L1_D.sets[L1_Dtrace.L1_index].numRows[1].tag == trace.L1_tag) {
                        // cache hit
                }
                else {
                    if (cache.L1_D.sets[L1_Dtrace.L1_index].numRows[1].dirty) {
                        //write tag back
                    }
                }
            }
            else {

            }
        }
        else {
            // Cache Miss
        }
}

void writeCache(cache_t *cache, mem_params params, traceData trace) {

}


/**************************************************************************
This function prints the simulation results to a file
***************************************************************************/
void printResultsToFile(mem_params params){
    float percent;
    static char res[30];
    strcpy(res,"output.");
    strcat(res, trace_file);
    strcat(res, ".");
    strcat(res, params.setName);
    strcat(res, ".txt");
    FILE *results = fopen(res,"w");
    fprintf(results, "---------------------------------------------------------\n");
    fprintf(results, "      %s.%s     Simulation Results\n", trace_file, params.setName);
    fprintf(results, "---------------------------------------------------------\n\n");
    fprintf(results, "Memory System:\n");
    fprintf(results, "  L1 Dcache size = %d : ways = %d : block size = %d\n", params.L1.cache_size, params.L1.assoc, params.L1.block_size);
    fprintf(results, "  L1 Icache size = %d : ways = %d : block size = %d\n", params.L1.cache_size, params.L1.assoc, params.L1.block_size);
    fprintf(results, "  L2 Cache size = %d : ways = %d : block size = %d\n", params.L2.cache_size, params.L2.assoc, params.L2.block_size);
    fprintf(results, "  Memory ready time  = %d : chunksize = %d : chunktime = %d\n\n", params.mmem.ready, params.mmem.chunkSize, params.mmem.chunkTime);
    fprintf(results, "Execute time =    %llu;    Total refs = %llu\n", vals.exec_time, (vals.instruction_references + vals.number_reads + vals.number_writes));
    fprintf(results, "Flush time =      %lu\n", vals.flush_time);
    fprintf(results, "Instruction references = %lu;    Data references = %lu\n\n", vals.instruction_references, (vals.number_reads + vals.number_writes));
    fprintf(results, "Number of reference types:     [Percentage]\n");
    percent = (float)(vals.number_reads*100/(vals.instruction_references + vals.number_reads + vals.number_writes));
    fprintf(results, "  Reads =             %lu     [%f]\n", vals.number_reads, percent);
    percent = (float)(vals.number_writes*100/(vals.instruction_references + vals.number_reads + vals.number_writes));
    fprintf(results, "  Writes =             %lu     [%f]\n", vals.number_writes, percent);
    percent = (float)(vals.instruction_references*100/(vals.instruction_references + vals.number_reads + vals.number_writes));
    fprintf(results, "  Inst. =             %lu     [%f]\n", vals.instruction_references, percent);

    fclose(results);
}
