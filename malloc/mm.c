#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "memlib.h"
#include "mm.h"

/****************************************************************/
// Useful type aliases

typedef uint64_t word;
typedef uint32_t tag;
typedef uint8_t  byte;
typedef byte*    address; 

/*****************************************************************/
//static functions

/****************************************************************/
//global variables

address g_baseHeap;

/****************************************************************/
// Useful constants

const uint8_t WORD_SIZE = sizeof (word);
const uint8_t DWORD_SIZE = 2 * sizeof(word);
const uint8_t HWORD_SIZE = sizeof(word)/2;
const uint8_t MIN_SIZE = 2;
// Add others... 

/****************************************************************/
// Inline functions
static inline tag* header (address ptr)
{
  ptr -= HWORD_SIZE;
   return (tag*) ptr;
}
static inline int isAllocated(address ptr)
{
  tag* hptr = header(ptr);
  return *hptr & 1;
}
static inline tag sizeOf(address ptr)
{
  tag* hptr = header(ptr);
  tag tmp = *hptr;
  return (tmp >> 1) << 1;
}
static inline tag* footer(address ptr){
   return (tag*)(ptr + sizeOf(ptr) * WORD_SIZE - WORD_SIZE);
}
static inline address nextBlock (address ptr)
{

  return ptr += (sizeOf(ptr) * WORD_SIZE);
}

static inline tag* nextHeader (address ptr)
{
tag* nhead = footer(ptr) + 0x01;
return nhead;
}
/* returns a pointer to the prev block’s
footer. HINT: you will use header()*/
static inline tag* prevFooter (address ptr)
{
  return header(ptr) - 0x01;
}
/* gives the basePtr of prev block */
static inline address prevBlock (address ptr)
{
  tag prevSize = *prevFooter(ptr) >> 1 << 1;
  return ptr -= (prevSize * WORD_SIZE);
}

static inline void makeBlock (address ptr, tag size , tag allo)
{
  tag head = size | allo;
  tag foot = size | allo;
  *header(ptr) = head;
  *footer(ptr) = foot;
}

static inline void toggleBlock (address ptr)
{
  tag head = *header(ptr) ^ 1;
  tag foot = *footer(ptr) ^ 1;
  *header(ptr) = head;
  *footer(ptr) = foot;
}


static address extendHeap (uint32_t numWords)
{
  address ptr = mem_sbrk((int)(numWords * WORD_SIZE));
  if(ptr == NULL){
    return NULL;
  }
  if((*prevFooter(ptr) & 1) == 0)
  {
    makeBlock(prevBlock(ptr), numWords, 0);
    ptr = prevBlock(ptr);
  }
  else
  {
  makeBlock(ptr,numWords, 0);
  }
  tag dummyHead = 0 | 1;
  *nextHeader(ptr) = dummyHead;
  //Use mem_sbrk to increase the heap size by the appropriate #
  //   of bytes. Return NULL on error.
  // Make a free block of size "numWords"
  // Place the end sentinel header
  // Coalesce newly created free block
  return ptr;
}




/****************************************************************/
// Non-inline functions

int
mm_init (void)
{
  address start = mem_sbrk(64);
  if(start == NULL){
    return -1;
  }
  tag prevfoot = 0 | 1;
  tag nexthead = 0 | 1;
  g_baseHeap = start + DWORD_SIZE;
  *header(g_baseHeap) = 6 | 0;
  *footer(g_baseHeap) = 6 | 0;
  *prevFooter(g_baseHeap) = prevfoot;
  *nextHeader(g_baseHeap) = nexthead;
  return 0;

}

/****************************************************************/

void*
mm_malloc (uint32_t size)
{
  uint32_t num_bytes =  size + WORD_SIZE;
  uint32_t num_dword = (num_bytes + (DWORD_SIZE - (tag)1)) / DWORD_SIZE;
  uint32_t num_word = num_dword * (tag)2;
  address temp;
  for(temp = g_baseHeap; sizeOf(temp) !=0; temp = nextBlock(temp))
  {
    if(!isAllocated(temp) && sizeOf(temp) >= num_word)
    {
      break;
    }
  }
  if(sizeOf(temp) == num_word)
  {
    toggleBlock(temp);
    return temp;
  }
  if(sizeOf(temp) == 0)
  {
    temp = extendHeap(num_word);
    tag oldSize = sizeOf(temp);
    makeBlock(temp, num_word, 1);
    makeBlock(nextBlock(temp), oldSize - num_word, 0);
  }
  if(num_word < sizeOf(temp))
  {
    tag oldSize = sizeOf(temp);
    makeBlock(temp, num_word, 1);
    makeBlock(nextBlock(temp), oldSize - num_word, 0);
  }
  return temp;
}

