#include "inc/ipc.h"
#include "inc/logging.h"
#include "pstack.h"
#include <sys/mman.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

size_t ipc_max_object_size (ipc_shared_data* _)
{
  return getpagesize() - sizeof(ipc_shared_data);
}

/// Init the shared memory
pstack_err_t ipc_init_shmem (ipc_shared_data** ipc)
{

  if (!ipc)
    return PSTACK_INTERNAL_ERROR;

  // Mmap a buffer for sharing the memory
  // NOTE: this allocates an entire page for this,
  //
  (*ipc) = 
    (ipc_shared_data*) mmap(
      NULL, getpagesize(),
      PROT_READ | PROT_WRITE,
      MAP_SHARED | MAP_ANONYMOUS,
      -1, 0
    );

  assert((size_t) getpagesize() > sizeof(ipc_shared_data));

  if ((*ipc) == NULL)
    return PSTACK_OUT_OF_MEMORY;

  // Initialize the shared structure
  sem_init(&(*ipc)->inbound_semaphore, 1, 0);
  sem_init(&(*ipc)->outbound_semaphore, 1, 0);
  (*ipc)->req_cmd = PSTACK_IPC_UNSET_COMMAND;
  (*ipc)->res_error = PSTACK_OK;
  (*ipc)->buffer_length = ipc_max_object_size(*ipc);
  (*ipc)->handle = NULL;

  return PSTACK_OK;
}

/// Store some value (element to be pushed) into the buffer
pstack_err_t ipc_store_buffer (ipc_shared_data* ipc, void* data, size_t size) 
{
  if (!data)
    return PSTACK_NULL_POINTER;
  if (!ipc)
    return PSTACK_INTERNAL_ERROR;

  // Increase memory size if we are moving
  // big objects around.
  // NOTE: this code is commented out, for ease of MVP

  /*while (ipc->buffer_length < size) {
    char* proc_data_end = ((char*) ipc) 
                        + sizeof(ipc_shared_data)
                        + ipc->buffer_length;
    void* res = mmap(
        proc_data_end, getpagesize(),
        PROT_READ | PROT_WRITE,
        MAP_ANONYMOUS | MAP_SHARED | MAP_FIXED,
        -1, 0
    );
    if (!res)
      return PSTACK_OUT_OF_MEMORY;
    ipc->buffer_length += getpagesize();
  }*/
  if (size > ipc_max_object_size(ipc)) {
    log$("ipc: Failed to store value of length %zu, max size is %zu", size, ipc_max_object_size(ipc));
    return PSTACK_TOO_BIG_ELEMENT;
  }

  memcpy(ipc->buffer, data, size);
  ipc->elem_size = size;

  return PSTACK_OK;
}

/// Wait for other process to set the semaphore
pstack_err_t ipc_wait_for_req (ipc_shared_data* ipc) {
  sem_wait(&ipc->inbound_semaphore);

  return PSTACK_OK;
}

/// Send the message & wait for request
pstack_err_t ipc_request (ipc_shared_data* ipc) {
  sem_post(&ipc->inbound_semaphore);
  sem_wait(&ipc->outbound_semaphore);

  return PSTACK_OK;
}

/// Send the message
pstack_err_t ipc_send_res (ipc_shared_data* ipc) {
  sem_post(&ipc->outbound_semaphore);
  
  return PSTACK_OK;
}

