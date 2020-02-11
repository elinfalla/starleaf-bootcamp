#include "debug.h"
#include "test.h"

// Ensures that free blocks are being merged
void test_fragmentation()
{
  TEST_START();
  bool never_null = true;
  size_t NUM_PTRS = 60;

  // Fill the allocator with 1k blocks
  void *ptrs[NUM_PTRS];
  for (size_t i = 0; i < NUM_PTRS; ++i) {
    ptrs[i] = malloc(1000);
    never_null &= (ptrs[i] != NULL); 
    if (!never_null) {
      mem_safe_printf("malloc returned NULL @ loop 1 iteration %d\n", i);
      break;
    }
  } 
  TEST_ASSERT(never_null);
  for (size_t i = 0; i < NUM_PTRS; ++i) {
    free(ptrs[i]);
  }
  // Now try allocating larger blocks
  void *d = NULL;
  for (int i = 0; i < 5; ++i) {
    d = malloc(10000);     
    never_null &= (d != NULL);
    if (!never_null) {
      mem_safe_printf("malloc returned NULL @ loop 2 iteration %i\n", i);
      break;
    }
  }
  TEST_ASSERT(never_null);
  TEST_END();
}

int main()
{
  mem_safe_printf("%s running\n", __FILE__);
  test_fragmentation();
}

