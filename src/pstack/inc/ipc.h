/**
 * \file
 * \brief Inter-process communication
 */
#ifndef PSTACK_INTERNAL_IPC
#define PSTACK_INTERNAL_IPC

#include <semaphore.h>
#include <stdbool.h>
#include "pstack.h"

typedef enum {
  PSTACK_IPC_UNSET_COMMAND, // No command currently set
  PSTACK_IPC_HELLO,       // Hello message to check what everything works
  PSTACK_IPC_CREATE,      // Create a new stack
  PSTACK_IPC_DESTROY,     // Destroy the stack
  PSTACK_IPC_PUSH,        // Push some value into the stack
  PSTACK_IPC_POP,         // Pop last value from the stack
  PSTACK_IPC_LENGTH,      // Get stack length
  PSTACK_IPC_GET_TOP,     // Get top element from the stack
  PSTACK_IPC_NUM_COMMANDS
} ipc_command_t;


typedef struct {
  
  // Semaphore to wait for main process to
  // send a message
  sem_t inbound_semaphore;

  // Semaphore to wait for subprocess
  // to respond
  sem_t outbound_semaphore;

  // Command given to the subprocess
  ipc_command_t req_cmd;

  // Error returned by the subprocess
  pstack_err_t res_error;

  // Passed array handle
  void* handle;

  // Size of the elements asked for
  size_t elem_size;

  // Length of the buffer used to pass around the elements 
  size_t buffer_length;

  // Buffer to pass array elements around
  char buffer[];

} ipc_shared_data;


/// Init the shared memory
pstack_err_t ipc_init_shmem (ipc_shared_data** ipc);

/// Store some value (element to be pushed) into the buffer
pstack_err_t ipc_store_buffer (ipc_shared_data* ipc, void* data, size_t size);


/// Send the message to subprocess & wait for the response
pstack_err_t ipc_request (ipc_shared_data* ipc);

/// Send the response from subprocess
pstack_err_t ipc_send_res (ipc_shared_data* ipc);

/// Wait for the request
pstack_err_t ipc_wait_for_req (ipc_shared_data* ipc);

/// Max buffer size to be thrown around between processes
size_t ipc_max_object_size (ipc_shared_data* ipc);


#endif
