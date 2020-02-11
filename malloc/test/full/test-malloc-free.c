#include "debug.h"
#include "test.h"

void test_free()
{
  TEST_START();
  void *d = malloc(10);
  free(d);

  bool we_survived = true;
  TEST_ASSERT(we_survived);
  TEST_END();
}

int main()
{
  mem_safe_printf("%s running\n", __FILE__);
  test_free();
}

