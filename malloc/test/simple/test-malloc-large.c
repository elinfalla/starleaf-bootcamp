#include "debug.h"
#include "test.h"

void test_large()
{
  TEST_START();
  void *large_alloc = malloc(1 << 12);
  TEST_ASSERT(large_alloc != NULL);
  TEST_END();
}

int main()
{
  mem_safe_printf("%s running\n", __FILE__);
  test_large();
}

