//This memory allocator included function like :malloc calloc and free 
//malloc --> crate space 
//calloc --> intial with 0
//free --> set space to NULL again
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
//save global information 
typedef char ALIGN[16];

union header{
    struct{
        size_t size; 
        unsigned is_free; 
        union header*next; 
    }s;
    ALIGN stub;
};

typedef union header header_t; 

header_t *head , *tail;
header_t *get_free_block(size_t size);
//step to use the lock
//init
//add 
//break 
//destory
pthread_mutex_t global_malloc_lock;


//malloc function 
//spark allocate the memory
//return NULL pointer when the size is zero

void *malloc(size_t size){
    
    //glbal set up
    size_t total_size;
    void*block;
    
    // create header
    header_t  *header;
    
    //size == 1 ? 
    if(!size){
        return NULL; 
    }
    //valid size loop
    // add lock 
	pthread_mutex_lock(&global_malloc_lock);

    //refer to next function 
    header = get_free_block(size);
   
    //mark that block not free release the global lock return a pointer to that block
    //the pointer is rever to the block of memory just found by a traversing the list 
    //+1 refer to the pointer after the adress that is not free 
    if(header){
        header->s.is_free = 0;
        pthread_mutex_unlock(&global_malloc_lock);
        return(void*)(header + 1); 
    }
    

    total_size = sizeof(header_t) + size;
    block = sbrk(total_size);
    if(block == (void*) -1){
        pthread_mutex_unlock(&global_malloc_lock);
        return NULL; 
    }

    header = block; 
    header->s.size = size; 
    header->s.is_free = 0; 
    header->s.next = NULL;

    if(!head) head = header; 
    if(tail) tail->s.next = header;
    //return 
    tail = header; 
    pthread_mutex_unlock(&global_malloc_lock);
    return (void*)(header+1); 
    
}

// next address 
// traverses the linked list and see if there already exist a block of memory 
header_t *get_free_block(size_t size){
    header_t *curr = head; 
    //check if it is a null pointer 
    while(curr){
        if(curr ->s.is_free && curr ->s.size >= size){
            return curr; 
        }
        curr = curr->s.next;
    }
    return NULL;
}

//free
//get the header of block that want to be free 
void free(void *block){
    header_t *header , *temp; 
    //get the pointer behind the block 
    void* programbreak;
    if(!block) return; 

    pthread_mutex_lock(&global_malloc_lock);
    //define header 
    //   ____
    //-->|___|-->
    //creat this 
    header = (header_t*)block - 1;

    //check is it the end of the heap
    programbreak = sbrk(0);
    
    //find t he end of the current block 
    //compared with the program break 
    if((char*)block + header->s.size == programbreak){
        if(head == tail){ 
            head = tail = NULL;
        }else{
            temp = head;
            while(temp){
                if(temp ->s.next == tail){
                    temp->s.next = NULL;
                    tail =temp;
                }
                temp = temp->s.next;
            }
        }
        //release memory to os
        //sbark -ve value will relase
        sbrk(0 - sizeof(header_t) - header->s.size);
		pthread_mutex_unlock(&global_malloc_lock);
        return;
    }
    header -> s.is_free = 1;
	pthread_mutex_unlock(&global_malloc_lock);
}
//allocate memory for an array of num elements of nsize bytes 
//return a pointer to the allocated memory 
void *calloc(size_t num , size_t nsize){
    size_t size; 
    void *block;
    if(!num || !nsize)
        return NULL;
    size = num * nsize;
    if(nsize != size/num)
        return NULL;
    block = malloc(size);
    if(!block)
        return NULL;
    //set all num to 0 
    //ie)int* a = (int*) calloc(5 , sizeof(int))
    //[0,0,0,0,0] 
    //return type : pointer 
    memset(block , 0 , size);
    return block;
}
//realloct memory , reshape the space to the size that user want 
void *realloc(void* block , size_t size){
    header_t *header;
    void *ret; 
    //check the block have a size or not , if not --> malloc function 
    if(!block || size)
        return malloc(size);
    
    //set the pointer one byte before that
    header = (header_t*) block -1;

    //if size already enough return original pointer
    //no operatopn is needed 
    if(header -> s.size >= size)
        return block; 

    //if the space is not enough
    //crease a space with malloc function 
    ret = malloc(size);
    
    //copy block -> ret provide given sizen (memcpy function)
    //free original block (since it is resize , delte orginal one
    if(ret){
        memcpy(ret, block , header -> s.size);
        free(block);

    }
    return ret;
}

int main(){
   //testing 
   //   
    char * str;
    str = (char *) malloc(15);
    strcpy(str , "jasonmemoryallo");
    printf("String = %s , Address = %u\n" , str , str); 
    
    str = (char*) realloc(str , 25);
    strcpy(str , "jasonmemoryallocator");
    printf("String = %s , Address = %u " , str , str); 
    free(str);
    
    return 0;
}


