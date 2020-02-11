#include "debug.h"
#include "test.h"

#include <stdlib.h>

// Randomly frees allocations
void test_random()
{
  TEST_START();

  // Random seed
  srand(4);

  int rd, counter;

  uint8_t* ptrs[1024];
  bool is_used[1024];

  // Just doing rand
  unsigned i, j;
  for (i = 0; i < 100; i++) {
    counter = 0;

    for (j = 0; j < 1024; j++) {
      is_used[j] = false;
    }

    while (counter < 1024) {
      rd = rand() % 1024;
      if (!is_used[rd]) {
        is_used[rd] = true;
        counter++;
      }
    }

    mem_safe_printf("Done round %d\n", i);
  }

  // Adding some mallocs and frees
  for (i = 0; i < 100; i++) {
    counter = 0;

    for (j = 0; j < 1024; j++) {
      is_used[j] = false;
      ptrs[j] = malloc(1);
    }

    while (counter < 1024) {
      rd = rand() % 1024;
      if (!is_used[rd]) {
        free(ptrs[rd]);
        is_used[rd] = true;
        counter++;
      }
    }

    mem_safe_printf("Done malloc/free round %d\n", i);
  }

  bool we_survived = true;
  TEST_ASSERT(we_survived);
  TEST_END();
}

int main()
{
  mem_safe_printf("%s running\n", __FILE__);
  test_random();
}
