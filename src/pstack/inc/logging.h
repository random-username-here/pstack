#ifndef PSTACK_LOGGING
#define PSTACK_LOGGING

#include "macro.h"
#include <stdbool.h>

PSTACK_STATIC bool pstack_is_main_process (void);

void _pstack_log(const char* file, int line, const char* fmt, ...);

#define log$(...) _pstack_log(__FILE__, __LINE__, __VA_ARGS__)

#endif
