#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>

#define MEM_STACK_SIZE 1024
#define MEM_PRIORITY 5

#define MEM_PRODUCER_SLEEP 1000
#define MEM_CONSUMER_SLEEP 1000

#define MEM_TEST_DATA 'A'
#define MEM_SLAB_BLOCK_SIZE 200
#define MEM_SLAB_BLOCK_NUM 1
#define MEM_SLAB_ALLIGN 4

char *data_ptr;
bool data_ready = false;

K_MEM_SLAB_DEFINE(mem_slab, MEM_SLAB_BLOCK_SIZE, MEM_SLAB_BLOCK_NUM, MEM_SLAB_ALLIGN);
K_THREAD_STACK_DEFINE(producer_stack, MEM_STACK_SIZE);
K_THREAD_STACK_DEFINE(consumer_stack, MEM_STACK_SIZE);

void producer_entry_point() {
  while (true) {
    printf("Producing\n");
    if (!data_ready) {
      //No data allocated in slab:
      if (k_mem_slab_alloc(&mem_slab, (void **)&data_ptr, K_MSEC(100)) == 0) {
        data_ready = true;
        //Set all the slab to some value:
        memset(data_ptr, MEM_TEST_DATA, 400); 
        
      } else {
      printf("Memory allocation time-out.\n");
      }
    }
    else //Slab already alocated - consumer has not yet freed it:
     {
      printf("Memory not yet freed by consumer, waiting.\n");   
    }

    k_sleep(K_MSEC(MEM_PRODUCER_SLEEP));
  }
}

void consumer_entry_point() {
  int i;
  while (true) {
    printf("Consuming\n");
    if (data_ready) {
      //Read data:
      printf("Data ready:\n");
      for (i=0; i<MEM_SLAB_BLOCK_SIZE; i++) {
        printf("%c", data_ptr[i]);
      }
      printf("\n");     

      //Free the slab, not needed any more:
      k_mem_slab_free(&mem_slab, (void **)&data_ptr);
      data_ready = false;
    } else {
      printf("Data not yet ready, waiting.\n");
    }
    k_sleep(K_MSEC(MEM_CONSUMER_SLEEP));
  }
}

static struct k_thread producer_thread, consumer_thread;
k_tid_t producer_tid, consumer_tid;


void main() {
  
  printf("Creating consumer thread.\n");
  producer_tid = k_thread_create(&producer_thread, producer_stack, MEM_STACK_SIZE, producer_entry_point, NULL, NULL, NULL, MEM_PRIORITY, 0, K_NO_WAIT);
  k_sleep(K_MSEC(MEM_CONSUMER_SLEEP));
  
  printf("Creating consumer thread.\n");
  consumer_tid = k_thread_create(&consumer_thread, consumer_stack, MEM_STACK_SIZE, consumer_entry_point, NULL, NULL, NULL, MEM_PRIORITY, 0, K_NO_WAIT);
  printf("Threads started.\n");
}