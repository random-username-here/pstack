#include "inc/impl.h"
#include "inc/ipc.h"
#include "inc/logging.h"
#include "pstack.h"
#include <stdint.h>
#include <stdio.h>
#include <stdalign.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// ----[ Functions called ] ------------------------------//

static void ipc_handle_unset   (ipc_shared_data* ipc);
static void ipc_handle_hello   (ipc_shared_data* ipc);
static void ipc_handle_create  (ipc_shared_data* ipc);
static void ipc_handle_destroy (ipc_shared_data* ipc);
static void ipc_handle_push    (ipc_shared_data* ipc);
static void ipc_handle_pop     (ipc_shared_data* ipc);
static void ipc_handle_length  (ipc_shared_data* ipc);
static void ipc_handle_get_top (ipc_shared_data* ipc);

const ipc_handler pstack_ipc_handlers[] = {
  [PSTACK_IPC_UNSET_COMMAND] = &ipc_handle_unset,
  [PSTACK_IPC_HELLO]         = &ipc_handle_hello,
  [PSTACK_IPC_CREATE]        = &ipc_handle_create,
  [PSTACK_IPC_DESTROY]       = &ipc_handle_destroy,
  [PSTACK_IPC_PUSH]          = &ipc_handle_push,
  [PSTACK_IPC_POP]           = &ipc_handle_pop,
  [PSTACK_IPC_LENGTH]        = &ipc_handle_length,
  [PSTACK_IPC_GET_TOP]       = &ipc_handle_get_top,
};

static const char hello_world[] = "Hello world!";

static void ipc_handle_unset (ipc_shared_data* ipc)
{
  assert(ipc);

  log$("Got IPC_UNSET_COMMAND (command variable was reset), panicking!!!");
  ipc->res_error = PSTACK_INTERNAL_ERROR;
}

static void ipc_handle_hello (ipc_shared_data* ipc)
{
  assert(ipc);
  
  log$("Got IPC_HELLO, returning `hello` message");
  ipc_store_buffer(ipc, (void*) hello_world, sizeof(hello_world));
}

//----[ The actuall stack here ]--------------------------//

typedef struct {

  // Handle given to the user
  // If set to NULL, this stack was destroyed,
  // and this stack can be reused
  pstack_t handle;

  // Number of elements in the stack
  size_t size;

  // Number of elements for which space exists
  size_t capacity;

  // Size of the element
  size_t element_size;

  // The stack data
  alignas(alignof(max_align_t)) char *data;

} stack_t;

// Array of ALL the stacks in the program
// (so this thing is global)
static stack_t* stacks = NULL;
static size_t num_stacks = 0;

//----[ Implementation of the common functions ]----------//
// (there are no uncommon ones)

static void ipc_handle_create (ipc_shared_data* ipc)
{
  assert(ipc);

  log$("Got IPC_CREATE (elem_size=%zu), creating a new stack", ipc->elem_size);

  if (num_stacks > 0 && stacks == NULL) {
    log$(" . Number of stacks is nonzero, but array of them points to NULL. Panic!");
    ipc->res_error = PSTACK_INTERNAL_ERROR;
    return;
  }

  if (ipc->elem_size == 0) {
    log$(" . Caught an attempt to create an array with zero size elements");
    ipc->res_error = PSTACK_WRONG_ELEMENT_SIZE;
    return;
  }

  stack_t* stk = NULL;

  // Look for stack to reuse
  for (size_t i = 0; i < num_stacks; ++i)
    if (stacks[i].handle == NULL)
      stk = &stacks[i];

  // Nothing found to reuse
  if (!stk) {
    // Allocate more memory
    size_t new_num_stacks = num_stacks * 3 / 2 + 1;
    // Note what stacks_new may be NULL if out of memory,
    // but stacks will be preserved.
    // And if stacks = realloc(stacks, ...) we can leak the memory
    stack_t* stacks_new = (stack_t*) realloc(stacks, sizeof(stack_t) * new_num_stacks);
    // We failed to allocate miserably
    // Now report that to the user
    if (!stacks_new) {
      log$(" . Failed to get more memory for the stack");
      // TODO: ask for a smaller chunk?
      ipc->res_error = PSTACK_OUT_OF_MEMORY;
      return;
    }
    // Memory is fine, zero it
    stacks = stacks_new;
    memset(stacks + num_stacks, 0, (new_num_stacks - num_stacks) * sizeof(stack_t));
    stk = stacks + num_stacks;
    num_stacks = new_num_stacks;
  }

  assert(stk);


  // TODO: make this unpredictable
  stk->handle = stk;
  stk->element_size = ipc->elem_size;
  stk->capacity = 0;

  log$(" . made a new stack at %p with handle %p", stk, stk->handle);

  ipc->handle = stk;
}

