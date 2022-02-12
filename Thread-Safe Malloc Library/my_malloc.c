#include <stdlib.h>
#include <stdio.h>
#include "my_malloc.h"
#include <unistd.h>
#include <pthread.h>
#define BLOCK_SIZE (sizeof(struct block))

//global parameter to record the head and end of current 
//heap list, and the address of the beginning of the Heap
//will assign it when first malloc a space.
block_ptr head = NULL;
block_ptr end = NULL;
void * HEAP_START = NULL;

//Global Parameter for locks.
pthread_mutex_t mutex_malloc = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_thread= PTHREAD_MUTEX_INITIALIZER;

//Non-lock approach need there own header for each threads.
__thread block_ptr head_thread = NULL;
__thread block_ptr end_thread = NULL;

//Thread Safe malloc/free: locking version
void *ts_malloc_lock(size_t size){
    pthread_mutex_lock(&mutex_malloc);
    void * ans = bf_malloc(size);
    pthread_mutex_unlock(&mutex_malloc);
    return ans;
}

void ts_free_lock(void *ptr){
    pthread_mutex_lock(&mutex_malloc);
    free_block(ptr);
    pthread_mutex_unlock(&mutex_malloc);  
}

//Thread Saft Malloc/free: Non-locking version
void *ts_malloc_nolock(size_t size){
    void * ans = bf_malloc_nonlock(size);
    return ans;
}

void ts_free_nolock(void *ptr){
    free_block_nonlock(ptr);
}


///////////////MALLOC///////////////////////
//function shared for two approaches
//add new space after break point.
block_ptr add_space(size_t size){
    block_ptr new = sbrk(BLOCK_SIZE+size);   
    new->block_size = size;
    new->prev = NULL;
    new->next = NULL;
    new->address =(char *)new + BLOCK_SIZE;
    return new;
}

block_ptr add_space_nonlock(size_t size){
    pthread_mutex_lock(&mutex_thread);
    block_ptr new = sbrk(BLOCK_SIZE+size);
    pthread_mutex_unlock(&mutex_thread);    
    new->block_size = size;
    new->prev = NULL;
    new->next = NULL;
    new->address =(char *)new + BLOCK_SIZE;
    return new;
}



//find the first avaliable free block
block_ptr find_first(size_t size){
    block_ptr curr = head;
    while(curr!=NULL){
        if(curr->block_size >= size){
            return curr;
        }
        else{
            curr = curr->next;
        }
    }
    return curr;
}

block_ptr find_first_nonlock(size_t size){
    block_ptr curr = head_thread;
    while(curr!=NULL){
        if(curr->block_size >= size){
            return curr;
        }
        else{
            curr = curr->next;
        }
    }
    return curr;
}

//split the curr block into two with one used, one free, the curr block
//will remaining in the linked list but with new size; the new block will
//return with the required size.
block_ptr split_block(block_ptr curr, size_t size){
    size_t curr_new_size = curr->block_size - size - BLOCK_SIZE;
    block_ptr new = (block_ptr)((char*)curr->address + curr_new_size);
    curr->block_size = curr_new_size;
    new->block_size = size;
    new->next = new->prev = NULL;
    new->address = (char *)new + BLOCK_SIZE;
    return new;
}

//remove the block from the free list, split it if needed; return the one gona to use;
block_ptr remove_block_from_list(block_ptr remove, size_t size){
    if(remove->block_size > BLOCK_SIZE+size){
        block_ptr new = split_block(remove,size);
        return new;
    }
    else{
        if(head==end && remove == head){
            head = NULL;
	        end = NULL;
            remove->next =NULL;
	        remove->prev = NULL;
        }
        else{
            if(remove == head){
                head = remove->next;
                head->prev = NULL;
                remove->next = NULL;
		        remove->prev = NULL;   
            }
            else if(remove == end){
                end = remove ->prev;
                end->next = NULL;
                remove->prev = NULL;
            }
            else{
                remove->prev->next = remove->next;
                remove->next->prev = remove->prev;
                remove->next = NULL;
		        remove->prev = NULL;
            }
        }
        return remove;    
    }
}

block_ptr remove_block_from_list_nonlock(block_ptr remove, size_t size){
    if(remove->block_size > BLOCK_SIZE+size){
        block_ptr new = split_block(remove,size);
        return new;
    }
    else{
        if(head_thread==end_thread && remove == head_thread){
            head_thread = NULL;
	        end_thread = NULL;
            remove->next =NULL;
	        remove->prev = NULL;
        }
        else{
            if(remove == head_thread){
                head_thread = remove->next;
                head_thread->prev = NULL;
                remove->next = NULL;
		        remove->prev = NULL;   
            }
            else if(remove == end_thread){
                end_thread = remove ->prev;
                end_thread->next = NULL;
                remove->prev = NULL;
            }
            else{
                remove->prev->next = remove->next;
                remove->next->prev = remove->prev;
                remove->next = NULL;
		        remove->prev = NULL;
            }
        }
        return remove;    
    }
}

//best fit approach to select the block to put the data in.
block_ptr bf_select(size_t size){
    block_ptr min = find_first(size);
    //if there is no block has enough space
    if(min==NULL){
        min = add_space(size);
        return min;
    }
    //if the return min is not the last block, have to check the 
    //other blocks in the linked list if there is better fit than current min.
    else{
	if(min->block_size == size){
	  min = remove_block_from_list(min,size);
	  return min;
	}
	else{
	  block_ptr curr = min;
	  while(curr!=NULL){
	    if(curr->block_size == size){
	      min = curr;
	      min = remove_block_from_list(min,size);
	      return min;
	    }
	    if(curr->block_size > size && curr->block_size < min->block_size){
	      min = curr;
	    }
            curr = curr->next;
	  }
        }
        min = remove_block_from_list(min,size);
	return min;
    }
}

