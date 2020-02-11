#include "debug.h"
#include "test.h"

void test_zero()
{
  TEST_START();
  void *zero_alloc = malloc(0);
  TEST_ASSERT(zero_alloc == NULL);
  TEST_END();
}

int main()
{
  mem_safe_printf("%s running\n", __FILE__);
  test_zero();
}

