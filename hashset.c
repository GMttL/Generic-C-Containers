#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h> 			// DEBUGGING PURPOSES ONLY!!


static void hashsetINIT(hashset *h)
{
	for (int i = 0; i < h->numBuckets; i++)
	{
		VectorNew(h->elems + i, h->elemSize, h->freefn, 1);
	}
}

void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn)
{
	assert(elemSize > 0);
	assert(numBuckets > 0);
	assert(hashfn);
	assert(comparefn);
	
	h->elemSize = elemSize;			// size for the elements that'll be stored in the vector.
	h->numBuckets = numBuckets;		
	h->elems = malloc(numBuckets * sizeof(vector));
	h->hashfn = hashfn;
	h->cmpfn = comparefn;			// for the elements of the vector
	h->freefn = freefn; 			// free function for the elements contained in the vector.
	h->logSize = 0;
	hashsetINIT(h);			// initialise every vector so we don't deal with wild ptrs.
}

void HashSetDispose(hashset *h)
{
	for (int i = 0; i < h->numBuckets; i++)
	{
		VectorDispose(h->elems + i);
	}

	free(h->elems);
}

int HashSetCount(const hashset *h)
{
	return h->logSize;	
}

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData)
{
	assert(mapfn);

	for (int i = 0; i < h->numBuckets; i++)
	{
		VectorMap(h->elems + i, mapfn, auxData);
	}
}

void HashSetEnter(hashset *h, const void *elemAddr)
{
	assert(elemAddr);
	
	int hash = h->hashfn(elemAddr, h->numBuckets);

	assert(hash >= 0 && hash < h->numBuckets);

	vector* vec = h->elems + hash;

	int pos = -1;
	if (VectorLength(vec) != 0)
	{
		pos = VectorSearch(vec, elemAddr, h->cmpfn, 0, false);
	}

	if (pos != -1)
	{
		VectorReplace(vec, elemAddr, pos);
	}
	else
	{
		VectorAppend(vec, elemAddr);
		static int i = 0;
		h->logSize++;
	}

	//VectorSort(vec, elemAddr);			// SEG FAULT at the 369'th call. Can't figure out WHY and I've exhausted all my ideas.
}

void *HashSetLookup(const hashset *h, const void *elemAddr)
{
	assert(elemAddr);

	int hash = h->hashfn(elemAddr, h->numBuckets);
	
	assert(hash >= 0 && hash < h->numBuckets);

	vector* vec = h->elems + hash;
	if (VectorLength(vec) != 0)
	{
		int pos = VectorSearch(vec, elemAddr, h->cmpfn, 0, false);
		return pos == -1 ? NULL : VectorNth(vec, pos);
	}

	return NULL;
}
