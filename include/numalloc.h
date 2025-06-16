
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void *malloc_aligned(size_t size, size_t alignment);
void *realloc_aligned(void *ptr, size_t size, size_t alignment);
void free_aligned(void *ptr);

#ifdef __cplusplus
}
#endif


#define NUMALLOC(TYPE, COUNT, ALIGN) malloc_aligned(sizeof(TYPE) * (COUNT), ALIGN)
#define NUMFREE(PTR) aligned_free(PTR)
