#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <search.h>

void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation)
{
	assert(initialAllocation > 0);
	assert(elemSize > 0);

	v->freefn = freeFn;
	v->elemSize = elemSize;
	v->allocSize = initialAllocation;
	v->logSize = 0;
	v->elems = malloc(initialAllocation * elemSize);

	assert(v->elems != NULL);
}

void VectorDispose(vector *v)
{
	if (v->freefn)
	{
		for (int i = 0; i < v->logSize; i++)
		{
			v->freefn( (char *)v->elems + i * v->elemSize);
		}
	}

	free(v->elems);
}

int VectorLength(const vector *v)
{
	return v->logSize;
}

void *VectorNth(const vector *v, int position)
{ 
	assert(position >= 0 && position < v->logSize);
	
	return ((char*)v->elems + position * v->elemSize);
}

void VectorReplace(vector *v, const void *elemAddr, int position)
{
	assert(position >= 0 && position < v->logSize);
	
	if (v->freefn)
	{
		v->freefn((char*)v->elems + position * v->elemSize);
	}
	
	memcpy((char*)v->elems + position * v->elemSize, elemAddr, v->elemSize);
}


static void growVector(vector *v) 
{
	v->allocSize *= 2;
	v->elems = realloc(v->elems, v->allocSize * v->elemSize);
}

void VectorInsert(vector *v, const void *elemAddr, int position)
{
	assert(position >= 0 && position <= v->logSize);
	assert(elemAddr);

	if (v->logSize == v->allocSize)
	{
		growVector(v);
	}

	int backSpace = ((char*)v->elems + v->logSize * v->elemSize) - ((char*)v->elems + position * v->elemSize);
	memmove((char*)v->elems + (position + 1) * v->elemSize, ((char*)v->elems + position * v->elemSize), backSpace);
	memcpy((char*)v->elems + position * v->elemSize, elemAddr, v->elemSize);

	v->logSize++;
}

void VectorAppend(vector *v, const void *elemAddr)
{
	if (v->logSize == v->allocSize)
	{
		growVector(v);
	}
	
	VectorInsert(v, elemAddr, v->logSize);	
}

void VectorDelete(vector *v, int position)
{
	assert(position >= 0 && position < v->logSize);

	if (v->freefn)
	{
		v->freefn((char*)v->elems + position * v->elemSize);
	}

	int trailingSpace = ((char*)v->elems + v->logSize * v->elemSize) - ((char*)v->elems + (position + 1) * v->elemSize);
	memcpy( (char*)v->elems + position * v->elemSize,
			((char*)v->elems + v->logSize * v->elemSize) - trailingSpace,
			trailingSpace);
		
	v->logSize--;
}

void VectorSort(vector *v, VectorCompareFunction compare)
{
	qsort(v->elems,v->logSize,v->elemSize,compare);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData)
{
	for (int i = 0; i < v->logSize; i++)
	{
		mapFn( ( (char*) v->elems + i * v->elemSize), auxData);
	}
}


static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted)
{

	assert(startIndex >= 0 && startIndex < v->logSize);
	assert(searchFn != NULL);
	assert(key != NULL);

	void* val = NULL;

	if (isSorted)
	{
		val = bsearch(key, ((char*) v->elems + startIndex * v->elemSize), v->logSize - startIndex, v->elemSize, searchFn);
	}
	else
	{
		size_t logicalSize = v->logSize - startIndex;
		val = lfind(key, ((char*) v->elems + startIndex * v->elemSize), &logicalSize, v->elemSize, searchFn);
	}

	if (val)
	{
		return ((char*)val - (char*)v->elems) / v->elemSize;		
	}
	
	return kNotFound;
}
