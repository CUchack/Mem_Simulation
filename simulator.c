#include "simulator.h"

cacheVals vals;
extern char *config_file, *trace_file;

/*****************************************
This function initializes the tracking variables for an iteration of the
simulation.
*******************************************/
void init_sim(mem_params *params) {
   vals.total_hit = 0;
   vals.flushes = 0;
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
   vals.total_dirty_kickouts = 0;
   vals.L1i_dirty_kickouts = 0;
   vals.L1d_dirty_kickouts = 0;
   vals.L2_dirty_kickouts = 0;
   vals.L1i_transfers = 0;
   vals.L1d_transfers = 0;
   vals.L2_transfers = 0;
   vals.exec_time = 0;
   vals.instruction_references = 0;
   vals.number_reads = 0;
   vals.number_writes= 0;
   vals.flush_time= 0;
   vals.read_cycles= 0;
   vals.write_cycles= 0;
   vals.instruction_cycles= 0;
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

void simulate (mem_params params) {
   rewind(stdin);

   //Create and open log file for session
   static char log[30];
   strcpy(log,"log.");
   strcat(log, trace_file);
   strcat(log, ".");
   strcat(log, params.setName);
   strcat(log, ".txt");

   traceData Trace;
   unsigned int tagSize_L1, tagSize_L2;//addrSize, tag, offset,

   params.L1.numRowsL1 = params.L1.cache_size/(params.L1.assoc* params.L1.block_size);
   params.L1.numOffsetL1 = floor(log2(params.L1.block_size));
   params.L1.numIndexL1 = floor(log2(params.L1.numRowsL1));
   params.L2.numRowsL2 = params.L2.cache_size/(params.L2.assoc* params.L2.block_size);
   params.L2.numOffsetL2 = floor(log2(params.L2.block_size));
   params.L2.numIndexL2 = floor(log2(params.L2.numRowsL2));
   tagSize_L1 = ADDR_LEN-params.L1.numIndexL1-params.L1.numOffsetL1;                   //find the number of bits used to represent a tag
   tagSize_L2 = ADDR_LEN-params.L2.numIndexL2-params.L2.numOffsetL2;                   //find the number of bits used to represent a tag

#ifdef DEBUG
   FILE *logfile = fopen(log,"w");
   fprintf(logfile, "Set name: %s\n", &params.setName);
   fprintf(logfile, "L1 Parameters:\nCache size: %d \nAssociativity: %hu\n", params.L1.cache_size,params.L1.assoc);
   fprintf(logfile, "L2 Parameters:\nCache size: %d \nAssociativity: %hu\n", params.L2.cache_size,params.L2.assoc);
   fprintf(logfile, "Number of L1 offset bits: %d\n", params.L1.numOffsetL1);
   fprintf(logfile, "number of L1 rows: %d\n", params.L1.numRowsL1);
   fprintf(logfile, "Number of L2 offset bits: %d\n", params.L2.numOffsetL2);
   fprintf(logfile, "number of L2 rows: %d\n", params.L2.numRowsL2);
   fprintf(logfile, "Number of L1 tag bits: %d\n", tagSize_L1);
   fprintf(logfile, "Number of L2 tag bits: %d\n", tagSize_L2);
   fprintf(logfile, "Number of L1 Index bits: %d\n", params.L1.numIndexL1);
   fprintf(logfile, "Number of L2 Index bits: %d\n", params.L2.numIndexL2);
#endif // DEBUG

   init_sim(&params);

   //allocate space for cache
   cache_sys cache;
   cache.L1_D.assoc = params.L1.assoc;
   cache.L1_I.assoc = params.L1.assoc;
   cache.L2.assoc = params.L2.assoc;

   cache.L1_D.row = malloc( sizeof(cache_set) * params.L1.numRowsL1 );
   cache.L1_I.row = malloc( sizeof(cache_set) * params.L1.numRowsL1 );
   cache.L2.row = malloc(   sizeof(cache_set) * params.L2.numRowsL2 );

   for ( int i = 0; i < params.L1.numRowsL1; i++ ) {
      cache.L1_D.row[i].col = malloc( sizeof ( block_st ) * params.L1.assoc );
      cache.L1_I.row[i].col = malloc( sizeof ( block_st ) * params.L1.assoc );
      for (int j = 0; j < params.L1.assoc; j++) {
         cache.L1_D.row[i].col[j].tag = 0;
         cache.L1_I.row[i].col[j].tag = 0;
         cache.L1_D.row[i].col[j].valid = 0;
         cache.L1_I.row[i].col[j].valid = 0;
         cache.L1_D.row[i].col[j].dirty = 0;
         cache.L1_I.row[i].col[j].dirty = 0;
      }
   }
   for ( int i = 0; i < params.L2.numRowsL2; i++ ) {
      cache.L2.row[i].col = malloc( sizeof ( block_st ) * params.L2.assoc );
      for (int j = 0; j < params.L2.assoc; j++) {
         cache.L2.row[i].col[j].tag = 0;
         cache.L2.row[i].col[j].valid = 0;
         cache.L2.row[i].col[j].dirty = 0;
      }
   }

   while (scanf("%c %llx %d\n",&Trace.refType,&Trace.address,&Trace.numBytes) == 3) {

      //calculate index values for caches
      Trace.L1_index = ((Trace.address << (tagSize_L1 + 16)) >> (tagSize_L1 + 16 + params.L1.numOffsetL1));
      Trace.L2_index = ((Trace.address << (tagSize_L2 + 16)) >> (tagSize_L2 + 16 + params.L2.numOffsetL2));
      //calculate offset values for caches
      Trace.L1_offset = ((Trace.address << (tagSize_L1 + 16 + params.L1.numIndexL1)) >> (tagSize_L1 + 16 + params.L1.numIndexL1));
      Trace.L2_offset = ((Trace.address << (tagSize_L2 + 16 + params.L2.numIndexL2)) >> (tagSize_L2 + 16 + params.L2.numIndexL2));
      //calculate tag values for caches
      Trace.L1_tag = Trace.address >> (params.L1.numOffsetL1 + params.L1.numIndexL1);
      Trace.L2_tag = Trace.address >> (params.L2.numOffsetL2 + params.L2.numIndexL2);

#ifdef DEBUG
      fprintf(logfile, "Reference type: %c\nAddress: %llx\nNumber of Bytes: %d\n", Trace.refType, Trace.address, Trace.numBytes);
      fprintf(logfile, "extracted L1 index: %x\n", Trace.L1_index);
      fprintf(logfile, "extracted L2 index: %x\n", Trace.L2_index);
      fprintf(logfile, "L1 offset: %d \n", Trace.L1_offset);
      fprintf(logfile, "L2 offset: %d \n", Trace.L2_offset);
      fprintf(logfile, "extracted L1 tag: %llx\n", Trace.L1_tag);
      fprintf(logfile, "extracted L2 tag: %llx\n", Trace.L2_tag);
#endif // DEBUG

      checkCache(&cache, params, Trace);
   }
#ifdef DEBUG
   fclose(logfile);
#endif // DEBUG
   printResultsToFile(&cache.L1_I, &cache.L1_D, &cache.L2, params);
}

/***************************************************************************
This function checks the cache levels for a hit, if it's not there it checks
the next level until it gets to main memory.  It also tracks the statistics
for the main result printout
***************************************************************************/
void checkCache(cache_sys *caches, mem_params params, traceData trace) {
   switch (trace.refType) {
   case 'I':
      instReadCache(&caches->L1_I, &caches->L2, trace);
      vals.instruction_references++;
      break;
   case 'R':
      dataReadCache(&caches->L1_D, &caches->L2, trace);
      vals.number_reads++;
      break;
   case 'W':
      dataWriteCache(&caches->L1_D, &caches->L2, trace);
      vals.number_writes++;
      break;
   }
}

void instReadCache(cache_t *l1, cache_t *l2, traceData trace) {
   int reqsl1 = (int)ceil((double)trace.numBytes/4);
   if (trace.L1_offset%4 + trace.numBytes > 4*reqsl1) {
      reqsl1++;
   }
   int reqsl2 = (int)ceil((double)trace.numBytes/16);
   if (trace.L2_offset%16 + trace.numBytes > 16*reqsl2) {
      reqsl2++;
   }
   // seach l1 for tag
   for (int i = 0; i < l1->assoc; i++) {
      if ((l1->row[trace.L1_index].col[i].tag == trace.L1_tag) && (l1->row[trace.L1_index].col[i].valid)) {
         // l1 cache hit
         vals.L1i_hit+=reqsl1;
         lruUpdate(l1,trace.L1_index,i);
         return;
      }
   }
   // l1 cache miss, check l2
   vals.L1i_miss++;
   vals.L1i_transfers++;
   for (int i = 0; i < l2->assoc; i++) {
      // search l2 for tag
      if ((l2->row[trace.L2_index].col[i].tag == trace.L2_tag) && (l2->row[trace.L2_index].col[i].valid)) {
         // l2 cache hit, find block in l1 to evict
         vals.L2_hit+=reqsl2;
         vals.L1i_hit+=reqsl1-1;
         lruUpdate(l2,trace.L2_index,i);
         if (l1->row[trace.L1_index].col[0].valid) {
            // l1 valid, l1 kickout required
            if (l1->row[trace.L1_index].col[0].dirty) {
               // l1 dirty, l2 dirty kickout required
               l2->row[trace.L2_index].col[0].dirty = false;
               if (l2->row[trace.L2_index].col[0].valid) {
                  // l1 dirty, l2 valid, l2 kickout required
                  if (l2->row[trace.L2_index].col[0].dirty) {
                     // l1,l2 block dirty, l2 dirty kickout required
                     vals.L2_dirty_kickouts++;
                  }
                  vals.L2_kickouts++;
               }
               vals.L1i_dirty_kickouts++;
               l2->row[trace.L2_index].col[0].tag = l1->row[trace.L1_index].col[0].tag;
            }
            vals.L1i_kickouts++;
         }
         // write l2 hit to free LRU l1
         l1->row[trace.L1_index].col[0].tag = trace.L1_tag;
         l1->row[trace.L1_index].col[0].valid = true;
         l2->row[trace.L2_index].col[0].valid = true;
         l1->row[trace.L1_index].col[0].dirty = false;
         l2->row[trace.L2_index].col[l2->assoc-1].dirty = false; //lru update puts the hit tag to the back
         l2->row[trace.L2_index].col[l2->assoc-1].valid = true;
         return;
      }
   }
   if (l1->row[trace.L1_index].col[0].valid) {
      // l2 cache miss (l1 valid)
      if(l1->row[trace.L1_index].col[0].dirty) {
         // l2 cache miss (l1 valid and dirty)
         vals.L1d_dirty_kickouts++;
      }
      // instant dirty kickout l1 -> l2 -> mem
      vals.L1d_kickouts++;
      vals.L2_dirty_kickouts++;
      vals.L2_kickouts++;
   }
   // l2 cache miss
   vals.L2_miss++;
   vals.L1i_hit+=reqsl1-1;
   vals.L2_transfers++;
   l1->row[trace.L1_index].col[0].tag = trace.L1_tag;
   l2->row[trace.L2_index].col[0].tag = trace.L2_tag;
   l1->row[trace.L1_index].col[0].dirty = false;
   l2->row[trace.L2_index].col[0].dirty = false;
   l1->row[trace.L1_index].col[0].valid = true;
   l2->row[trace.L2_index].col[0].valid = true;
   lruUpdate(l1,trace.L1_index,0);
   lruUpdate(l2,trace.L2_index,0);
   return;
}

void dataReadCache(cache_t *l1, cache_t *l2, traceData trace) {
   int reqsl1 = (int)ceil((double)trace.numBytes/4);
   if (trace.L1_offset%4 + trace.numBytes > 4*reqsl1) {
      reqsl1++;
   }
   int reqsl2 = (int)ceil((double)trace.numBytes/16);
   if (trace.L2_offset%16 + trace.numBytes > 16*reqsl2) {
      reqsl2++;
   }
   // seach l1 for tag
   for (int i = 0; i < l1->assoc; i++) {
      if ((l1->row[trace.L1_index].col[i].tag == trace.L1_tag) && (l1->row[trace.L1_index].col[i].valid)) {
         // l1 cache hit
         vals.L1d_hit+=reqsl1;
         lruUpdate(l1,trace.L1_index,i);
         return;
      }
   }
   // l1 cache miss, check l2
   vals.L1d_miss++;
   vals.L1d_transfers++;
   for (int i = 0; i < l2->assoc; i++) {
      // search l2 for tag
      if ((l2->row[trace.L2_index].col[i].tag == trace.L2_tag) && (l2->row[trace.L2_index].col[i].valid)) {
         // l2 cache hit, find block in l1 to evict
         vals.L2_hit+=reqsl2;
         vals.L1d_hit+=reqsl1-1;
         lruUpdate(l2,trace.L2_index,i);
         if (l1->row[trace.L1_index].col[0].valid) {
            // l1 valid, l1 kickout required
            if (l1->row[trace.L1_index].col[0].dirty) {
               // l1 dirty, l2 dirty kickout required
               l2->row[trace.L2_index].col[0].dirty = false;
               if (l2->row[trace.L2_index].col[0].valid) {
                  // l1 dirty, l2 valid, l2 kickout required
                  if (l2->row[trace.L2_index].col[0].dirty) {
                     // l1,l2 block dirty, l2 dirty kickout required
                     vals.L2_dirty_kickouts++;
                  }
                  vals.L2_kickouts++;
               }
               vals.L1d_dirty_kickouts++;
               l2->row[trace.L2_index].col[0].tag = l1->row[trace.L1_index].col[0].tag;
            }
            vals.L1d_kickouts++;
         }
         // write l2 hit to free LRU l1
         l1->row[trace.L1_index].col[0].tag = trace.L1_tag;
         l1->row[trace.L1_index].col[0].valid = true;
         l2->row[trace.L2_index].col[0].valid = true;
         l1->row[trace.L1_index].col[0].dirty = false;
         l2->row[trace.L2_index].col[l2->assoc-1].dirty = false; //lru update puts the hit tag to the back
         l2->row[trace.L2_index].col[l2->assoc-1].valid = true;
         return;
      }
   }
   // l2 miss
   if (l1->row[trace.L1_index].col[0].valid) {
      // l2 cache miss (l1 valid)
      if(l1->row[trace.L1_index].col[0].dirty) {
         // l2 cache miss (l1 valid and dirty)
         vals.L1d_dirty_kickouts++;
      }
      // instant dirty kickout l1 -> l2 -> mem
      vals.L1d_kickouts++;
      vals.L2_dirty_kickouts++;
      vals.L2_kickouts++;
   }
   // l2 cache miss
   vals.L2_miss++;
   vals.L1d_hit+=reqsl1-1;
   vals.L2_transfers++;
   l1->row[trace.L1_index].col[0].tag = trace.L1_tag;
   l2->row[trace.L2_index].col[0].tag = trace.L2_tag;
   l1->row[trace.L1_index].col[0].dirty = false;
   l2->row[trace.L2_index].col[0].dirty = false;
   l1->row[trace.L1_index].col[0].valid = true;
   l2->row[trace.L2_index].col[0].valid = true;
   lruUpdate(l1,trace.L1_index,0);
   lruUpdate(l2,trace.L2_index,0);
   return;
}

void dataWriteCache(cache_t *l1, cache_t *l2, traceData trace) {
   int reqsl1 = (int)ceil((double)trace.numBytes/4);
   if (trace.L1_offset%4 + trace.numBytes > 4*reqsl1) {
      reqsl1++;
   }
   int reqsl2 = (int)ceil((double)trace.numBytes/16);
   if (trace.L2_offset%16 + trace.numBytes > 16*reqsl2) {
      reqsl2++;
   }
   // seach l1 for tag
   for (int i = 0; i < l1->assoc; i++) {
      if ((l1->row[trace.L1_index].col[i].tag == trace.L1_tag) && (l1->row[trace.L1_index].col[i].valid)) {
         // l1 cache hit
         vals.L1d_hit+=reqsl1;
         l1->row[trace.L1_index].col[i].dirty = true;
         lruUpdate(l1,trace.L1_index,i);
         return;
      }
   }
   // l1 cache miss, check l2
   vals.L1d_miss++;
   vals.L1d_transfers++;
   for (int i = 0; i < l2->assoc; i++) {
      // search l2 for tag
      if ((l2->row[trace.L2_index].col[i].tag == trace.L2_tag) && (l2->row[trace.L2_index].col[i].valid)) {
         // l2 cache hit, find block in l1 to evict
         vals.L2_hit+=reqsl2;
         vals.L1d_hit+=reqsl1-1;
         lruUpdate(l2,trace.L2_index,i);
         if (l1->row[trace.L1_index].col[0].valid) {
            // l1 valid, l1 kickout required
            if (l1->row[trace.L1_index].col[0].dirty) {
               // l1 dirty, l2 dirty kickout required
               l2->row[trace.L2_index].col[0].dirty = true;
               if (l2->row[trace.L2_index].col[0].valid) {
                  // l1 dirty, l2 valid, l2 kickout required
                  if (l2->row[trace.L2_index].col[0].dirty) {
                     // l1,l2 block dirty, l2 dirty kickout required
                     vals.L2_dirty_kickouts++;
                  }
                  vals.L2_kickouts++;
               }
               vals.L1d_dirty_kickouts++;
               l2->row[trace.L2_index].col[0].tag = l1->row[trace.L1_index].col[0].tag;
            }
            vals.L1d_kickouts++;
         }
         // write l2 hit to free LRU l1
         l1->row[trace.L1_index].col[0].tag = trace.L1_tag;
         l1->row[trace.L1_index].col[0].valid = true;
         l2->row[trace.L2_index].col[0].valid = true;
         l1->row[trace.L1_index].col[0].dirty = true;
         l2->row[trace.L2_index].col[l2->assoc-1].dirty = true; //lru update puts the hit tag to the back
         l2->row[trace.L2_index].col[l2->assoc-1].valid = true;
         return;
      }
   }

   if (l1->row[trace.L1_index].col[0].valid) {
      // l2 cache miss (l1 valid)
      if(l1->row[trace.L1_index].col[0].dirty) {
         // l2 cache miss (l1 valid and dirty)
         vals.L1d_dirty_kickouts++;
      }
      // instant dirty kickout l1 -> l2 -> mem
      vals.L1d_kickouts++;
      vals.L2_dirty_kickouts++;
      vals.L2_kickouts++;
   }
   // l2 cache miss
   vals.L2_miss++;
   vals.L1d_hit+=reqsl1-1;
   vals.L2_transfers++;
   l1->row[trace.L1_index].col[0].tag = trace.L1_tag;
   l2->row[trace.L2_index].col[0].tag = trace.L2_tag;
   l1->row[trace.L1_index].col[0].dirty = true;
   l2->row[trace.L2_index].col[0].dirty = false;
   l1->row[trace.L1_index].col[0].valid = true;
   l2->row[trace.L2_index].col[0].valid = true;
   lruUpdate(l1,trace.L1_index,0);
   lruUpdate(l2,trace.L2_index,0);
   return;
}

// put recently used block_st at end. LRU  block_st is at beginning
void lruUpdate(cache_t *cache, unsigned int index, unsigned int col) {
   if (col == cache->assoc-1) {
      return;
   }
   block_st curr = cache->row[index].col[col];
   for (int i = col; i < cache->assoc-1; i++) {
      cache->row[index].col[i] = cache->row[index].col[i+1];
   }
   cache->row[index].col[cache->assoc-1] = curr;
}


/**************************************************************************
This function prints the simulation results to a file
***************************************************************************/
void printResultsToFile(cache_t *l1i, cache_t *l1d, cache_t *l2,mem_params params) {
   float percent;
   static char res[30];
   unsigned long exec_time=10;
   float cpi = 4.9;
   strcpy(res,"output.");
   strcat(res, trace_file);
   strcat(res, ".");
   strcat(res, params.setName);
   strcat(res, ".txt");
   FILE *results = fopen(res,"w");

   /**** Initialize vals for print testing ******/
   vals.read_cycles = 100;
   vals.write_cycles = 100;
   vals.instruction_cycles = 400;
   vals.exec_time = vals.read_cycles + vals.write_cycles + vals.instruction_cycles;

   /*********************************************/

   fprintf(results, "---------------------------------------------------------\n");
   fprintf(results, "      %s.%s     Simulation Results\n", trace_file, params.setName);
   fprintf(results, "---------------------------------------------------------\n\n");
   fprintf(results, "Memory System:\n");
   fprintf(results, "  L1 Dcache size = %d : ways = %d : block size = %d\n", params.L1.cache_size, params.L1.assoc, params.L1.block_size);
   fprintf(results, "  L1 Icache size = %d : ways = %d : block size = %d\n", params.L1.cache_size, params.L1.assoc, params.L1.block_size);
   fprintf(results, "  L2 Cache size = %d : ways = %d : block size = %d\n", params.L2.cache_size, params.L2.assoc, params.L2.block_size);
   fprintf(results, "  Memory ready time  = %d : chunksize = %d : chunktime = %d\n\n", params.mmem.ready, params.mmem.chunkSize, params.mmem.chunkTime);
   fprintf(results, "Execute time =    %llu;    Total refs = %ld\n", vals.exec_time, (vals.instruction_references + vals.number_reads + vals.number_writes));
   fprintf(results, "Flush time =      %lu\n", vals.flush_time);
   fprintf(results, "Instruction references = %lu;    Data references = %lu\n\n", vals.instruction_references, (vals.number_reads + vals.number_writes));
   fprintf(results, "Number of reference types:     [Percentage]\n");
   percent = (float)(vals.number_reads*100/(vals.instruction_references + vals.number_reads + vals.number_writes));
   fprintf(results, "  Reads   =             %lu     [%.1f\%]\n", vals.number_reads, percent);
   percent = (float)(vals.number_writes*100/(vals.instruction_references + vals.number_reads + vals.number_writes));
   fprintf(results, "  Writes  =             %lu     [%.1f\%]\n", vals.number_writes, percent);
   percent = (float)(vals.instruction_references*100/(vals.instruction_references + vals.number_reads + vals.number_writes));
   fprintf(results, "  Inst.   =             %lu     [%.1f\%]\n", vals.instruction_references, percent);
   fprintf(results, "  Total   =             %lu\n\n", (vals.instruction_references + vals.number_reads + vals.number_writes));
   fprintf(results, "Total cycles for activities:  [Percentage]\n");
   percent = (float)(vals.read_cycles*100/vals.exec_time);
   fprintf(results, "  Reads   =            %lld     [%.1f\%]\n", vals.read_cycles, percent);
   percent = (float)(vals.write_cycles*100/vals.exec_time);
   fprintf(results, "  Writes  =            %lld     [%.1f\%]\n", vals.write_cycles, percent);
   percent = (float)(vals.instruction_cycles*100/vals.exec_time);
   fprintf(results, "  Inst.   =            %lld     [%.1f\%]\n", vals.instruction_cycles, percent);
   fprintf(results, "  Total   =            %lld\n\n", vals.exec_time);
   fprintf(results, "Average cycles per activity:\n");
   if (vals.number_reads != 0) {
      fprintf(results, "  Read    =            %.1f\n", (float)(vals.read_cycles/vals.number_reads));
   } else
      fprintf(results, "  Read    =            N/A\n");

   if (vals.number_writes != 0) {
      fprintf(results, "  Write   =            %.1f\n", (float)(vals.write_cycles/vals.number_writes));
   } else
      fprintf(results, "  Write   =            N/A\n");
   if (vals.instruction_references != 0) {
      fprintf(results, "  Inst.   =            %.1f\n", (float)(vals.instruction_cycles/vals.instruction_references));
   } else
      fprintf(results, "  Inst.   =            N/A\n");
   //Calculate ideal exec time and CPI
   fprintf(results, "Ideal: Exec. Time = %lu;   CPI = %.1f\n", exec_time, cpi);
   //calculate ideal exec time and CPI for mis-aligned access
   fprintf(results, "Ideal mis-aligned: Exec. Time = %lu;   CPI = %.1f\n\n", exec_time, cpi);

   fprintf(results, "Memory Level:      L1i\n");
   fprintf(results, "  Hit Count       =   %lu\n", vals.L1i_hit);
   fprintf(results, "  Miss count      =   %lu\n", vals.L1i_miss);
   fprintf(results, "  Total Requests  =   %lu\n", (vals.L1i_hit + vals.L1i_miss));
   fprintf(results, "  Hit Rate        =   %.1f\%\n", (float)((float)vals.L1i_hit*100/((float)vals.L1i_hit + (float)vals.L1i_miss)));
   fprintf(results, "  Miss Rate       =   %.1f\%\n", (float)((float)vals.L1i_miss*100/((float)vals.L1i_hit + (float)vals.L1i_miss)));
   fprintf(results, "  Kickouts        =   %lu\n", vals.L1i_kickouts);
   fprintf(results, "  Dirty kickouts  =   %lu\n", vals.L1i_dirty_kickouts);
   fprintf(results, "  Transfers       =   %lu\n", vals.L1i_transfers);
   fprintf(results, "  Flush Kickouts  =   %lu\n\n", vals.flushes);

   fprintf(results, "Memory Level:      L1d\n");
   fprintf(results, "  Hit Count       =   %lu\n", vals.L1d_hit);
   fprintf(results, "  Miss count      =   %lu\n", vals.L1d_miss);
   fprintf(results, "  Total Requests  =   %lu\n", (vals.L1d_hit + vals.L1d_miss));
   fprintf(results, "  Hit Rate        =   %.1f\%\n", (float)((float)vals.L1d_hit*100/((float)vals.L1d_hit + (float)vals.L1d_miss)));
   fprintf(results, "  Miss Rate       =   %.1f\%\n", (float)((float)vals.L1d_miss*100/((float)vals.L1d_hit + (float)vals.L1d_miss)));
   fprintf(results, "  Kickouts        =   %lu\n", vals.L1d_kickouts);
   fprintf(results, "  Dirty kickouts  =   %lu\n", vals.L1d_dirty_kickouts);
   fprintf(results, "  Transfers       =   %lu\n", vals.L1d_transfers);
   fprintf(results, "  Flush Kickouts  =   %lu\n\n", vals.flushes);

   fprintf(results, "Memory Level:      L2\n");
   fprintf(results, "  Hit Count       =   %lu\n", vals.L2_hit);
   fprintf(results, "  Miss count      =   %lu\n", vals.L2_miss);
   fprintf(results, "  Total Requests  =   %lu\n", (vals.L2_hit + vals.L2_miss));
   fprintf(results, "  Hit Rate        =   %.1f\%\n", (float)((float)vals.L2_hit*100/((float)vals.L2_hit + (float)vals.L2_miss)));
   fprintf(results, "  Miss Rate       =   %.1f\%\n", (float)((float)vals.L2_miss*100/((float)vals.L2_hit + (float)vals.L2_miss)));
   fprintf(results, "  Kickouts        =   %lu\n", vals.L2_kickouts);
   fprintf(results, "  Dirty kickouts  =   %lu\n", vals.L2_dirty_kickouts);
   fprintf(results, "  Transfers       =   %lu\n", vals.L2_transfers);
   fprintf(results, "  Flush Kickouts  =   %lu\n\n", vals.flushes);

   //Calculate memory cost according to specifications
   int l1_cost, l2_cost, mem_cost, total_cost, temp;

   temp = 100* (pow(2,(params.L1.cache_size/4096)-1) *log2(params.L1.assoc)+1);
   temp = pow(2,(params.L1.cache_size/4096)-1);
   temp = (log2(params.L1.assoc));
   temp = 25* pow(2,(params.L2.cache_size/32768)-1);
   temp = 50*(log2(params.L2.assoc));


//   if (params.L1.assoc > 1)
      l1_cost = 100* (pow(2,(params.L1.cache_size/4096)-1) *(log2(params.L1.assoc) + 1));
//   else
//      l1_cost = 100* pow(2,(params.L1.cache_size/4096)-1);

   fprintf(results, "L1 cache cost (Icache $%d) + (Dcache $%d) = $%d\n", l1_cost, l1_cost, 2*l1_cost);

//   if (params.L2.assoc > 1)
      l2_cost = 25* (pow(2,(params.L2.cache_size/32768)-1) * (log2(params.L2.assoc) + 1));
//   else
//      l2_cost = 25* pow(2,(params.L2.cache_size/32768)-1);

   fprintf(results, "L2 cache cost = $%d\n", l2_cost);
   mem_cost = 75 + (params.mmem.ready/50)*200 + (params.mmem.chunkSize/15 - 1)*100;

   temp = 50 * (50.0/params.mmem.ready);
   temp = 50.0/params.mmem.ready;
   temp = (params.mmem.ready/50.0)*200;
   temp = (params.mmem.chunkSize/15 - 1)*100;


   fprintf(results, "Memory Cost = $%d\n", mem_cost);
   total_cost = mem_cost + 2*l1_cost + l2_cost;
   fprintf(results, "Total cost = $%d\n", total_cost);
   fprintf(results, "Flushes = %lu : Invalidates = %lu\n\n", vals.flushes, vals.total_kickouts);
   fprintf(results, "----------------------------------------------------------------------------\n\n");
   fprintf(results, "Cache final contents - Index and Tag values are in HEX\n\n");
   fprintf(results, "MemoryLevel: L1i\n");

   for (int i = 0; i < params.L1.numRowsL1; i++) {
      for (int j = 0; j < params.L1.assoc; j++) {
         if ((l1i->row[i].col[j].valid)) {
            fprintf(results, "Index:\t%x\t|", i);
            fprintf(results," V:%s D:%s Tag: %llx | ", l1i->row[i].col[j].valid ? "true " : "false", l1i->row[i].col[j].dirty ? "true " : "false", l1i->row[i].col[j].tag);
            fprintf(results, "\n");
         }

      }
   }
   fprintf(results, "\nMemoryLevel: L1d\n");

   for (int i = 0; i < params.L1.numRowsL1; i++) {
      for (int j = 0; j < params.L1.assoc; j++) {
         if ((l1d->row[i].col[j].valid)) {
            fprintf(results, "Index:\t%x\t|", i);
            fprintf(results," V:%s D:%s Tag: %llx | ", l1d->row[i].col[j].valid ? "true " : "false", l1d->row[i].col[j].dirty ? "true " : "false", l1d->row[i].col[j].tag);
            fprintf(results, "\n");
         }
      }
   }
   fprintf(results, "\nMemoryLevel: L2\n");

   for (int i = 0; i < params.L2.numRowsL2; i++) {
      for (int j = 0; j < params.L2.assoc; j++) {
         if ((l2->row[i].col[j].valid)) {
            fprintf(results, "Index:\t%x\t|", i);
            fprintf(results," V:%s D:%s Tag: %llx | ", l2->row[i].col[j].valid ? "true " : "false", l2->row[i].col[j].dirty ? "true " : "false", l2->row[i].col[j].tag);
            fprintf(results, "\n");
         }
      }
   }
   fclose(results);
}
