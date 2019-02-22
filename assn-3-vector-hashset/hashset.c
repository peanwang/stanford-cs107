#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>


void HashSetNew(hashset *h, int elemSize, int numBuckets,HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn){
	assert(elemSize>0);
	assert(numBuckets>0);
	assert(hashfn !=NULL);
	assert(comparefn !=NULL);
	h->elemNum = 0;
	h->elemsize = elemSize;
	h->numBuckets = numBuckets;
	h->hashfn= hashfn;
	h->comparefn = comparefn;
	h->freefn = freefn;
	h->bucket = (vector*)malloc(sizeof(vector)*h->numBuckets);
	assert(h->bucket);
	for(int i=0;i<numBuckets;i++)
		VectorNew(h->bucket+i,elemSize,freefn,4);
}


void HashSetDispose(hashset *h){
	if(h->freefn !=NULL){
		for(int i=0;i<h->numBuckets;i++)	
			VectorMap(h->bucket+i, h->freefn, NULL);
	}
	for(int i=0;i<h->numBuckets;i++)	
		VectorDispose(h->bucket+i);	
	free(h->bucket);
}

int HashSetCount(const hashset *h){ 
	return h->elemNum;
}

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData){
	assert(mapfn !=NULL);
	for(int i=0;i<h->numBuckets;i++){
		VectorMap(h->bucket+i, mapfn, auxData);
	}
}


void HashSetEnter(hashset *h, const void *elemAddr){
	assert(elemAddr !=NULL);
	void* result = HashSetLookup(h, elemAddr);
	if(result == NULL){
		int bucket = h->hashfn(elemAddr, h->numBuckets);	
		assert(bucket>=0);
		assert(h->numBuckets>bucket);
		VectorAppend(h->bucket+bucket, elemAddr);	
		h->elemNum++;	
	}
	else{
		memcpy(result, elemAddr, h->elemsize);
	}
}


void *HashSetLookup(const hashset *h, const void *elemAddr){ 
	assert(elemAddr !=NULL);	
	int bucket = h->hashfn(elemAddr, h->numBuckets);	
	assert(bucket>=0);
	assert(h->numBuckets>bucket);	
	int result = VectorSearch(h->bucket+bucket, elemAddr, h->comparefn, 0, false);
	return result==-1? NULL: VectorNth(h->bucket+bucket, result);
}