// Find the stack with specified handle
// This sets the error if nothing was found
static stack_t* find_stack(ipc_shared_data* ipc, pstack_t handle) {

  log$(" . searching for stack...");

  if (num_stacks && stacks == NULL) {
    log$(" . . list of all the stacks is broken");
    ipc->res_error = PSTACK_INTERNAL_ERROR;
    return NULL;
  }

  for (size_t i = 0; i < num_stacks; ++i) {
    if (stacks[i].handle == handle) {
      log$(" . stack found, it is %zu-th in list, address %p", i, &stacks[i]);
      return &stacks[i];
    }
  }


  log$(" . stack was not found");
  ipc->res_error = PSTACK_BROKEN;
  return NULL;
}

static void ipc_handle_destroy (ipc_shared_data* ipc) {
  log$("Got IPC_DESTROY (handle=%p), destroying the stack", ipc->handle);

  stack_t* stack = find_stack(ipc, ipc->handle);
  if (!stack) // error was already set
    return;

  log$(" . destroying it");
  stack->handle = NULL;
  free(stack->data);

}

static void ipc_handle_push(ipc_shared_data* ipc) {
  log$("Got IPC_PUSH (handle=%p, size=%zu), adding one element to the stack", ipc->handle, ipc->elem_size);

  stack_t* stack = find_stack(ipc, ipc->handle);
  if (!stack) return;

  if (stack->element_size != ipc->elem_size) {
    log$(" . attempted to push element of size %zu vs expected size %zu", ipc->elem_size, stack->element_size);
    ipc->res_error = PSTACK_WRONG_ELEMENT_SIZE;
    return;
  }

  if (stack->size + 1 > stack->capacity) {
    log$(" . reallocating buffer");
    size_t new_capacity = stack->capacity * 3 / 2 + 1;
    char* mem = (char*) realloc(stack->data, stack->element_size * new_capacity);
    if (!mem) {
      log$(" . ran out of memory");
      ipc->res_error = PSTACK_OUT_OF_MEMORY;
      return;
    }
    memset(mem + stack->capacity, 0, (new_capacity - stack->capacity) * stack->element_size);
    stack->capacity = new_capacity;
    stack->data = mem;
  }

  log$(" . writting element into the stack");
  memcpy(stack->data + stack->size * stack->element_size, ipc->buffer, stack->element_size);
  stack->size++;
}

static void ipc_handle_pop(ipc_shared_data* ipc) {
  log$("Got IPC_POP (handle=%p), deleting top element", ipc->handle);

  stack_t* stack = find_stack(ipc, ipc->handle);
  if (!stack) return;

  if (stack->size == 0) {
    ipc->res_error = PSTACK_UNDERFLOW;
    return;
  }

  stack->size--;

  if (stack->size + 1 < stack->capacity/4) {
    log$(" . shrinking the buffer");
    // time to free some extra memory
    size_t new_capacity = stack->capacity / 4;
    char* mem = (char*) realloc(stack->data, stack->element_size * new_capacity);
    if (!mem) {
      // well, we are not freeing it.
    } else {
      stack->data = mem;
      stack->capacity = new_capacity;
    }
  }
}

static void ipc_handle_length  (ipc_shared_data* ipc) {
  log$("Got IPC_LENGTH (handle=%p), returning stack length", ipc->handle);

  stack_t* stack = find_stack(ipc, ipc->handle);
  if (!stack) return;

  ipc->elem_size = stack->size;
}

static void ipc_handle_get_top (ipc_shared_data* ipc) {
  log$("Got IPC_TOP (handle=%p, expected size=%zu), extracting top element", ipc->handle, ipc->elem_size);

  stack_t* stack = find_stack(ipc, ipc->handle);
  if (!stack) return;

  if (stack->size == 0) {
    ipc->res_error = PSTACK_UNDERFLOW;
    return;
  }

  ipc_store_buffer(ipc, stack->data + (stack->size-1)*stack->element_size, stack->element_size);
}
