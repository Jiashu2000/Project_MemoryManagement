# Memory-Management

- Name: Jiashu Chen
- Repository URL: https://github.com/Jiashu2000/Memory-Management.git

## Table of Contents
- Data Structure
- Freelist Features
- Results Observed
- Resources

## Data Structure
The data structure used to support heap memory management is a singly linked list of freeNode. The freeNode class contains two data fields: size, which is an unsigned integer representing the size of the memory block that could be used, and next, which is a pointer to the next freeNode. Therefore, the NODESIZE is 16 bytes.

## Freelist Features
There are two main funtions manipulating the freelist to achieve memory management: getmem() and freemem().

### getmem()
- The getmem() function takes a positive integer as input, which is the requested memory size. The input size will be round up to include NODESIZE and for alignment. If there isn't enough momery on the freelist or there isn't a freeNode with size larger than the requested size, it will call the malloc() function to get more memory. 
- The getmem() function will find the freeNode that satisfy the request. If the founded freeNode has a size much larger than the requested size, the freeNode will be splited and remaining memory will be left on the freelist. On the contrary, if the remaining memory is small, the freeNode will be entrely taken out from the freelist. The threshold of splitting the freeNode is defined by the variable SPLITTHRES.
- The getmem() function will return the pointer to the address of the founded freeNode + NODESIZE.

### freemem()
- The freemem() function takes a pointer to previously allocated memory address as input. To obtain the size of the memory block, the function will minus the the address by NODESIZE and then access the freeNode's size.
- The freemem() functin will add the memory block back to the freelist and merge the block with neighbors if the previous freeNode or post freeNode are physically adjacent.

### bench
- In the bench program, a big array is created to store allocated memory pointers. The length of the array equals the number of trials. If not specified by users, the default number of trials is 10000. With the array, freemem() could access previously allocated memory address in O(1) time. 
- Taking the modulus could be expensive for big numbers. The program takes modulus only once and store it when necessary.

### additional parameters
- minimum malloc() request: since malloc() is expensive, malloc() should be called with certain minimum memory request.
- split threshold: since one of the main problems with heap memory allocation, the threshold of splitting the big memory block should be considered.

## Results Observed
-  the bench program finished 10000 ntrials in 0.008072 seconds. The minimum malloc() request is 4000 and the split threshold is 96.
-  the bench program finished 10000 ntrials in 0.005380 seconds. The minimum malloc() request is 4000 and the split threshold is 160.
-  the bench program finished 10000 ntrials in 0.006250 seconds. The minimum malloc() request is 4000 and the split threshold is 240.
-  the bench program finished 10000 ntrials in 0.006341 seconds. The minimum malloc() request is 8000 and the split threshold is 240.
-  the bench program finished 10000 ntrials in 0.005567 seconds. The minimum malloc() request is 8000 and the split threshold is 160.
-  Overall, it seems that setting the minimum malloc() size at 4000 and split threshold at 160 is slightly better.

## Resources
- CSE374 Course materials
