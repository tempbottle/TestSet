/*
 * Copyright (C) 1997-2004 by Objective Systems, Inc.
 *
 * This software is furnished under an open source license and may be 
 * used and copied only in accordance with the terms of this license. 
 * The text of the license may generally be found in the root 
 * directory of this installation in the LICENSE.txt file.  It 
 * can also be viewed online at the following URL:
 *
 *   http://www.obj-sys.com/open/license.html
 *
 * Any redistributions of this file including modified versions must 
 * maintain this copyright notice.
 *
 *****************************************************************************/

#include <stdlib.h>
#include "memheap.h"

ASN1UINT      g_defBlkSize = XM_K_MEMBLKSIZ;
OSMallocFunc  g_malloc_func = malloc;
#ifndef _NO_REALLOC
OSReallocFunc g_realloc_func = realloc;
#else
OSReallocFunc g_realloc_func = 0;
#endif
OSFreeFunc    g_free_func = free;

static OSMemLink* memHeapAddBlock (OSMemLink** ppMemLink, 
                                   void* pMemBlk, int blockType);

typedef struct MemElemDescr {
   ASN1OCTET       flags;       /* isFree = 1, isLast = 2, isSaved = 4 */
   ASN1OCTET       spare;
   ASN1USINT       nunits;      /* size = nunits * 8 */
   ASN1USINT       prevOff;     /* offset of the prev block (-prevOff * 8) */
   union {
      ASN1USINT    nextFreeOff; /* offset/8 of the next free block */
      ASN1USINT    beginOff;    /* offset/8 of beginning of block in MemBlk */
   } u;

} OSMemElemDescr;

typedef struct MemBlk {
   OSMemLink*      plink;
   ASN1USINT       free_x;      /* index of free space at end of block */
   ASN1USINT       freeMem;     /* size of free space before free_x    */
   ASN1USINT       nunits;      /* size of data                        */
   ASN1USINT       lastElemOff; /* last element offset in block        */
   ASN1USINT       freeElemOff; /* first free element offset in block  */
   ASN1USINT       nsaved;      /* num of saved elems in the block     */

   ASN1USINT       spare[2];    /* forces alignment on 8-bytes boundary,
                                   for 64-bit systems */
   char            data[8];
} OSMemBlk;

/* Macros for operations with memory blocks */

#define QOFFSETOF(pElem, pPrevElem) \
((ASN1USINT)(((unsigned)((char*)pElem - (char*)pPrevElem)) >> 3u))

#define OFFSETOF(pElem, pPrevElem) \
((ASN1UINT)((char*)pElem - (char*)pPrevElem))

#define ISFREE(pElem)      (pElem->flags & 1)
#define SET_FREE(pElem)    (pElem->flags |= 1)
#define CLEAR_FREE(pElem)  (pElem->flags &= (~1))

#define ISLAST(pElem)      (pElem->flags & 2)
#define SET_LAST(pElem)    (pElem->flags |= 2)
#define CLEAR_LAST(pElem)  (pElem->flags &= (~2))

#define ISSAVED(pElem)      (pElem->flags & 4)
#define SET_SAVED(pMemBlk,pElem)    do { \
(pElem->flags |= 4); pMemBlk->nsaved++; } while (0)
#define CLEAR_SAVED(pMemBlk,pElem)  do { \
(pElem->flags &= (~4)); pMemBlk->nsaved--; } while (0)

#define ISFIRST(pElem)    (int)(pElem->prevOff == 0)

#define GETPREV(pElem) \
((pElem->prevOff == 0) ? 0 : \
((OSMemElemDescr*) (((char*)pElem) - (pElem->prevOff * 8u))))

#define GETNEXT(pElem) \
((ISLAST (pElem)) ? 0 : \
((OSMemElemDescr*)(((char*)pElem) + ((pElem->nunits + 1) * 8u))))

#define GET_NEXT_FREE(pElem) \
((pElem->u.nextFreeOff == 0) ? 0 : \
((OSMemElemDescr*) (((char*)pElem) + (pElem->u.nextFreeOff * 8u))))

#define GET_MEMBLK(pElem) \
((OSMemBlk*) (((char*)pElem) - (pElem->u.beginOff * 8u) - \
sizeof (OSMemBlk) + sizeof ((OSMemBlk*)0)->data))

#define GET_LAST_ELEM(pMemBlk) \
((pMemBlk->lastElemOff == 0) ? 0 : \
(OSMemElemDescr*)&pMemBlk->data[(pMemBlk->lastElemOff - 1) * 8u])

#define SET_LAST_ELEM(pMemBlk, pElem) \
pMemBlk->lastElemOff = (ASN1USINT)((pElem == 0) ? 0 : \
(SET_LAST (pElem), (QOFFSETOF (pElem, pMemBlk->data) + 1)))

#define GET_FREE_ELEM(pMemBlk) \
((pMemBlk->freeElemOff == 0) ? 0 : \
(OSMemElemDescr*)&pMemBlk->data[(pMemBlk->freeElemOff - 1) * 8u])

#define FORCE_SET_FREE_ELEM(pMemBlk, pElem) do { \
if (pElem == 0) { pMemBlk->freeElemOff = 0; break; } \
SET_FREE (pElem); \
pMemBlk->freeElemOff = (ASN1USINT)(QOFFSETOF (pElem, pMemBlk->data) + 1); \
} while (0)

#define SET_FREE_ELEM(pMemBlk, pElem) setLastElem (pMemBlk, pElem)

/* Memory debugging macros */

#define RTMEMDIAG1(msg)
#define RTMEMDIAG2(msg,a)
#define RTMEMDIAG3(msg,a,b)
#define RTMEMDIAG4(msg,a,b,c)
#define FILLFREEMEM(mem,size)
#define FILLNEWMEM(mem,size)
#define CHECKMEMELEM(memblk, elem)
#define CHECKMEMBLOCK(memheap,memblk)
#define CHECKMEMHEAP(memheap)
#define TRACEMEMELEM(memblk, elem, name)
#define TRACEFREE(memlink,name)

static void setLastElem (OSMemBlk* pMemBlk, OSMemElemDescr* pElem) 
{
   if (pElem == 0) { 
      pMemBlk->freeElemOff = 0; 
      return; 
   }
   else if (ISLAST (pElem)) 
      return; 
   else if (pMemBlk->freeElemOff > QOFFSETOF (pElem, pMemBlk->data) + 1) {
      pElem->u.nextFreeOff = QOFFSETOF (GET_FREE_ELEM (pMemBlk), pElem); 
      FORCE_SET_FREE_ELEM (pMemBlk, pElem); 
   } 
   else if (pMemBlk->freeElemOff == 0) { 
      pElem->u.nextFreeOff = 0;          
      FORCE_SET_FREE_ELEM (pMemBlk, pElem); 
   } 
   else { 
      SET_FREE (pElem); 
      pElem->u.nextFreeOff = 0; 
   }
}

