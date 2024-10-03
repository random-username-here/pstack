// Here lives the global. Do not include this file when not needed,
//
#ifndef PSTACK_INTERNAL_SHM
#define PSTACK_INTERNAL_SHM

#include "ipc.h"

// Data shared between processes, used by pstack
//
// FIXME: hide this, so nobody could use `extern shared_data` and fiddle with it
//
// NOTE: required to be global, because user should not see that in any way
//       (and this means it must not be passed as function params)
//
extern ipc_shared_data* shared_process_data;

#endif
