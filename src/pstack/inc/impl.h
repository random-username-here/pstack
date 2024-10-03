#ifndef PSTACK_INTERNAL_IMPL
#define PSTACK_INTERNAL_IMPL

#include "ipc.h"

typedef void (*ipc_handler)(ipc_shared_data* ipc);

extern const ipc_handler pstack_ipc_handlers[];

#endif