void* memHeapAlloc (void** ppvMemHeap, int nbytes)
{
   OSMemHeap* pMemHeap;
   OSMemLink* pMemLink, **ppMemLink;
   OSMemBlk*  pMemBlk = 0;
   void* mem_p = NULL;
   unsigned remUnits;
   ASN1UINT nunits;

   if (ppvMemHeap == 0)
      return 0;

   if (*ppvMemHeap == 0)
      if (memHeapCreate (ppvMemHeap) != ASN_OK)
         return 0;

   /* Round number of bytes to nearest 8-byte boundary */

   nunits = (((unsigned)(nbytes + 7)) >> 3);

   pMemHeap = (OSMemHeap*) *ppvMemHeap;
   ppMemLink = &pMemHeap->phead;

   /* if size is greater than 2**19, then allocate as RAW block */
   
   if (nunits > (1<<16) - 2) {
      void *data;

      /* allocate raw block */

      data = malloc (nbytes);
      if (data == NULL) {
         return NULL;
      }
      pMemLink = memHeapAddBlock (ppMemLink, data, RTMEMMALLOC | RTMEMRAW);
      /* save size of the RAW memory block behind the pMemLink */
      *(int*)(((char*)pMemLink) + sizeof (OSMemLink)) = nbytes;
      return data;   
   }
   
   RTMEMDIAG2 ("memHeapAlloc: adjusted nbytes = %d\n", nbytes);

   /* Try to allocate a slot from an existing block on the list */

   for (pMemLink = *ppMemLink; pMemLink != 0; pMemLink = pMemLink->pnext) {
      if (pMemLink->blockType & RTMEMRAW) continue;
      else pMemBlk = (OSMemBlk*) pMemLink->pMemBlk;

      remUnits = pMemBlk->nunits - pMemBlk->free_x;

      if ((unsigned)(nunits + 1) <= remUnits) {
         OSMemElemDescr* pElem = (OSMemElemDescr*)
            &pMemBlk->data [((ASN1UINT)pMemBlk->free_x) * 8u];
         OSMemElemDescr* pPrevElem;

         RTMEMDIAG1 ("memHeapAlloc: found existing slot..\n");

         /* if block is clean, set some vars in heap */
         if (pMemBlk->free_x == 0) {
            pMemHeap->freeUnits -= pMemBlk->nunits;
            pMemHeap->freeBlocks--;
         }

         pElem->flags = 0;
         if (pMemBlk->lastElemOff != 0)
            pElem->prevOff = 
               (ASN1USINT)(pMemBlk->free_x - pMemBlk->lastElemOff + 1);
         else 
            pElem->prevOff = 0;
         
         pPrevElem = GET_LAST_ELEM (pMemBlk);
         if (pPrevElem != 0)
            CLEAR_LAST (pPrevElem);
         
         pElem->nunits = (ASN1USINT)nunits; 
         pElem->u.beginOff = QOFFSETOF (pElem, pMemBlk->data);
         pMemBlk->lastElemOff = (ASN1USINT)(pMemBlk->free_x + 1);

         mem_p = (void*) (pElem + 1);
         
         /* sizeof (OSMemElemDescr) == 1 unit */
         pMemBlk->free_x += nunits + 1; 
         
         SET_LAST_ELEM (pMemBlk, pElem);
         
         FILLNEWMEM (mem_p, nunits * 8u);
         TRACEMEMELEM(pMemBlk, pElem, "Allocated");
         CHECKMEMELEM (pMemBlk, pElem);
         CHECKMEMBLOCK (pMemHeap, pMemBlk);
         break;
      }
   }

   /* If not successful, look for empty elements in existing blocks */

   if (0 == mem_p) {
      for (pMemLink = *ppMemLink; pMemLink != 0; pMemLink = pMemLink->pnext) {
         if (pMemLink->blockType & RTMEMRAW) continue;
         
         pMemBlk = (OSMemBlk*) pMemLink->pMemBlk;

         if (nunits <= (ASN1UINT)pMemBlk->freeMem) {
            OSMemElemDescr* pElem = GET_FREE_ELEM(pMemBlk), *pPrevFree = 0;

            RTMEMDIAG2 
               ("memHeapAlloc: try to reuse empty elems in "
                "pMemBlk = 0x%x..\n", pMemBlk);

            while (pElem != 0) {
               if (ISFREE (pElem)) { 
                  if (nunits <= (ASN1UINT)pElem->nunits) {
                     RTMEMDIAG3 
                        ("memHeapAlloc: "
                         "found an exisiting free element 0x%x, size %d\n", 
                        pElem, (pElem->nunits * 8u));
                     
                     if (pMemBlk->freeElemOff == 
                         QOFFSETOF (pElem, pMemBlk->data) + 1) 
                     {
                        
                        /* modify the pMemBlk->freeElemOff value if necsry */

                        OSMemElemDescr* nextFree = GET_NEXT_FREE (pElem);
                        FORCE_SET_FREE_ELEM (pMemBlk, nextFree); 
                     }
                     else if (pPrevFree != 0) {
                        OSMemElemDescr* pNextFree = GET_NEXT_FREE (pElem);
                        if (pNextFree != 0)
                           pPrevFree->u.nextFreeOff = QOFFSETOF (pNextFree, 
                              pPrevFree);
                        else
                           pPrevFree->u.nextFreeOff = 0;
                     } 

                     CLEAR_FREE (pElem);

                     /* set beginOff value */

                     pElem->u.beginOff = QOFFSETOF (pElem, pMemBlk->data);
                     
                     pMemBlk->freeMem -= pElem->nunits;

                     CHECKMEMELEM (pMemBlk, pElem);
                     CHECKMEMBLOCK (pMemHeap, pMemBlk);
                     
                     mem_p = memHeapRealloc 
                        (ppvMemHeap, pElem + 1, nunits * 8u);

                     FILLNEWMEM (mem_p, nunits * 8u);
                     TRACEMEMELEM(pMemBlk, pElem, "Allocated");
                     break;
                  }
               }
               pPrevFree = pElem;
               pElem = GET_NEXT_FREE (pElem);
            }
            if (mem_p != 0) break;
         }
      }   
   }

   /* If not successful, malloc a new block and alloc from it */

   if (!mem_p) {
      ASN1UINT allocSize, dataUnits;
      ASN1OCTET* pmem;
      register ASN1UINT defBlkSize = pMemHeap->defBlkSize;

      RTMEMDIAG1 ("memHeapAlloc: alloc block..\n");

      allocSize = (ASN1UINT) ((((ASN1UINT)nunits) * 8u) + 
         sizeof (OSMemBlk) + sizeof (OSMemElemDescr));
      allocSize = (ASN1UINT) (allocSize < defBlkSize) ? defBlkSize : 
         ((allocSize + defBlkSize - 1) / defBlkSize * defBlkSize);
      dataUnits = (ASN1UINT)((allocSize - sizeof (OSMemBlk)) >> 3u);
      if (dataUnits >= (1u<<16)) {
         dataUnits = (ASN1UINT)((1u<<16) - 1);
         allocSize = (ASN1UINT)
            ((((ASN1UINT)dataUnits) * 8u) + sizeof (OSMemBlk));
      }  

      pmem = (ASN1OCTET*) malloc (allocSize + sizeof (OSMemLink));
      if (0 != pmem) {
         OSMemElemDescr* pElem;

         pMemBlk = (OSMemBlk*) (pmem + sizeof (OSMemLink));
         pElem = (OSMemElemDescr*)&pMemBlk->data[0];

         /* set vars in heap */
         pMemHeap->usedUnits += dataUnits;
         pMemHeap->usedBlocks++;

         mem_p = (void*) (pElem + 1);
         pElem->nunits = (ASN1USINT)nunits;
         pElem->flags = 0;
         pElem->prevOff = 0;
         pElem->u.beginOff = QOFFSETOF (pElem, pMemBlk->data);

         /* sizeof (OSMemElemDescr) == 1 unit */
         pMemBlk->free_x = (ASN1USINT)(nunits + 1); 

         pMemBlk->freeMem = 0;
         pMemBlk->nunits = (ASN1USINT)dataUnits;
         SET_LAST_ELEM (pMemBlk, pElem);
         pMemBlk->freeElemOff = 0;
         pMemBlk->nsaved = 0;

         memHeapAddBlock (ppMemLink, pMemBlk, RTMEMSTD | RTMEMLINK);

         FILLNEWMEM (mem_p, nunits * 8u);
         TRACEMEMELEM(pMemBlk, pElem, "Allocated");
         CHECKMEMELEM (pMemBlk, pElem);
         CHECKMEMBLOCK (pMemHeap, pMemBlk);
      }
   }
   RTMEMDIAG2 ("memHeapAlloc: pMemBlk = 0x%x\n", pMemBlk);
   RTMEMDIAG2 ("memHeapAlloc: pMemBlk->free_x = %d\n", pMemBlk->free_x);
   RTMEMDIAG2 ("memHeapAlloc: pMemBlk->size = %d\n", pMemBlk->nunits * 8u);
   RTMEMDIAG2 ("memHeapAlloc: mem_p = 0x%x\n", mem_p);

   return (mem_p);
}

