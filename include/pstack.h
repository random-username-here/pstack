/**
 * \file
 * \brief A protected stack. Break it if you can.
 */

#ifndef PSTACK
#define PSTACK

#include <stddef.h>

/// The type of the stack.
typedef void* pstack_t;

/// Error of the pstack
typedef enum {

  /// Everything is fine
  PSTACK_OK = 0,

  /// A broken pstack was passed
  PSTACK_BROKEN,

  /// This operation will underflow the stack
  PSTACK_UNDERFLOW,

  /// User passed the null pointer for some argument
  PSTACK_NULL_POINTER,

  /// The program had ran out of memory
  PSTACK_OUT_OF_MEMORY,

  /// Some system error occured
  PSTACK_SYSTEM_ERROR,

  /// The element in the stack is too big
  /// Currently maximum element size is approx. 4096 bytes
  PSTACK_TOO_BIG_ELEMENT,

  /// Internal error
  PSTACK_INTERNAL_ERROR,

  /// User has given the element of wrong length
  PSTACK_WRONG_ELEMENT_SIZE

} pstack_err_t;

/// Create a new stack with given element size
pstack_err_t pstack_new(pstack_t* stack, size_t elem_size);

/// Destroy the stack
pstack_err_t pstack_destroy(pstack_t* stack);

/// Push the element into the stack
pstack_err_t pstack_push(pstack_t* stack, void* val_begin, size_t elem_size);

/// Pop element from the stack
pstack_err_t pstack_pop(pstack_t* stack);

/// Get last element from the stack.
/// Data is written into the `out` pointer.
pstack_err_t pstack_get_last(pstack_t* stack, void* out, size_t elem_size);

/// Get number of elements in the stack
pstack_err_t pstack_size(pstack_t* stack, size_t* out);

#endif
