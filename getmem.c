/* getmem.c
   implements getmem (malloc) for memory system
   CSE 374 HW6
*/
// Copyright [2022] <Jiashu Chen>
/*
Name: Jiashu Chen
Date: 07/12/2022
*/

#include <assert.h>
#include "mem.h"
#include "mem_impl.h"

/* the threshold for leaving the remaining chunk in the freelist */
#define SPLITTHRESH 96

/* initialize global variables*/
freeNode* freelist = NULL;

uintptr_t totalmalloc = 0;

uintptr_t totalfree = 0;

/* declare helper functions */

/*
 the findBlock function searches through the freelist for 
 a block at least as large as the requested size.
 the output is the pointer to the start of the block.
*/ 
void* findBlock(uintptr_t size);

/*
 the splitBlock function is a helper function to the findBlock
 function. When the searched block is much larger than the 
 requested size (remaining memory exceeds splitting threshold),
 the block is going to be splited.
 the splitBlock function creates a new freeNode to store the
 remaining memory of the original block.
 */
freeNode* splitBlock(freeNode* p, uintptr_t size);

/*
 the requestMem function is to request more memory using malloc.
 the function is used at the begining or whenever the freelist
 does not have a freeNode with size larger than the requested size.
 */
void requestMem(uintptr_t requestSize);

/*
  the insertMem function is a helper function to the requestMem
  function. The function is to insert new memory into the freelist.
 */
void insertMem(uintptr_t mem, uintptr_t size);


/* Define functions: */

/*
 the getmem function is to get the memory block at least
 as large as the request size
 input: requested size
 output: void pointer to the start of the memory block needed.
 */
void* getmem(uintptr_t size) {
  check_heap();
  if (size <= 0) {  // invalid size
    return NULL;
  }

  // around up the size for alignment
  int remain = size % NODESIZE;
  if (remain != 0) {
    size = size + (16 - remain);
  }

  // request memory if the freelist is NULL or
  // remaining free memory cannot cover request.
  if (freelist == NULL || totalfree < size) {
    int requestSize = size + 4000;
    requestMem(requestSize);
  }

  void* p = findBlock(size);
  check_heap();
  return p;
}

/*
  the findBlock function is to find the first memory block
  in the freelist that is larger than the requested size.
  Split the memory block if remaining memory is
  larger than the splitting threshold. 
 */
void* findBlock(uintptr_t size) {
  freeNode* currentNode = freelist;
  freeNode* prevNode = freelist;
  while (currentNode) {
    if (currentNode->size >= size) {
      break;
    }
    prevNode = currentNode;
    currentNode = currentNode->next;
  }
  // if there is no freeNode in the freelist with size
  // larger than the requested size, request more memory,
  // and then recall the findBlock() function.
  if (!currentNode) {
    int requestSize = size + 4000;
    requestMem(requestSize);
    return findBlock(size);
  }
  // check if the remaining block size is larger than splitting threshold
  if ((currentNode->size) - size >= SPLITTHRESH) {
    freeNode* remainNode = splitBlock(currentNode, size);
    // prevNode == currentNode means that currentNode is the freelist head
    if (prevNode == currentNode) {
      freelist = remainNode;
    } else {
      prevNode->next = remainNode;
    }
  } else {
    // if the remaing memory is small, return the whole block
    // update the allocated size
    size = currentNode->size;
    if (prevNode == currentNode) {
      freelist =  currentNode->next;
    } else {
      prevNode->next = currentNode->next;
    }
  }
  totalfree = totalfree - size - NODESIZE;
  return ((void*) currentNode + NODESIZE);
}

/*
 splitting the block by creating a new freeNode for remaining memory
 input:
 freeNode* p: pointer to the start of the current freeNode.
 uintptr_t size: size to be split from the current freeNode.
 */
freeNode* splitBlock(freeNode* p, uintptr_t size) {
  freeNode* newNode = (freeNode*) ((uintptr_t)(p) + size + NODESIZE);
  newNode->size = p->size - size - NODESIZE;
  newNode->next = p->next;
  p->size = size;
  p->next = NULL;
  return newNode;
}

/*
 the function is to request more memory from emacs.
 to improve efficiency, the request size is at least 4000.
 */
void requestMem(uintptr_t requestSize) {
  // if the input request size is smaller than 4000, change to 4000.
  if (requestSize <= 4000) {
    requestSize = 4000;
  } else {
    // round up the size for alignment.
    // include NODESIZE
    int remain = requestSize % NODESIZE;
    if (remain != 0) {
      requestSize = NODESIZE + requestSize + (16 - remain);
    } else {
      requestSize += NODESIZE;
    }
  }
  uintptr_t mem = (uintptr_t) malloc(requestSize);
  // check if malloc allocated memory successfully.
  if ((void*) mem == NULL) {
    fprintf(stderr, "Failed to get memory from Malloc()\n");
    exit(EXIT_FAILURE);
  }
  totalmalloc += requestSize;
  totalfree += requestSize;
  // insert the memory block to the freelist
  insertMem(mem, requestSize);
}

/*
 the function is to add the new memory to the freelist.
 There are four different cases of insertion.
 input:
 uintptr_t mem: address of the head of the new memory block.
 uintptr_t size: total size of the new memery block.
 */
void insertMem(uintptr_t mem, uintptr_t size) {
  // create new freeNode for the new memory block;
  freeNode* newNode = (freeNode*) (mem);
  newNode->size = size - NODESIZE;
  newNode->next = NULL;

  // case 1: first memory request, the freelist is null
  // newNode becomes head of the freelist.
  if (freelist == NULL) {
    freelist = newNode;
  } else {
    freeNode* currentNode = freelist;
    freeNode* prevNode = freelist;
    while (currentNode) {
      if (mem < (uintptr_t) currentNode) {
      // case 2: the freelist is not null
      // but the new memory address is before the freelist.
      // newNode becomes the head of the freelist.
      // connects to previous head.
       if (prevNode == currentNode) {
          newNode->next = currentNode;
          freelist = newNode;
          return;
       } else {
      // case3: mem inserted to the middle of the freelist
          newNode->next = prevNode->next;
          prevNode->next = newNode;
          return;
        }
       } else {
        prevNode = currentNode;
        currentNode = currentNode->next;
      }
    }
    // case4: the while loop break because currentNode
    // reaches the end of the freelist
    // mem inserted to the end of the freelist
    if (currentNode == NULL) {
      prevNode->next = newNode;
    }
  }
}