void* memHeapAllocZ (void** ppvMemHeap, int nbytes)
{
   void* ptr = memHeapAlloc (ppvMemHeap, nbytes);
   if (0 != ptr) memset (ptr, 0, nbytes);
   return ptr;
}

void memHeapFreePtr (void** ppvMemHeap, void* mem_p) 
{
   OSMemHeap* pMemHeap;
   OSMemLink** ppMemLink;
   OSMemElemDescr* pElem;
   OSMemBlk* pMemBlk;
   OSMemLink* pMemLink, *pPrevMemLink = 0;

   RTMEMDIAG2 ("memHeapFreePtr: freeing mem_p = 0x%x\n", mem_p);

   if (mem_p == 0 || ppvMemHeap == 0 || *ppvMemHeap == 0) return;

   pMemHeap = *(OSMemHeap**)ppvMemHeap;
   ppMemLink = &pMemHeap->phead;

   /* look for chain of RAW blocks first */

   for (pMemLink = *ppMemLink; pMemLink != 0; pMemLink = pMemLink->pnextRaw) {
      if ((pMemLink->blockType & RTMEMRAW) &&
           pMemLink->pMemBlk == mem_p) 
      {
         if(pMemLink->pnext != 0) {
            pMemLink->pnext->pprev = pMemLink->pprev;
         }
         if(pMemLink->pprev != 0) {
            pMemLink->pprev->pnext = pMemLink->pnext;
         }
         else { /* head */
            *ppMemLink = pMemLink->pnext;
         }
         if (pPrevMemLink != 0)
            pPrevMemLink->pnextRaw = pMemLink->pnextRaw;
         else if (*ppMemLink != 0 && (*ppMemLink)->pnextRaw == 0 && 
            *ppMemLink != pMemLink->pnextRaw)
         {
            (*ppMemLink)->pnextRaw = pMemLink->pnextRaw;
         }
         if ((pMemLink->blockType & RTMEMLINK) && 
             (pMemLink->blockType & RTMEMMALLOC))
         {
            free (pMemLink);
         }
         else {
            if (pMemLink->blockType & RTMEMMALLOC)
               free (pMemLink->pMemBlk);
            free (pMemLink);
         }
         return;
      }
      pPrevMemLink = pMemLink;
   }

   pElem = (OSMemElemDescr*) (((char*)mem_p) - sizeof (OSMemElemDescr));
   pMemBlk = GET_MEMBLK (pElem);

   CHECKMEMELEM (pMemBlk, pElem);
   CHECKMEMBLOCK(pMemHeap, pMemBlk);

   if (ISFREE (pElem)) { /* already freed! */
      RTMEMDIAG2 ("memHeapFreePtr: "
                  "the element 0x%x is already freed!\n", pElem);
      return;   
   }

   if (ISSAVED (pElem)) {
      CLEAR_SAVED (pMemBlk, pElem);
      if (pMemBlk->nsaved == 0)
         pMemBlk->plink->blockType &= (~RTMEMSAVED);
   }

   TRACEMEMELEM(pMemBlk, pElem, "Freed");
   CHECKMEMELEM (pMemBlk, pElem);
   CHECKMEMBLOCK(pMemHeap, pMemBlk);

   RTMEMDIAG2 ("memHeapFreePtr: pMemBlk = 0x%x\n", pMemBlk);
   RTMEMDIAG2 ("memHeapFreePtr: pMemBlk->size = %d\n", pMemBlk->nunits * 8u);

   if (ISLAST (pElem)) { /* is it the last? */
      OSMemElemDescr* pPrevElem = GETPREV (pElem);
      
      CHECKMEMELEM (pMemBlk, pPrevElem);

      pMemBlk->free_x -= (pElem->nunits + 1);

      FILLFREEMEM (&pMemBlk->data [pMemBlk->free_x * 8u], 
         (pElem->nunits + 1) * 8u);

      if (pPrevElem != 0 && ISFREE (pPrevElem)) {
         OSMemElemDescr* pFreeElem;

         pMemBlk->free_x -= (pPrevElem->nunits + 1);
         pMemBlk->freeMem -= pPrevElem->nunits;
         SET_LAST_ELEM (pMemBlk, GETPREV (pPrevElem));
         
         /* wasn't it the last elem in block? */
         if (pMemBlk->lastElemOff != 0) { 
            
            /* correct nextFreeOff for previous free element */

            pFreeElem = GET_FREE_ELEM (pMemBlk);
            if (pFreeElem == pPrevElem) {
               pMemBlk->freeElemOff = 0; /* it was the last free elem */
            }
            else {
               OSMemElemDescr* pNextFree = 0;
               
               while (pFreeElem < pPrevElem) {
                  pNextFree = pFreeElem;
                  pFreeElem = GET_NEXT_FREE (pFreeElem);
               }
               pNextFree->u.nextFreeOff = 0;
            }
         }
      }
      else {
         SET_LAST_ELEM (pMemBlk, pPrevElem);
      }

      RTMEMDIAG2 ("memHeapFreePtr: pMemBlk->free_x = %d\n", pMemBlk->free_x);

      /* The question is: do we really want to get rid of the   */
      /* block or should we keep it around for reuse?           */
      if (pMemBlk->lastElemOff == 0) { /* was it the last elem in block? */
         
         if ((pMemHeap->flags & RT_MH_DONTKEEPFREE) ||
             (pMemHeap->keepFreeUnits > 0 && 
              pMemHeap->freeUnits + pMemBlk->nunits > pMemHeap->keepFreeUnits))
         {
            ASN1OCTET blockType = pMemBlk->plink->blockType;

            /* we may free the block */

            pMemHeap->usedUnits -= pMemBlk->nunits;
            pMemHeap->usedBlocks --;

            if(pMemBlk->plink->pnext != 0) {
               pMemBlk->plink->pnext->pprev = pMemBlk->plink->pprev;
            }
            if(pMemBlk->plink->pprev != 0) {
               pMemBlk->plink->pprev->pnext = pMemBlk->plink->pnext;
            }
            else { /* head */
               if (pMemBlk->plink->pnext != 0 && 
                   !(pMemBlk->plink->pnext->blockType & RTMEMRAW))
               {
                  pMemBlk->plink->pnext->pnextRaw = (*ppMemLink)->pnextRaw;
               }
               *ppMemLink = pMemBlk->plink->pnext;
            }
            FILLFREEMEM (pMemBlk->plink, sizeof (*pMemBlk->plink));
            FILLFREEMEM (pMemBlk->data, (pMemBlk->nunits * 8u));
         
            free (pMemBlk->plink);
            
            if (!(blockType & RTMEMLINK)) {
               FILLFREEMEM (pMemBlk, sizeof (*pMemBlk));
               free (pMemBlk);
            }
            RTMEMDIAG2 ("memHeapFreePtr: pMemBlk = 0x%x was freed\n", pMemBlk);
         }
         else {
            /* reset pMemBlk for re-usage */
            pMemBlk->free_x = 0;
            pMemBlk->freeElemOff = 0;
            pMemBlk->lastElemOff = 0;
            pMemBlk->freeMem = 0;
            pMemBlk->nsaved = 0;
            pMemHeap->freeUnits += pMemBlk->nunits;
            pMemHeap->freeBlocks ++;
         }
      }
      else {
         SET_LAST (GET_LAST_ELEM (pMemBlk));
         FILLFREEMEM (((char*) &pMemBlk->data[0]) + (pMemBlk->free_x * 8u), 
                      (pMemBlk->nunits - pMemBlk->free_x) * 8u);
         CHECKMEMBLOCK (pMemHeap, pMemBlk);
      }
   }
   else { /* mark as free elem inside the block */
      CHECKMEMBLOCK (pMemHeap, pMemBlk);

      SET_FREE_ELEM(pMemBlk, pElem);

      pMemBlk->freeMem += pElem->nunits;
      RTMEMDIAG2 ("memHeapFreePtr: element 0x%x marked as free.\n", pElem);

      /* try to unite free blocks, if possible */
      if (!ISFIRST (pElem)) {
         if (ISFREE (GETPREV (pElem))) {
            OSMemElemDescr* prevelem_p = GETPREV (pElem);
         
            /* +1 because the OSMemElemDescr has size ONE unit (8 bytes) */
            prevelem_p->nunits += pElem->nunits + 1; 

            pElem = prevelem_p;
            pMemBlk->freeMem ++; /* sizeof (OSMemElemDescr) == 1 */
         }
         else {
            /* look for nearest previous free block to correct nextFreeOff */
         
            OSMemElemDescr* prevelem_p = pElem;
         
            do {
               prevelem_p = GETPREV (prevelem_p);
            }
            while (prevelem_p && !ISFREE (prevelem_p));

            if (prevelem_p != 0) {
               OSMemElemDescr* pNextFree =  GET_NEXT_FREE (prevelem_p);
               if (pNextFree != 0) 
                  pElem->u.nextFreeOff = QOFFSETOF (pNextFree, pElem);
               else
                  pElem->u.nextFreeOff = 0;
               prevelem_p->u.nextFreeOff = QOFFSETOF (pElem, prevelem_p);
         
               CHECKMEMELEM (pMemBlk, prevelem_p);
            }
         }
      }
      if (!ISLAST (pElem) && ISFREE (GETNEXT (pElem))) {
         OSMemElemDescr* nextelem_p = GETNEXT (pElem);
         
         /* +1 because the OSMemElemDescr has size ONE unit (8 bytes) */
         pElem->nunits += nextelem_p->nunits + 1; 

         if (nextelem_p->u.nextFreeOff == 0)
            pElem->u.nextFreeOff = 0;
         else
            pElem->u.nextFreeOff = 
               QOFFSETOF (GET_NEXT_FREE (nextelem_p), pElem);
         pMemBlk->freeMem ++;
      }

      /* correct the prevOff field of next element */
      if (!ISLAST (pElem)) {  
         OSMemElemDescr* nextelem_p = GETNEXT (pElem);
         nextelem_p->prevOff = QOFFSETOF (nextelem_p, pElem);
      }

      CHECKMEMELEM (pMemBlk, pElem);
      FILLFREEMEM (pElem + 1, (pElem->nunits * 8u));
      CHECKMEMELEM (pMemBlk, pElem);
      CHECKMEMBLOCK (pMemHeap, pMemBlk);
   }
} 

