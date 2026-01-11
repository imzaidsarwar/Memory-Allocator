#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>

typedef char ALIGN[16];

// To prevent two or more threads from concurrently accessing memory, we will put a basic locking mechanism in place.
pthread_mutex_t global_malloc_lock;

union header {
	struct {
		size_t size; // size of block
		unsigned is_free; // indication is block is free or not
		union header *next; // next block if on free list
	} s;
	ALIGN stub; // force alignment of blocks
};
typedef union header header_t;

header_t *head, *tail;

void *malloc(size_t size){
    /*
    void *block;

    block = sbrk(size);

    if((void *)-1 == block)
    {
        return NULL;
    }

    return block;*/

    // total size of which we need to allocate header + block
    size_t total_size;
    
    // actual block
    void* block;
    
    // header of memory block
    header_t* header;

    // size not be zero
    if(!size)
    {
        return NULL;
    }

    // Use the lock
    pthread_mutex_lock(&global_malloc_lock);

    // find free block of given size
    header = get_free_block(size);

    // if free block is avilable
    if(header)
    {
        // mark block not free
        header->s.is_free = 0;

        // Use the lock
        pthread_mutex_unlock(&global_malloc_lock);

        // return the starting pointer of block 
        return (void*)(header+1);
    }

    // If free block is not avilable then we need to expand the heap
    // Calculate the total size of block
    total_size = sizeof(header_t) + size;

    // increment the program break pointer and allocate the memory
    block = sbrk(total_size);

    // block must not be null 
    if((void*)-1 == block)
    {
        // Use the lock
        pthread_mutex_unlock(&global_malloc_lock);

        return NULL;
    }

    // Set the header properties, which we need further to expand the heap
    // header is equal to block
    header = block;

    // set header size
    header->s.size = size;

    // set indication header is free or not
    header->s.is_free = 0;

    // header's next points to NULL because it's top of the heap
    header->s.next = NULL;

    // if head NULL means it's first time.
    if(!head)
    {
        // Now head is equal to 1st header
        head = header;
    }

    // if tail not null means tail point to prev. header we need to update it
    if(tail)
    {
        // Now tail's next point to newly created header
        tail->s.next = header;
    }

    // if tail null means its 1st time 
    // Now tail is equal to newly created header 
    tail = header;

    // Use the lock
    pthread_mutex_unlock(&global_malloc_lock);

    // return the pointer 
    return (void*)(header+1);

}

header_t *get_free_block(size_t size)
{
	header_t *curr = head;

    // traverse the list to find requested size free memory block
	while(curr) {

        // if free memory block of given size is found
		if (curr->s.is_free && curr->s.size >= size)
        {
            // return curr pointer
            return curr;
        }
		
        // move curr pointer forward
		curr = curr->s.next;
	}

    // if there no free block of memory of given size return null
	return NULL;
}


void free(void* block) {

    header_t* header, *temp;
    void *programbreak;

    // block not be null
    if(!block)
    {
        return;
    }

    // Use the lock
    pthread_mutex_lock(&global_malloc_lock);

    // get the header of block which we want to free
    header = (header_t*)block - 1;

    // get current value of program break
    programbreak = sbrk(0);

    // check is block to be freed is at the end of heap
    if(programbreak == (char*)block + header->s.size)
    {

        // if there is only one node
        if(head == tail)
        {
            head = tail = NULL;
        }

        else {

            // tempary pointer to traverse the linked list
            temp = head;

            // Calculate the ammount of memory need to be free
            while(temp) {
                // if we reached the tail
                if(tail == temp->s.next) {
                    
                    // temp node point to null
                    temp->s.next = NULL;
                    
                    // tail is equal to temp
                    tail = temp;
                }

                // move temp node forward
                temp = temp->s.next;
            }
        }

        // Release total memory to OS
        sbrk(0 - sizeof(header_t) - header->s.size);

        // Use the lock
        pthread_mutex_unlock(&global_malloc_lock);

		return;
    }

    // block is not at the end of heap, simply mark the free.
    header->s.is_free = 1;

    // Use the lock
    pthread_mutex_unlock(&global_malloc_lock);

}


void *calloc(size_t num, size_t nsize)
{
	size_t size;
	void *block;

    // sizes not be zero
	if (!num || !nsize)
    {
        return NULL;
    }

    // calculate total size for allocation
	size = num * nsize;

	// check mul overflow 
	if (nsize != size / num)
		return NULL;

    // allocate the memory
	block = malloc(size);

    // memory block not be null
	if (!block)
    {
        return NULL;
    }	
    
    // fill memory block by zero 
	memset(block, 0, size);

    // return block
	return block;
}


void *realloc(void *block, size_t size)
{
	header_t *header;
	void *ret;

    // block and size null
	if (!block || !size)
    {
        // allocate the memory
        return malloc(size);
    }
		
	header = (header_t*)block - 1;

    // If header size is already equal to requested size do nothing
	if (header->s.size >= size)
    {
        // just return block
        return block;
    }
	
    // else allocate the memory
	ret = malloc(size);

	if (ret) {
		
        // copy the block data into newly created block
		memcpy(ret, block, header->s.size);

        // free the older block of memory
		free(block);
	}

	return ret;
}