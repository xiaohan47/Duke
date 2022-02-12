#include <stdlib.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct block *block_ptr;
struct block{
    size_t block_size;
    void * address;
    block_ptr next;
    block_ptr prev;
};


//Thread Safe malloc/free: locking version
void *ts_malloc_lock(size_t size);
void ts_free_lock(void *ptr);

//Thread Safe malloc/free: non-locking version
void *ts_malloc_nolock(size_t size);
void ts_free_nolock(void *ptr);

block_ptr add_space(size_t size);
block_ptr find_first(size_t size);
block_ptr split_block(block_ptr curr, size_t size);
block_ptr remove_block_from_list(block_ptr remove, size_t size);
block_ptr bf_select(size_t size);
block_ptr combine_blocks(block_ptr front, block_ptr back);
block_ptr find_next_free(block_ptr block);

block_ptr add_space_nonlock(size_t size);
block_ptr find_first_nonlock(size_t size);
block_ptr remove_block_from_list_nonlock(block_ptr remove, size_t size);
block_ptr bf_select_nonlock(size_t size);
block_ptr combine_blocks_nonlock(block_ptr front, block_ptr back);
block_ptr find_next_free_nonlock(block_ptr block);


void free_block(void *ptr);
void *bf_malloc(size_t size);

void free_block_nonlock(void *ptr);
void * bf_malloc_nonlock(size_t size);
