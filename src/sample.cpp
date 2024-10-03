#include "pstack.h"
#include <stdio.h>
#include <string.h>

#define ESC_RED "\x1b[91m"
#define ESC_GREEN "\x1b[92m"
#define ESC_BLUE "\x1b[94m"
#define ESC_GRAY "\x1b[90m"
#define ESC_RESET "\x1b[0m"

const char* errors[] = {
  [PSTACK_OK]                  =  ESC_GREEN "ok" ESC_RESET,
  [PSTACK_BROKEN]              =  ESC_RED   "broken pointer" ESC_RESET,
  [PSTACK_UNDERFLOW]           =  ESC_RED   "will underflow" ESC_RESET,
  [PSTACK_NULL_POINTER]        =  ESC_RED   "got the NULL"   ESC_RESET,
  [PSTACK_OUT_OF_MEMORY]       =  ESC_RED   "out of memory"  ESC_RESET,
  [PSTACK_SYSTEM_ERROR]        =  ESC_RED   "system errored" ESC_RESET,
  [PSTACK_TOO_BIG_ELEMENT]     =  ESC_RED   "too big elem"   ESC_RESET,
  [PSTACK_INTERNAL_ERROR]      =  ESC_BLUE  "internal error" ESC_RESET,
  [PSTACK_WRONG_ELEMENT_SIZE]  =  ESC_RED   "wrong elt size" ESC_RESET
};

#define checked$(v) do {\
    pstack_err_t err = (v);\
    size_t len = strlen(#v);\
    printf("\n " ESC_GRAY "doing " ESC_RESET "%s " ESC_GRAY, #v);\
    for (size_t i = len; i < 60; ++i)\
      printf(".");\
    printf(" " ESC_RESET "%s\n\n", errors[err]);\
  } while(0)

int main (void) {

  pstack_t stk = NULL;
  pstack_t stk2 = NULL;

  checked$(pstack_new(&stk, sizeof(int)));
  checked$(pstack_new(&stk2, sizeof(char)));

  int a = 3;
  checked$(pstack_push(&stk, &a, sizeof(int)));
  a = 4;
  checked$(pstack_push(&stk, &a, sizeof(int)));

  int v = 0;
  checked$(pstack_get_last(&stk, &v, sizeof(int)));
  printf("top element is %d\n", v);

  size_t sz = -1;
  checked$(pstack_size(&stk, &sz));
  printf("stack size is %zu\n", sz);

  checked$(pstack_pop(&stk));

  checked$(pstack_get_last(&stk, &v, sizeof(int)));
  printf("now top element is %d\n", v);

  checked$(pstack_size(&stk, &sz));
  printf("now stack size is %zu\n", sz);
  
  checked$(pstack_destroy(&stk2));
  checked$(pstack_destroy(&stk));
  
}