static void initNewFreeElement (OSMemBlk* pMemBlk, 
   OSMemElemDescr* pNewElem, OSMemElemDescr* pElem) 
{
   OSMemElemDescr *pNextElem, *pPrevElem = 0;

   /* create new free element on the freed place */

   pNewElem->flags = 0;
   SET_FREE (pNewElem);

   pNewElem->prevOff = QOFFSETOF (pNewElem, pElem);

   if (pMemBlk->freeElemOff != 0 && 
      pMemBlk->freeElemOff < QOFFSETOF (pElem, pMemBlk->data) + 1)
   {
      /* look for nearest previous free block to correct its nextFreeOff */
      
      pPrevElem = pElem;

      do {
         pPrevElem = GETPREV (pPrevElem);
      }
      while (pPrevElem && !ISFREE (pPrevElem));
   }
   if (pPrevElem != 0) { /* if it is not first free element... */

      /* correct nextFreeOff for prev free element */
      
      pPrevElem->u.nextFreeOff = QOFFSETOF (pNewElem, pPrevElem);
   }
   else {  /* if it is first free element in the block */
      FORCE_SET_FREE_ELEM (pMemBlk, pNewElem);
   }
   
   pNextElem = GETNEXT (pNewElem);
   if (ISFREE (pNextElem)) {
      
      /* if the next elem is free, then unite them together */

      pNewElem->nunits += pNextElem->nunits + 1;
      if (pNextElem->u.nextFreeOff != 0)
         pNewElem->u.nextFreeOff = QOFFSETOF (GET_NEXT_FREE (pNextElem), 
            pNewElem);
      else
         pNewElem->u.nextFreeOff = 0;
      pMemBlk->freeMem++; /* +1 because space for MemElemDescr is freed now */
      pNextElem = GETNEXT (pNewElem);
   }
   pNextElem->prevOff = QOFFSETOF (pNextElem, pNewElem);

   if (pMemBlk->freeElemOff != 0) {

      /* look for the next nearest free elem */

      pNextElem = GETNEXT (pNewElem);
      while (pNextElem != 0 && !ISFREE (pNextElem))
         pNextElem = GETNEXT (pNextElem);

      /* set nextFreeOff for new element */
   
      if (pNextElem != 0)
         pNewElem->u.nextFreeOff = QOFFSETOF (pNextElem, pNewElem);
      else
         pNewElem->u.nextFreeOff = 0;
   }
   else
      pNewElem->u.nextFreeOff = 0;

}