block_ptr bf_select_nonlock(size_t size){
    block_ptr min = find_first_nonlock(size);
    //if there is no block has enough space
    if(min==NULL){
        min = add_space_nonlock(size);
        return min;
    }
    //if the return min is not the last block, have to check the 
    //other blocks in the linked list if there is better fit than current min.
    else{
	if(min->block_size == size){
	  min = remove_block_from_list_nonlock(min,size);
	  return min;
	}
	else{
	  block_ptr curr = min;
	  while(curr!=NULL){
	    if(curr->block_size == size){
	      min = curr;
	      min = remove_block_from_list_nonlock(min,size);
	      return min;
	    }
	    if(curr->block_size > size && curr->block_size < min->block_size){
	      min = curr;
	    }
            curr = curr->next;
	  }
        }
        min = remove_block_from_list_nonlock(min,size);
	return min;
    }
}

void * bf_malloc(size_t size){
    //if there is no free block, just extend the heap
    if(head==NULL){
        block_ptr block = add_space(size);
        return block->address; 
    }
    else{
        block_ptr block = bf_select(size);
        return block->address;
    }
}

void * bf_malloc_nonlock(size_t size){
    //if there is no free block, just extend the heap
    if(head_thread==NULL){
        block_ptr block = add_space_nonlock(size);
        return block->address; 
    }
    else{
        block_ptr block = bf_select_nonlock(size);
        return block->address;
    }
}

///////////////FREE//////////////////////////////////////

//merge two free blocks if needed.
block_ptr combine_blocks(block_ptr front, block_ptr back){
    if(end == back){
        end = front;
    }
    front->next = back->next;
    if(back->next!=NULL){
      back->next->prev = front;
    }
    back->next = NULL;
    back->prev = NULL;
    front->block_size += BLOCK_SIZE + back->block_size;
    return front;
}
block_ptr combine_blocks_nonlock(block_ptr front, block_ptr back){
    if(end_thread == back){
        end_thread = front;
    }
    front->next = back->next;
    if(back->next!=NULL){
      back->next->prev = front;
    }
    back->next = NULL;
    back->prev = NULL;
    front->block_size += BLOCK_SIZE + back->block_size;
    return front;
}

//find the block in the linked list next to the current block
block_ptr find_next_free(block_ptr block){
    block_ptr curr = head;    
    while(curr!=NULL){
        if(curr->address > block->address){
            return curr;
        }
        curr=curr->next;
    }
    return NULL;
}

block_ptr find_next_free_nonlock(block_ptr block){
    block_ptr curr = head_thread;    
    while(curr!=NULL){
        if(curr->address > block->address){
            return curr;
        }
        curr=curr->next;
    }
    return NULL;
}

//free the block 
void free_block(void *ptr){
    if (ptr == NULL){
        return;
    }
    block_ptr block =(block_ptr)((char*) ptr - BLOCK_SIZE);
    //empty linked list
    if(head == NULL){
        head = block;
	    end = block;
    }
    else{
        block_ptr next_free = find_next_free(block);
        //if there is no next free, block will be the new end;
        if(next_free==NULL){
            end->next = block;
            block->prev = end;
            block->next = NULL;
            end = block;
        }
        //if next_free is head
        else if(next_free == head){
            block->next = next_free;
            block->prev = NULL;
            next_free -> prev = block;
            head = block;
        }
        //else block inbetween two free blocks
        else{
            block->next = next_free;
            block->prev = next_free->prev;
	        next_free->prev->next = block;
            next_free->prev = block;
        }
	    next_free = NULL;
    }
    //after place the block into the linked list, merge the block with the previous and next block if needed
    if(block->prev!=NULL && ((char*) block->prev->address + block->prev->block_size == (char *) block)){
        block = combine_blocks(block->prev,block);
    }
    if(block->next!=NULL && ((char*)block->address + block->block_size == (char *)block->next)){
        block = combine_blocks(block,block->next);
    }
}

void free_block_nonlock(void *ptr){
    if (ptr == NULL){
        return;
    }
    block_ptr block =(block_ptr)((char*) ptr - BLOCK_SIZE);
    //empty linked list
    if(head_thread == NULL){
        head_thread = block;
	    end_thread = block;
    }
    else{
        block_ptr next_free = find_next_free_nonlock(block);
        //if there is no next free, block will be the new end;
        if(next_free==NULL){
            end_thread->next = block;
            block->prev = end_thread;
            block->next = NULL;
            end_thread = block;
        }
        //if next_free is head
        else if(next_free == head_thread){
            block->next = next_free;
            block->prev = NULL;
            next_free -> prev = block;
            head_thread = block;
        }
        //else block inbetween two free blocks
        else{
            block->next = next_free;
            block->prev = next_free->prev;
	        next_free->prev->next = block;
            next_free->prev = block;
        }
	    next_free = NULL;
    }
    //after place the block into the linked list, merge the block with the previous and next block if needed
    if(block->prev!=NULL && ((char*) block->prev->address + block->prev->block_size == (char *) block)){
        block = combine_blocks_nonlock(block->prev,block);
    }
    if(block->next!=NULL && ((char*)block->address + block->block_size == (char *)block->next)){
        block = combine_blocks_nonlock(block,block->next);
    }
}

