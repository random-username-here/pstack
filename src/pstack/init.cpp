#include "inc/impl.h"
#include "inc/ipc.h"
#include "inc/logging.h"
#include "inc/shm.h"
#include <pstack.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <stdlib.h>

//=====[ Global variables ]===============================//

/// PID of the process which handles the stack
static pid_t handler_process;

/// Were there any errors during subprocess launch?
static bool had_initialization_failed;

/// Data shared between subprocess and the parent
ipc_shared_data* shared_process_data;

//=====[ Subprocess management ]==========================//

PSTACK_STATIC bool pstack_is_main_process (void) {
  return handler_process != 0;
}

/// Entry point for the subprocess
static void pstack_sub_process_begin (void) {
  log$("Subprocess started");
  while (1) {
    ipc_wait_for_req(shared_process_data);
    log$("Recived message type=%d", shared_process_data->req_cmd);
    if (shared_process_data->req_cmd >= PSTACK_IPC_NUM_COMMANDS) {
      // Something has gone wrong, we are given an unknown command
      log$("!!! Command is unknown", shared_process_data->req_cmd);
      shared_process_data->res_error = PSTACK_INTERNAL_ERROR;
    } else {
      // Reset the error from previous request
      shared_process_data->res_error = PSTACK_OK;
      // Process this
      pstack_ipc_handlers[shared_process_data->req_cmd](shared_process_data);
    }
    ipc_send_res(shared_process_data);
  }
}

/// This function is ran after the 
static void pstack_main_process_begin (void) {
  log$("Sending a hello message");
  shared_process_data->req_cmd = PSTACK_IPC_HELLO;
  ipc_request(shared_process_data);
  log$("Got `%s`", shared_process_data->buffer);
}

/// This consturctor is ran before the 
__attribute__((constructor))
static void pstack_global_ctor (void)
{

  handler_process = 1;

  log$("Allocating shared memory");
  // Init the IPC shared memory
  pstack_err_t err = ipc_init_shmem(&shared_process_data);
  if (err) {
    log$("Failed to allocate shared memory");
    had_initialization_failed = true;
    return;
  }

  log$("Forking the process");
  handler_process = fork();

  // No fork for us, mark a failure
  if (handler_process == -1) {
    log$("Failed to spawn the process");
    had_initialization_failed = true;
    return;
  }

  // Switch path based on which process we are in
  if (handler_process == 0) {
    // We are in the child.
    pstack_sub_process_begin();
    log$("!!! Subprocess loop exited, will terminate the process");
    // Do not enter the main()!
    exit(EXIT_FAILURE);
  } else {
    // Initialize the main process IPC stuff
    pstack_main_process_begin();
  }
}

__attribute__((destructor))
static void pstack_global_dtor (void)
{
  // If we failed, no process was started
  if (had_initialization_failed)
    return;
  // We are done, kill him
  kill(handler_process, SIGTERM);
}