void* memHeapRealloc (void** ppvMemHeap, void* mem_p, int nbytes_)
{
   OSMemHeap* pMemHeap;
   OSMemLink** ppMemLink;
   OSMemBlk* pMemBlk;
   OSMemElemDescr* pElem;
   OSMemLink* pMemLink, *pPrevMemLink = 0;
   void *newMem_p;
   unsigned nbytes, nunits;

   /* if mem_p == NULL - do memAlloc */

   if (ppvMemHeap == 0 || *ppvMemHeap == 0) return 0;

   if (mem_p == 0) {
      return memHeapAlloc (ppvMemHeap, nbytes_);
   }

   pMemHeap = *(OSMemHeap**)ppvMemHeap;
   ppMemLink = &pMemHeap->phead;

   /* look for chain of RAW blocks first */

   for (pMemLink = *ppMemLink; pMemLink != 0; pMemLink = pMemLink->pnextRaw) {
      if ((pMemLink->blockType & RTMEMRAW) &&
           pMemLink->pMemBlk == mem_p) 
      {
         if (pMemLink->blockType & RTMEMMALLOC)
            if (realloc != 0) 
               pMemLink->pMemBlk = realloc (pMemLink->pMemBlk, nbytes_);
            else {
               /* use malloc/memcpy/free sequence instead of realloc */
               ASN1OCTET* newBuf;
               int oldSize = *(int*)(((char*)pMemLink) + sizeof (OSMemLink));

               if (oldSize == -1) return 0;
               newBuf = (ASN1OCTET*)malloc (nbytes_);
               memcpy (newBuf, pMemLink->pMemBlk, ASN1MIN (oldSize, nbytes_));
               free (pMemLink->pMemBlk);
               pMemLink->pMemBlk = newBuf;
            }
         else 
            return 0;
         *(int*)(((char*)pMemLink) + sizeof (OSMemLink)) = nbytes_;
         return pMemLink->pMemBlk;
      }
      pPrevMemLink = pMemLink;
   }

   /* Round number of bytes to nearest 8-byte boundary */

   nbytes = ((unsigned)(nbytes_ + 7)) & (~7);
   nunits = nbytes >> 3;

   pElem = (OSMemElemDescr*) (((char*)mem_p) - sizeof (OSMemElemDescr));

   RTMEMDIAG4 
      ("memHeapRealloc: mem_p = 0x%x, old size = %d, new nbytes = %d\n", 
       mem_p, pElem->nunits * 8u, nbytes);

   if ((unsigned)pElem->nunits == nunits)
      return mem_p;

   pMemBlk = GET_MEMBLK (pElem);

   CHECKMEMELEM (pMemBlk, pElem);
   CHECKMEMBLOCK(pMemHeap, pMemBlk);

   if ((unsigned)pElem->nunits < nunits) { /* expanding */
   
      if (nunits - pElem->nunits <= (unsigned)pMemBlk->nunits) {

         /* Try to expand the existing element in the existing block */

         if (ISLAST (pElem)) { /* if the last element in the block */
         
            /* if the free space in the block is enough */
         
            if ((int)(nunits - pElem->nunits) <= 
                (int)(pMemBlk->nunits - pMemBlk->free_x)) 
            { 
               pMemBlk->free_x += nunits - pElem->nunits;
               pElem->nunits = (ASN1USINT)nunits;

               RTMEMDIAG1 ("memHeapRealloc: memory element is expanded.\n");
               
               FILLNEWMEM (&pMemBlk->data [(pMemBlk->free_x - 
                  (nunits - pElem->nunits)) * 8u], 
                  (nunits - pElem->nunits) * 8u);
               
               TRACEMEMELEM (pMemBlk, pElem, "Reallocated");
               CHECKMEMELEM (pMemBlk, pElem);
               CHECKMEMBLOCK (pMemHeap, pMemBlk);

               return (mem_p);
            } 
         }
         else {
            OSMemElemDescr* pNextElem, *pFreeElem; 
            unsigned sumSize = pElem->nunits, freeMem = 0;
         
            RTMEMDIAG1 ("memHeapRealloc: look for free element after "
               "current block.\n");

            /* look for free element after pElem */

            pNextElem = GETNEXT (pElem);
            if (ISFREE (pNextElem)) {
               /* +1 'cos sizeof (OSMemElemDescr) == 1 unit */
               sumSize += pNextElem->nunits + 1; 
               freeMem++;
            }
            
            if (sumSize >= nunits) {

               RTMEMDIAG1 ("memHeapRealloc: reuse free element.\n");

               if (ISFREE (pNextElem)) {
                  pFreeElem = GET_FREE_ELEM (pMemBlk);
                  if (pFreeElem == pNextElem) {
                     FORCE_SET_FREE_ELEM (pMemBlk, GET_NEXT_FREE (pNextElem));
                  }
                  else if (pFreeElem < pElem) {
                     
                     /* look for previous free elem to correct nextFreeOff */

                     for (; pFreeElem != 0 && pFreeElem < pNextElem;) {
                        OSMemElemDescr* pNextFreeElem = 
                           GET_NEXT_FREE (pFreeElem);
                        if (pNextFreeElem == pNextElem) {
                           if (pNextElem->u.nextFreeOff != 0)
                              pFreeElem->u.nextFreeOff = QOFFSETOF 
                                 (GET_NEXT_FREE (pNextElem), pFreeElem);
                           else
                              pFreeElem->u.nextFreeOff = 0;
                           CHECKMEMELEM (pMemBlk, pFreeElem);
                           break;
                        }
                        pFreeElem = pNextFreeElem;
                     }
                  }
               }

               /* reuse empty elements after the pElem */
               
               pMemBlk->freeMem += freeMem;
     
               if (sumSize - nunits > 1) {
                  OSMemElemDescr* pNewElem;
               
                  /* if sumSize is too large, then create new empty element */

                  pNewElem = (OSMemElemDescr*) 
                     (((char*)(pElem + 1)) + nbytes);
                  pNewElem->nunits = (ASN1USINT)(sumSize - nunits - 1);

                  initNewFreeElement (pMemBlk, pNewElem, pElem);

                  pMemBlk->freeMem--; /* sizeof (OSMemElemDescr) == 1 unit */
                  pMemBlk->freeMem -= (nunits - pElem->nunits);
                  pElem->nunits = (ASN1USINT)nunits;
               }
               else {
                  pMemBlk->freeMem -= (sumSize - pElem->nunits);
                  pElem->nunits = (ASN1USINT)sumSize;

                  /* modify the prevOff of the next elem */

                  pNextElem = GETNEXT (pElem);
                  if (pNextElem != 0)
                     pNextElem->prevOff = QOFFSETOF (pNextElem, pElem);
               }
               
               TRACEMEMELEM (pMemBlk, pElem, "Reallocated");
               CHECKMEMELEM (pMemBlk, pElem);
               CHECKMEMELEM (pMemBlk, (!ISLAST (pElem)) ? GETNEXT (pElem) : 0);
               CHECKMEMBLOCK (pMemHeap, pMemBlk);
               return (mem_p);
            }
         }
      }

      /* If not successful, allocate a new element and move the data into it */

      RTMEMDIAG1 ("memHeapRealloc: allocate new memory...\n");

      CHECKMEMHEAP (pMemHeap);

      newMem_p = memHeapAlloc (ppvMemHeap, nbytes);
      
      if (newMem_p == 0)
         return 0;

      /* if the old memory block is marked as saved then mark the new block
         as saved as well. */

      if (ISSAVED (pElem)) 
         memHeapMarkSaved (ppvMemHeap, newMem_p, TRUE);

      CHECKMEMHEAP (pMemHeap);

      memcpy (newMem_p, mem_p, (((ASN1UINT)pElem->nunits) * 8u));

      /* free old element */

      RTMEMDIAG1 ("memHeapRealloc: free old pointer...\n");

      memHeapFreePtr (ppvMemHeap, mem_p);

      CHECKMEMHEAP (pMemHeap);

      return (newMem_p);
   }
   else { /* shrinking */
      RTMEMDIAG1 ("memHeapRealloc: shrinking ...\n");
      
      /* just free the pointer, if nbytes == 0 */

      if (nbytes == 0) {
         RTMEMDIAG1 ("memHeapRealloc: free pointer...\n");
         memHeapFreePtr (ppvMemHeap, mem_p);
         return (NULL);
      }

      /* do not shrink, if size diff is too small */

      /* sizeof (OSMemElemDescr) == 1 unit */
      if (pElem->nunits - nunits > 1) { 
         
         /* if it is the last element in the block, then just change the size 
            and free_x. */

         if (ISLAST (pElem)) {
            pMemBlk->free_x -= (pElem->nunits - nunits);

            FILLFREEMEM (&pMemBlk->data [pMemBlk->free_x * 8u], 
               (pElem->nunits - nunits) * 8u);
         }
         else {
            OSMemElemDescr* pNewElem;

            /* create new free element on the freed place */

            pNewElem = (OSMemElemDescr*) (((char*)(pElem + 1)) + nbytes);

            /* sizeof (OSMemElemDescr) == 1 unit */
            pNewElem->nunits = (ASN1USINT)(pElem->nunits - nunits - 1); 
            
            initNewFreeElement (pMemBlk, pNewElem, pElem);
            
            pMemBlk->freeMem += (pElem->nunits - nunits) - 1;
         }
         pElem->nunits = (ASN1USINT)nunits;
         
         TRACEMEMELEM (pMemBlk, pElem, "Reallocated");
         CHECKMEMELEM (pMemBlk, pElem);
         CHECKMEMELEM (pMemBlk, (!ISLAST (pElem)) ? GETNEXT (pElem) : pElem);
         CHECKMEMBLOCK (pMemHeap, pMemBlk);
      }
      return (mem_p);
   }
}

