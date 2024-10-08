#ifndef PSTACK_LOGGING
#define PSTACK_LOGGING

#include <stdbool.h>

bool pstack_is_main_process (void);

void _pstack_log(const char* file, int line, const char* fmt, ...);

#ifdef PSTACK_LOG
  #define log$(...) _pstack_log(__FILE__, __LINE__, __VA_ARGS__)
#else
  #define log$(...)
#endif

#endif
