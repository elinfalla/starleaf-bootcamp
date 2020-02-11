#include "debug.h"
#include "test.h"

void test_oom()
{
  TEST_START();
  void *too_large_malloc = malloc(1 << 30);
  TEST_ASSERT(too_large_malloc == NULL);
  TEST_END();
}

int main()
{
  mem_safe_printf("%s running\n", __FILE__);
  test_oom();
}