/* Clears heap memory (frees all memory, reset all heap's variables) */
void memHeapFreeAll (void** ppvMemHeap)
{
   OSMemHeap* pMemHeap;
   OSMemLink* pMemLink;
   OSMemLink* pMemLink2;

   if (ppvMemHeap == 0 || *ppvMemHeap == 0) return;
   pMemHeap = *(OSMemHeap**)ppvMemHeap;

   pMemLink = pMemHeap->phead;
   RTMEMDIAG2 ("memHeapFreeAll: pMemHeap = 0x%x\n", pMemHeap);

   TRACEFREE (pMemHeap, "memHeapFreeAll\n\n");
   CHECKMEMHEAP (pMemHeap);

   /* Release any dynamic memory blocks that may have been allocated */

   while (pMemLink) {
      pMemLink2 = pMemLink;
      pMemLink  = pMemLink2->pnext;

      RTMEMDIAG3 ("memHeapFreeAll: pMemLink2 = 0x%x, pMemLink = 0x%x\n", 
                  pMemLink2, pMemLink);
      
#ifdef _MEMDEBUG
      if (pMemLink2->blockType & RTMEMSTD) {
         OSMemBlk* pMemBlk = (OSMemBlk*) pMemLink2->pMemBlk;
         FILLFREEMEM (pMemBlk->data, (pMemBlk->nunits * 8u));
         FILLFREEMEM (pMemBlk, sizeof (*pMemBlk));
      }
#endif
      if (!(pMemLink2->blockType & RTMEMSAVED)) {
         OSMemBlk* pMemBlk = (OSMemBlk*) pMemLink2->pMemBlk;

         /* unlink block first */

         if(pMemLink2->pnext != 0) {
            pMemLink2->pnext->pprev = pMemLink2->pprev;
         }
         if(pMemLink2->pprev != 0) {
            pMemLink2->pprev->pnext = pMemLink2->pnext;
         }
         else { /* head */
            pMemHeap->phead = pMemLink2->pnext;
         }

         /* correct heap's variables */

         pMemHeap->usedUnits -= pMemBlk->nunits;

         if (pMemBlk->free_x == 0)
            pMemHeap->freeBlocks --;
         else
            pMemHeap->usedBlocks --;

         /* free link and block */

         if (((pMemLink2->blockType & RTMEMSTD) || 
              (pMemLink2->blockType & RTMEMMALLOC)) &&
              !(pMemLink2->blockType & RTMEMLINK)) 
            free (pMemLink2->pMemBlk);
         free (pMemLink2);
      }
   }
}

