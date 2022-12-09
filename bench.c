/* bench.c is the benchmarking /test program for mem memory management
   bench.c is implemented as part of HW6, CSE374 22WI
*/
// CSE374 HW6
// Jiashu Chen
// 07/12/2022
// Copyright [2022] <Jiashu Chen>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include "mem.h"

/* print_stats is a helper function that prints performance
   information about the memory system.  Requires a clock_t start
   time, and prints elapsed time plus results from get_mem_stats()
*/
void print_stats(clock_t start, uintptr_t* total_size,
                 uintptr_t* total_free, uintptr_t* n_free_blocks);

/* fill_mem is a helper function that takes in an address and the
   size of the memory allocation at that address.  It should fill
   the first 16 bytes (or up to size) with hexedecimal values.*/
void fill_mem(void* ptr, uintptr_t size);


/* Synopsis:   bench (main)
   [ntrials] (10000) getmem + freemem calls
   [pctget] (50) % of calls that are get mem
   [pctlarge] (10) % of calls requesting more memory than lower limit
   [small_limit] (200) largest size in bytes of small block
   [large_limit] (20000) largest size in byes of large block
   [random_seed] (time) initial seed for randn
*/
int main(int argc, char** argv ) {
  // Initialize the parameters
  int NTRIALS;
  int PCTGET;
  int PCTLARGE;
  int SMALL_L;
  int LARGE_L;

  (argc > 1) ? (NTRIALS = atoi(argv[1])) : (NTRIALS = 10000);
  (argc > 2) ? (PCTGET = atoi(argv[2])) : (PCTGET = 50);
  (argc > 3) ? (PCTLARGE = atoi(argv[3])) : (PCTLARGE = 10);
  (argc > 4) ? (SMALL_L = atoi(argv[4])) : (SMALL_L = 200);
  (argc > 5) ? (LARGE_L = atoi(argv[5])) : (LARGE_L = 20000);

  // initialize random number gen.
  (argc > 6) ? srand(atoi(argv[6])) : srand(time(NULL));

  printf("Running bench for %d trials, %d%% getmem calls.\n", NTRIALS, PCTGET);

  void* blocks[NTRIALS];  // upperbound block storage
  int ntrials = 0, nblocks = 0;
  clock_t start = clock();
  int tenPerc = NTRIALS/10;  // 10% of total trials

  uintptr_t* total_size = (uintptr_t*) malloc(sizeof(uintptr_t*));
  uintptr_t* total_free = (uintptr_t*) malloc(sizeof(uintptr_t*));
  uintptr_t* n_free_blocks = (uintptr_t*) malloc(sizeof(uintptr_t*));

  // perform NTRIALS mem operations

  for ( ; ntrials < NTRIALS; ntrials++) {
    if (ntrials != 0 && tenPerc != 0 && ntrials % tenPerc == 0) {
      printf("    %d percent done\n", ntrials/tenPerc * 10);
      print_stats(start, total_size, total_free, n_free_blocks);
    }
    /* generate a random number to determine the task of this trial
       get large memory, or get small memory, or free a random memory */
    int r = rand() % (100+1);

    if (r <= PCTGET) {  // get memory
      uintptr_t size;
      if (r <= PCTLARGE) {  // get large memory
        size = rand() % (LARGE_L - SMALL_L + 1) + SMALL_L;
      } else {  // get small memory
        size = rand() % (SMALL_L + 1);
      }
      void* p = getmem(size);
      fill_mem(p, size);
      blocks[nblocks] = p;
      nblocks++;

    } else {  // free memory
      if (nblocks == 0) continue;
      int b = rand() % nblocks;

      freemem(blocks[b]);
      if (b != nblocks-1) {
        blocks[b] = blocks[nblocks-1];
      }
      nblocks--;
    }
  }
  printf(" 100 percent done\n");
  print_stats(start, total_size, total_free, n_free_blocks);
  free(total_size);
  free(total_free);
  free(n_free_blocks);

  return EXIT_SUCCESS;
}

/* Define helper functions here. */

void print_stats(clock_t start,  uintptr_t* total_size,
                uintptr_t* total_free, uintptr_t* n_free_blocks) {
  clock_t end = clock();
  double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
  printf("Total CPU time used: %f seconds \n", cpu_time_used);
  get_mem_stats(total_size, total_free, n_free_blocks);
  printf("Total amount of storage acquired from the underlying system: %lu \n",
         *total_size);
  printf("Total number of blocks on the free list: %lu \n", *n_free_blocks);
  printf("Total free on the free list: %lu \n", *total_free);
  if (*n_free_blocks != 0) {
    printf("Average number of bytes in free blocks: %lu \n",
          *total_free/(*n_free_blocks));
  } else {
    printf("Average number of bytes in free blocks: %f \n", 0.0);
  }
}

void fill_mem(void* ptr, uintptr_t size) {
  if (size > 16) {
    size = 16;
  }
  for (int i = 0; i < size; i++) {
    unsigned char* p = (unsigned char*)((uintptr_t) ptr + i);
    *p = 0xFE;
  }
}
