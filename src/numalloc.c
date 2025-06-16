#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32
# include <malloc.h>
/*void * _aligned_malloc(size_t size, size_t alignment);
void * _aligned_realloc(void *ptr, size_t size, size_t alignment);
void _aligned_free(void *ptr);
*/
#else
#include <inttypes.h>
typedef uintptr_t malloc_aligned_ULONG_PTR;
#endif

// size : the size of allocated memory
//        The actual size of allocation will be greater than this size.
// alignment : the alignment boundary
void *malloc_aligned(size_t size, size_t alignment){
#ifdef _WIN32
	return (void*)_aligned_malloc(size, alignment);
#else
	void *pa, *ptr;

	//pa=malloc(((size+alignment-1)&~(alignment-1))+sizeof(void *)+alignment-1);

	pa = malloc((size+alignment-1)+sizeof(void*));
	if(!pa){ return NULL; }

	ptr = (void*)( ((malloc_aligned_ULONG_PTR)pa+sizeof(void*)+alignment-1)&~(alignment-1) );
	*((void **)ptr-1) = pa;

	return ptr;
#endif
}
void *realloc_aligned(void *ptr, size_t size, size_t alignment){
#ifdef _WIN32
	return (void*)_aligned_realloc(ptr, size, alignment);
#else
	void *pa;

	pa = realloc(*((void**)ptr-1), (size+alignment-1)+sizeof(void*));
	if(!pa){ return NULL; }

	ptr = (void*)( ((malloc_aligned_ULONG_PTR)pa+sizeof(void*)+alignment-1)&~(alignment-1) );
	*((void **)ptr-1) = pa;

	return ptr;
#endif
}


void free_aligned(void *ptr){
#ifdef _WIN32
	_aligned_free(ptr);
#else
	if(ptr){
		free(*((void **)ptr-1));
	}
#endif
}