/* increments internal refCnt. use memHeapRelease to decrement and release */
void memHeapAddRef (void** ppvMemHeap)
{
   OSMemHeap* pMemHeap;

   if (ppvMemHeap == 0 || *ppvMemHeap == 0) return;
   pMemHeap = *(OSMemHeap**)ppvMemHeap;
   pMemHeap->refCnt++;
}

/* Frees all memory and heap structure as well (if was allocated) */
void memHeapRelease (void** ppvMemHeap)
{
   OSMemHeap** ppMemHeap = (OSMemHeap**)ppvMemHeap;

   if (ppMemHeap != 0 && *ppMemHeap != 0 && --(*ppMemHeap)->refCnt == 0) {
      OSMemLink* pMemLink, *pMemLink2;

      memHeapFreeAll (ppvMemHeap);

      /* if there are RTMEMSAVED blocks - release memory for links only */

      pMemLink = (*ppMemHeap)->phead;
      while (pMemLink) {
         pMemLink2 = pMemLink;
         pMemLink  = pMemLink2->pnext;

         free (pMemLink2);
      }

      if ((*ppMemHeap)->flags & RT_MH_FREEHEAPDESC)
         free (*ppMemHeap);

      *ppMemHeap = 0;
   }
}

/* This function is used for marking memory block as "saved". It means
 * that the memory block containing the specified memory pointer won't be
 * freed after calls to memHeapFreeAll/memHeapReset. User is responsible 
 * for freeing the marked memory block by call to memFreeBlock */

void* memHeapMarkSaved (void** ppvMemHeap, const void* mem_p, 
                        ASN1BOOL saved) 
{
   OSMemHeap* pMemHeap;
   OSMemLink* pMemLink;
   ASN1UINT nsaved = 1;

   RTMEMDIAG2 ("memHeapMarkSaved: for mem_p = 0x%x\n", mem_p);

   if (ppvMemHeap == 0 || *ppvMemHeap == 0 || mem_p == 0) 
      return 0;

   pMemHeap = *(OSMemHeap**)ppvMemHeap;
   pMemLink = pMemHeap->phead;

   /* look for chain of RAW blocks first */

   for (; pMemLink != 0; pMemLink = pMemLink->pnextRaw) {
      if ((pMemLink->blockType & RTMEMRAW) &&
           pMemLink->pMemBlk == mem_p) 
      {
         break;
      }
   }
   if (pMemLink == 0) {
      OSMemElemDescr* pElem;
      OSMemBlk* pMemBlk;

      /* gain the MemLink from pointer */

      pElem = (OSMemElemDescr*) (((char*)mem_p) - sizeof (OSMemElemDescr));

      if (ISFREE (pElem)) { /* already freed! */
         RTMEMDIAG2 ("memHeapMarkSaved: the element 0x%x is already free!\n", 
            pElem);
         return 0;   
      }

      if ((ISSAVED (pElem) && !saved) || (!ISSAVED (pElem) && saved)) {

         pMemBlk = GET_MEMBLK (pElem);

         CHECKMEMELEM (pMemBlk, pElem);
         CHECKMEMBLOCK(pMemHeap, pMemBlk);

         pMemLink = pMemBlk->plink;

         if (saved) 
            SET_SAVED (pMemBlk, pElem);
         else
            CLEAR_SAVED (pMemBlk, pElem);
         nsaved = pMemBlk->nsaved;
      }
      else
         return 0;
   }
   if (saved && nsaved > 0) 
      pMemLink->blockType |= RTMEMSAVED;
   else if (nsaved == 0)
      pMemLink->blockType &= (~RTMEMSAVED);
   return pMemLink->pMemBlk;
}

/* This function will set the free index in all blocks to zero thereby  */
/* allowing the blocks to be reused (ED, 3/17/2002)..                   */

