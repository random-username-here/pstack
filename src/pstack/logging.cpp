#include "inc/logging.h"
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>

#define ESC_MAIN "\x1b[92m"
#define ESC_SUB  "\x1b[94m"
#define ESC_RST  "\x1b[0m"
#define ESC_LOC  "\x1b[93m"
#define ESC_GRAY "\x1b[90m"

void _pstack_log(const char* file, int line, const char* fmt, ...)
{
  assert(file);
  assert(fmt);

  fprintf(stderr, "%s " ESC_LOC "%30s:%03d" ESC_RST ESC_GRAY " | " ESC_RST,
      (pstack_is_main_process() ? ESC_MAIN "main" ESC_RST : ESC_SUB "subp" ESC_RST),
      file, line
  );

  va_list args; 
  va_start(args, fmt);

  vfprintf(stderr, fmt, args);

  va_end(args);

  fprintf(stderr, "\n"); 
  fflush(stderr); // In case we are crashing, buffer will be not flushed
}
