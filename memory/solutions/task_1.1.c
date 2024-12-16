/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>  
#include <zephyr/kernel.h>

#define MEM_STACK_SIZE 2000
#define MEM_PRIORITY 5

#define MEM_ALLOC_SIZE 512
#define MEM_FREE_ON //If swithched of, you can monitor how the thread goes out of memory
#define MEM_WAIT_TIME 1000


void mem_entry_point() {
  char *mem_ptr;

  while (true) {
   
  printf("Allocating memory\n");
  mem_ptr = k_malloc(MEM_ALLOC_SIZE);
  if (mem_ptr == NULL) {
    printf("Memory allocation failed\n");
  } else {
    printf("Memory allocated at %p\n", mem_ptr);
    
    #ifdef MEM_FREE_ON
    printf("Freeing memory\n");
    k_free(mem_ptr);
    #endif
  }
  //Wait for a while just to have time to read the messages
  k_sleep(K_MSEC(1000));
  }
 }

K_THREAD_DEFINE(mem_tid, MEM_STACK_SIZE, mem_entry_point, NULL, NULL, NULL, MEM_PRIORITY, 0, 0);
