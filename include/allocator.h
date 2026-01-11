#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>
#include <pthread.h>

/* Alignment */
typedef char ALIGN[16];

/* Block header */
union header {
    struct {
        size_t size;
        unsigned is_free;
        union header *next;
    } s;
    ALIGN stub;
};

typedef union header header_t;

/* Global variables */
extern pthread_mutex_t global_malloc_lock;
extern header_t *head;
extern header_t *tail;

/* Allocator APIs */
void *malloc(size_t size);
void free(void *block);
void *calloc(size_t num, size_t size);
void *realloc(void *block, size_t size);

/* Internal helper */
header_t *get_free_block(size_t size);

#endif