void memHeapReset (void** ppvMemHeap)
{
   OSMemHeap* pMemHeap;
   OSMemLink *pMemLink;

   if (ppvMemHeap == 0 || *ppvMemHeap == 0) return;
   pMemHeap = *(OSMemHeap**)ppvMemHeap;

   pMemLink = pMemHeap->phead;
   TRACEFREE (pMemHeap, "memHeapReset\n\n");
   while (pMemLink) {
      if (!(pMemLink->blockType & RTMEMSAVED)) {
         if (pMemLink->blockType & RTMEMSTD) {
            OSMemBlk* pMemBlk = (OSMemBlk*) pMemLink->pMemBlk;
            if (pMemBlk->free_x != 0) {
               pMemHeap->freeUnits += pMemBlk->nunits;
               pMemHeap->freeBlocks ++;
            }
            pMemBlk->free_x = 0;
            pMemBlk->freeElemOff = 0;
            pMemBlk->lastElemOff = 0;
            pMemBlk->freeMem = 0;
            FILLFREEMEM (pMemBlk->data, pMemBlk->nunits * 8u);
         }
         else if (pMemLink->blockType & RTMEMRAW) {
            /* if RAW block - free it */
            memHeapFreePtr (ppvMemHeap, pMemLink->pMemBlk);
         }
      }
      pMemLink = pMemLink->pnext;
   }
}

/* add memory block to list */

static OSMemLink* memHeapAddBlock (OSMemLink** ppMemLink, 
                                     void* pMemBlk, int blockType)
{
   OSMemLink* pMemLink;

   /* if pMemBlk has RTMEMLINK flags it means that it is allocated 
    * cooperatively with OSMemLink, and we don't need to do additional
    * allocations for it. Just use pointer's arithemtic. */

   if (blockType & RTMEMLINK) 
      pMemLink = (OSMemLink*) (((ASN1OCTET*)pMemBlk) - sizeof (OSMemLink));
   else {
      pMemLink = (OSMemLink*) malloc (
         sizeof(OSMemLink) + sizeof (int));
      /* An extra integer is necessary to save a size of a RAW memory block
         to perform memRealloc through malloc/memcpy/free */
      *(int*)(((char*)pMemLink) + sizeof (OSMemLink)) = -1;
   }
   if (pMemLink == NULL) 
      return NULL;
   pMemLink->blockType = (ASN1OCTET)blockType;
   pMemLink->pMemBlk = pMemBlk;
   pMemLink->pprev = 0;
   pMemLink->pnext = *ppMemLink;

   if (*ppMemLink != 0) {
      if ((*ppMemLink)->blockType & RTMEMRAW)
         pMemLink->pnextRaw = *ppMemLink;
      else {
         pMemLink->pnextRaw = (*ppMemLink)->pnextRaw;
         (*ppMemLink)->pnextRaw = 0;
      }
   }
   else {
      pMemLink->pnextRaw = 0;
   }

   *ppMemLink = pMemLink; 
   if (pMemLink->pnext != 0)
      pMemLink->pnext->pprev = pMemLink;
   ((OSMemBlk*)pMemBlk)->plink = pMemLink; /*!AB */

   RTMEMDIAG2 ("memHeapAddBlock: pMemLink = 0x%x\n", pMemLink);
   RTMEMDIAG2 ("memHeapAddBlock: pMemLink->pnext = 0x%x\n", pMemLink->pnext);
   RTMEMDIAG2 ("memHeapAddBlock: pMemLink->pprev = 0x%x\n", pMemLink->pprev);

   return pMemLink;
}

int memHeapCheckPtr (void** ppvMemHeap, void* mem_p)
{
   OSMemHeap* pMemHeap;
   OSMemLink* pMemLink;

   RTMEMDIAG2 ("memHeapCheckPtr: for mem_p = 0x%x\n", mem_p);

   if (ppvMemHeap == 0 || *ppvMemHeap == 0 || mem_p == 0) 
      return 0;
   pMemHeap = *(OSMemHeap**)ppvMemHeap;

   pMemLink = pMemHeap->phead;

   for (; pMemLink != 0; pMemLink = pMemLink->pnext) {
      if (pMemLink->blockType & RTMEMRAW) {
         
         /* if RAW block, the pointer should be stored in pMemBlk */

         if (pMemLink->pMemBlk == mem_p) 
            return 1;
      }
      else {
         OSMemBlk* pMemBlk = (OSMemBlk*)pMemLink->pMemBlk;
         
         /* Check, is the pointer inside this memory page */

         if (mem_p > pMemLink->pMemBlk && 
             mem_p < (void*)(((ASN1OCTET*)pMemLink->pMemBlk) + 
                             pMemBlk->nunits * 8u))
         {
            /* Check, is the pointer a correct element of the mem page */

            OSMemElemDescr* pElem = (OSMemElemDescr*) pMemBlk->data;
            for (; pElem != 0; pElem = GETNEXT (pElem)) {
              
               void* curMem_p = (void*) (pElem + 1);
               if (curMem_p == mem_p && !ISFREE (pElem))
                  return 1;
            }
         }
      }
   }
   return 0;
}

void memHeapSetProperty (void** ppvMemHeap, ASN1UINT propId, void* pProp)
{
   OSMemHeap* pMemHeap;

   if (ppvMemHeap == 0) 
      return;

   if (*ppvMemHeap == 0)
      memHeapCreate (ppvMemHeap);

   pMemHeap = *(OSMemHeap**)ppvMemHeap;
   switch (propId) {
      case OSRTMH_PROPID_DEFBLKSIZE:
         pMemHeap->defBlkSize = *(ASN1UINT*)pProp;
         break;
      case OSRTMH_PROPID_SETFLAGS:
         pMemHeap->flags |= ((*(ASN1UINT*)pProp) & (~RT_MH_INTERNALMASK));
         break;
      case OSRTMH_PROPID_CLEARFLAGS:
         pMemHeap->flags &= ((~(*(ASN1UINT*)pProp)) | RT_MH_INTERNALMASK);
         break;
   }
} 

int memHeapCreate (void** ppvMemHeap) 
{
   OSMemHeap* pMemHeap;
   if (ppvMemHeap == 0) return ASN_E_INVPARAM;

   pMemHeap = (OSMemHeap*) g_malloc_func (sizeof (OSMemHeap));
   if (pMemHeap == NULL) return ASN_E_NOMEM;
   memset (pMemHeap, 0, sizeof (OSMemHeap));
   pMemHeap->defBlkSize = g_defBlkSize;
   pMemHeap->refCnt = 1;
   pMemHeap->flags = RT_MH_FREEHEAPDESC;
   *ppvMemHeap = (void*)pMemHeap;
   return ASN_OK;
}
