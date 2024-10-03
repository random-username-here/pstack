#include "pstack.h"
#include "inc/ipc.h"
#include "inc/shm.h"
#include <string.h>

#define notnull_or_die$(ptr) do { if (!(ptr)) return PSTACK_NULL_POINTER; } while (0)

/// Create a new stack with given element size
pstack_err_t pstack_new(pstack_t* stack, size_t elem_size)
{
  notnull_or_die$(stack);

  shared_process_data->req_cmd = PSTACK_IPC_CREATE;
  shared_process_data->elem_size = elem_size;

  ipc_request(shared_process_data);

  *stack = shared_process_data->handle;
  return shared_process_data->res_error;
}

pstack_err_t pstack_destroy(pstack_t* stack)
{
  notnull_or_die$(stack);

  shared_process_data->req_cmd = PSTACK_IPC_DESTROY;
  shared_process_data->handle = *stack;

  ipc_request(shared_process_data);

  return shared_process_data->res_error;
}


/// Push the element into the stack
pstack_err_t pstack_push(pstack_t* stack, void* val_begin, size_t elem_size)
{
  notnull_or_die$(stack);
  notnull_or_die$(val_begin);

  shared_process_data->req_cmd = PSTACK_IPC_PUSH;
  shared_process_data->handle = *stack;
  shared_process_data->elem_size = elem_size;
  ipc_store_buffer(shared_process_data, val_begin, elem_size);
  
  ipc_request(shared_process_data);
  
  return shared_process_data->res_error;
}

/// Pop element from the stack
pstack_err_t pstack_pop(pstack_t* stack)
{
  notnull_or_die$(stack);

  shared_process_data->req_cmd = PSTACK_IPC_POP;
  shared_process_data->handle = *stack;
  
  ipc_request(shared_process_data);
  
  return shared_process_data->res_error;
}

/// Get last element from the stack.
/// Data is written into the `out` pointer.
pstack_err_t pstack_get_last(pstack_t* stack, void* out, size_t elem_size)
{
  notnull_or_die$(stack);
  notnull_or_die$(out);

  shared_process_data->req_cmd = PSTACK_IPC_GET_TOP;
  shared_process_data->handle = *stack;
  shared_process_data->elem_size = elem_size;

  ipc_request(shared_process_data);

  if (shared_process_data->res_error != PSTACK_OK)
    return shared_process_data->res_error;

  if (shared_process_data->elem_size != elem_size)
    return PSTACK_WRONG_ELEMENT_SIZE;

  memcpy(out, shared_process_data->buffer, elem_size);

  return PSTACK_OK;
}

/// Get number of elements in the stack
pstack_err_t pstack_size(pstack_t* stack, size_t* out)
{
  notnull_or_die$(stack);
  notnull_or_die$(out);

  shared_process_data->req_cmd = PSTACK_IPC_LENGTH;
  shared_process_data->handle = *stack;

  ipc_request(shared_process_data);

  if (shared_process_data->res_error != PSTACK_OK)
    return shared_process_data->res_error;

  *out = shared_process_data->elem_size;
  
  return PSTACK_OK;
}


