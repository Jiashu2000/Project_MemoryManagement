/* freemem.c
   implements freemem (free) for memory system
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

/*
 the insertNode function is to add the returned memory block
 back to the freelist. There are four cases of insertion.
 Meanwhile, if the returned freeNode is adjacent to "neighbors"
 on the freelist, the node will be merged with adjacent neighbors.
*/
void insertNode(freeNode* node);

/*
 check if two freeNodes are adjacent in physical memory.
 if it is combine them to one freeNode.
 */
void checkCombine(freeNode* prev, freeNode* cur);

/*
 the isAdjacent function is a helper function to
 the checkCombine function. It checks whether two input
 freeNodes are physically adjacent in memmory.
 */
int isAdjacent(freeNode* prev, freeNode* cur);

/*
 the combineAdjNode function is a helper function to
 the checkCombine function. It merges two adjacent freeNodes
 into one freeNode.
 */
void combineAdjNode(freeNode* prev, freeNode* cur);


/* Define functions below: */

void freemem(void* p) {
  check_heap();
  if (p == NULL) {
    return;  // invalid input
  }
  // need to minus the pointer by NODESIZE
  // to get the real address of freeNode
  freeNode* ptr = (freeNode*) ((uintptr_t) p - NODESIZE);
  totalfree += ptr->size + NODESIZE;
  insertNode(ptr);  // insert node to the freelist
  check_heap();
}

/*                                                 
 the function is to add returned memory block to the freelist
 There are four different cases of insertion.
 input:
 freeNode* node: the pointer to the head of returned memory block
*/
void insertNode(freeNode* node) {
  // case 1: the freelist is null
  // newNode becomes head of the freelist.
  if (freelist == NULL) {
    freelist = node;
  } else {
    freeNode* currentNode = freelist;
    freeNode* prevNode = freelist;
    while (currentNode) {
      // if node < currentNode, it means that the node
      // should be placed before the currentNode
      if (node < currentNode) {
       // case2: node should be placed before currentNode.
       // the currentNode is freelist head.
       // therefore, the node should become the freelist head.
        if (prevNode == currentNode) {
           checkCombine(node, currentNode);
           freelist = node;
        } else {
         // case3: currentNode is in the middle of freelist.
         // check whether the node could combine with currentNode.
         // then check whether the node could combine with prevNode.
          checkCombine(node, currentNode);
          checkCombine(prevNode, node);
        }
        return;
       } else {
        prevNode = currentNode;
        currentNode = currentNode->next;
      }
    }
    // case4: currentNode is NULL. It means that it has reached
    // the end of the freelist. the node should be added to the end.
    if (currentNode == NULL) {
      checkCombine(prevNode, node);
    }
  }
}

/* 
 check whether two freeNodes are adjacent. It they are,
 combine them. If not, set the second freeNode as the next
 of the first freeNode.
 The order of two arguments matters.
 */
void checkCombine(freeNode* prev, freeNode* cur) {
  if (isAdjacent(prev, cur)) {
    combineAdjNode(prev, cur);
  } else {
    prev->next = cur;
  }
}

/*
 determine if two freeNodes are adjacent.
 true if it returns 1
 false if it returns 0
 */
int isAdjacent(freeNode* prev, freeNode* cur) {
  return (uintptr_t)prev + prev->size + NODESIZE == (uintptr_t)(cur);
}


/* combine adjacent freeNodes */
void combineAdjNode(freeNode* prev, freeNode* cur) {
  prev->next = cur->next;
  prev->size = prev->size + NODESIZE + cur->size;
}
