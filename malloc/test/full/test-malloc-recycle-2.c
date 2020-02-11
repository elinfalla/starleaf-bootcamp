#include "debug.h"
#include "test.h"

// Ensures that blocks are being correctly freed
void test_recycling()
{
  TEST_START();
  bool never_null = true;

  const int iterations = 5;
  int num_allocs = 50;
  const int alloc_size = 1000;
  void* allocs[num_allocs];

  for (int iter = 0; iter < iterations; ++iter) {
    for (int i = 0; i < num_allocs; ++i) {
      allocs[i] = malloc(alloc_size);
      mem_safe_printf("been allocated: %i\n", i);
      never_null &= (allocs[i] != NULL);
      memset(allocs[i], 0xee, alloc_size);
      if (!never_null) {
        mem_safe_printf("malloc returned NULL @ iteration %i:%i\n", iter, i);
        TEST_ASSERT(false);
      }
      /*if (allocs[i] == NULL)
      {
        num_allocs = i;
        break;
      }*/
    }
    for (int i = 0; i < num_allocs; ++i) {
      free(allocs[i]);
    }
  }

  //void* p = malloc(20000);
  TEST_ASSERT(never_null);
  TEST_END();
}

int main()
{
  mem_safe_printf("%s running\n", __FILE__);
  test_recycling();
}
