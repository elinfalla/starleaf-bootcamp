// bad practice to include in header files, 
// but reduces duplication in these tiny c files...
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST_START() \
  mem_safe_printf("TEST START %s\n", __FUNCTION__);

#define TEST_ASSERT(x) \
  mem_safe_printf("assert %s", #x); \
  if (x) { \
    mem_safe_printf(": passed!\n", __FUNCTION__); \
  } \
  else { \
    mem_safe_printf(": failed!\n", __FUNCTION__); \
    exit(1); \
  }

#define TEST_END() \
  mem_safe_printf("TEST SUCCESS %s\n\n", __FUNCTION__);

