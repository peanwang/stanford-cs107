#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <search.h>

static void VectorGrow(vector *v){
	v->alloclen *= 2;
	v->elems = realloc(v->elems, v->alloclen*v->elemsize);
}

/*
typedef struct {
	void* elems;
	int elemsize;
	int logicallen;
	int alloclen;
	VectorFreeFunction freefn;
} vector;
*/
void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation){
	assert( elemSize>0);
	assert( initialAllocation>=0);
	if( initialAllocation==0)
		initialAllocation = 4;
	v->elemsize = elemSize;
	v->logicallen = 0;
	v->alloclen = initialAllocation;
	v->elems = malloc( elemSize*v->alloclen);
	assert(v->elems != NULL);
	v->freefn = freeFn;
}

void VectorDispose(vector *v){
	if(v->freefn !=NULL){
		for(int i=0;i<v->logicallen;i++)
			v->freefn((char*)v->elems+i*v->elemsize);
	}
	free(v->elems);
}

int VectorLength(const vector *v){ 
	return v->logicallen; 
}

void *VectorNth(const vector *v, int position){
	assert(position>=0);
	assert(v->logicallen>position);
	return (char*)v->elems+position*v->elemsize;
}

void VectorReplace(vector *v, const void *elemAddr, int position){
	assert(position>=0);		
	assert(v->logicallen>position);
	char* replace = VectorNth(v,position);	
	memcpy(replace, elemAddr, v->elemsize);
}

void VectorInsert(vector *v, const void *elemAddr, int position){
	assert(position>=0);                
	assert(v->logicallen>=position);
	if ( v->logicallen == v->alloclen)
		VectorGrow(v);
	if (v->logicallen == position)
		VectorAppend(v, elemAddr);
	else{
		char *front = VectorNth(v, position);
		char* end = front + v->elemsize;
		int movesize =  v->elemsize*(v->logicallen-position);
		char* buffer = (char*)malloc(movesize);	
		memcpy(buffer, front, movesize);
		memcpy(front, elemAddr, v->elemsize);	
		memcpy(end, buffer, movesize);
		free(buffer);
		v->logicallen++;
	}
}

void VectorAppend(vector *v, const void *elemAddr)
{
	if ( v->logicallen == v->alloclen)
		VectorGrow(v);
	v->logicallen++;
	void *target = VectorNth(v,v->logicallen-1);
	memcpy(target, elemAddr, v->elemsize);
}

void VectorDelete(vector *v, int position){
	assert(position>=0);
	assert(v->logicallen>position);
	char *target = VectorNth(v,position);
	if(v->freefn !=NULL){
		v->freefn(target);
	}
	if(v->logicallen-1 != position){   //the last one 
		int end_size = v->logicallen-position-1;
		for(int i=0;i<end_size;i++){
			char* next = target + v->elemsize;			
			memcpy(target, next, v->elemsize);
			target = next;
		}
	}
	v->logicallen--;
}

void VectorSort(vector *v, VectorCompareFunction compare){
	assert(compare != NULL);
	qsort(v->elems, v->logicallen, v->elemsize, compare);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData){
	assert(mapFn != NULL);
	for(int i=0;i<v->logicallen;i++){
		char *target = (char*)v->elems + i*v->elemsize;		
		mapFn(target, auxData);
	}
}


static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted){
	assert(startIndex>=0);
	assert(v->logicallen>startIndex);
	assert(key != NULL);
	assert(searchFn != NULL);
	char* result;
	char* base = (char*)v->elems + startIndex*v->elemsize;
	if(isSorted)
		result = bsearch(key, base, v->logicallen-startIndex, v->elemsize, searchFn);
	else{
		size_t size = v->logicallen-startIndex;
		result = lfind(key, base, &size, v->elemsize, searchFn);
	}
	return result==NULL ?kNotFound :(result-(char*)v->elems)/v->elemsize;
} 

















