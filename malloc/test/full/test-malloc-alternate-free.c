#include "debug.h"
#include "test.h"

void test_alternating()
{
  TEST_START();

  // allocate a bunch of blocks
  size_t NUM_PTRS = 100;
  void *ptrs[NUM_PTRS];
  bool never_null = true;
  for (size_t i = 0; i < NUM_PTRS; ++i) {
    ptrs[i] = malloc(5);
    never_null &= (ptrs[i] != NULL);
    memset(ptrs[i], 0xee, 5);
  }
  TEST_ASSERT(never_null);

  // free them in an alternating fashion
  for (size_t i = 0; i < 2; ++i) {
    mem_safe_printf("Freeing %s\n", (i == 0 ? "evens" : "odds"));
    for (size_t j = i; j < NUM_PTRS; j += 2) {
      //mem_safe_printf("into test loop\n");
      free(ptrs[j]);
    }
  }
  bool we_survived = true;
  TEST_ASSERT(we_survived);


  bool not_null = true;
  void* pointer = malloc(65536);
  not_null = (pointer != NULL);

  TEST_ASSERT(not_null);


  TEST_END();
}

int main()
{
  mem_safe_printf("%s running\n", __FILE__);
  test_alternating();
}