/****************************************************************/

void
mm_free (void *ptr)
{
    if(isAllocated(prevBlock(ptr)) && isAllocated(nextBlock(ptr)))
    {
    toggleBlock(ptr);
    return;
    }
  if(!isAllocated(prevBlock(ptr)))
  {
    makeBlock(prevBlock(ptr), sizeOf(ptr) + sizeOf(prevBlock(ptr)), 0);
    ptr = prevBlock(ptr);
   }
  if(!isAllocated(nextBlock(ptr)))
  {
    makeBlock(ptr, sizeOf(ptr) + sizeOf(nextBlock(ptr)), 0);
  }
}

/****************************************************************/

void*
mm_realloc (void *ptr, uint32_t size)
{
  if(ptr == NULL)
  {
   ptr = mm_malloc(size);
   return ptr;
  }
  if(size == 0)
  {
    mm_free(ptr);
    return ptr;
  }
    mm_free(ptr);
    return mm_malloc(size);
  
}



void
printPtrDiff (const char* header, void* p, void* base)
{
  printf ("%s: %td\n", header, (address) p - (address) base);
}

void
printBlock (address p)
{
  printf ("Block Addr %p; Size %u; Alloc %d\n",
	  p, sizeOf (p), isAllocated (p)); 
}

int mm_check()
{
  //canonical loop to traverse all blocks
  printf("all blocks\n");
  for(address p = g_baseHeap; sizeOf(p) !=0; p = nextBlock(p))
  {
    printBlock(p);
  }
  return 0;
}
/*
int main()
{
  mem_init();
  mm_init();
  address a = mm_malloc(200);
  mm_free(a);
  address b = mm_malloc(53);
  b = mm_realloc(b, 78);
  mm_free(b);
  
  mm_check();

  return 0;

}
*/

/*
int
main ()
{
  // Each line is a DWORD
  //        Word      0       1
  //                 ====  ===========
  tag heapZero[] = { 0, 0, 1, 4 | 1,
                     0, 0, 0, 0, 
                     0, 0, 4 | 1, 2 | 0, 
                     0, 0, 2 | 0, 1 }; 
                    
  tag heapZero[16] = { 0 }; 
  // Point to DWORD 1 (DWORD 0 has no space before it)
  address g_heapBase = (address) heapZero + DWORD_SIZE;
  makeBlock (g_heapBase, 6 , 0);
  *prevFooter (g_heapBase) = 0 | 1;
  *nextHeader (g_heapBase) = 1;
  //makeBlock (g_heapBase, 4 , 1);
  //makeBlock (nextBlock (g_heapBase), 2, 1); 
  printPtrDiff ("header", header (g_heapBase), heapZero);
  printf ("%s: %d\n", "sizeOf", sizeOf (g_heapBase));
  printPtrDiff ("footer", footer (g_heapBase), heapZero);
  printPtrDiff ("nextBlock", nextBlock (g_heapBase), heapZero);
  printPtrDiff ("prevFooter", prevFooter (g_heapBase), heapZero);
  printPtrDiff ("nextHeader", nextHeader (g_heapBase), heapZero);
  address twoWordBlock = nextBlock (g_heapBase); 
  printPtrDiff ("prevBlock", prevBlock (twoWordBlock), heapZero);

  printf ("%s: %d\n", "isAllocated", isAllocated (g_heapBase)); 
  printf ("%s: %d\n", "sizeOf", sizeOf (g_heapBase));

  // Canonical loop to traverse all blocks
  printf ("All blocks\n"); 
  for (address p = g_heapBase; sizeOf (p) != 0; p = nextBlock (p))
    printBlock (p);
  
  return 0;
}
*/
//static inline address
//extendHeap (uint32_t numWords)
//{
  // Use mem_sbrk to increase the heap size by the appropriate #
  //   of bytes. Return NULL on error.
  // Make a free block of size "numWords"
  // Place the end sentinel header
  // Coalesce newly created free block
//}

/***********************************************************************/

/*
  Allocating initial heap area and default-initializing any global variables
  Place sentinel blocks in heap, one in prologue and one in epilogue
  Return -1 if any problems, else 0
*/
//int
//mm_init (void)
//{
  // Use mem_sbrk to allocate a heap of some # of DWORDS, perhaps 4
  //   like in the guide (at least 1 DWORD). 
  // Return -1 on error.
  // Set g_heapBase to point to DWORD 1 (0-based). 
  // Make a free block.
  // Place the begin sentinel footer
  // Place the end sentinel header
  // Return 0 for success
//}