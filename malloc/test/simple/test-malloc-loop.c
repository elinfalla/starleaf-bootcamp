#include "debug.h"
#include "test.h"

void test_loop()
{
  TEST_START();
  void *d;
  bool never_null = true;
  for (int i = 0; i < 1000; ++i) {
    d = malloc(10);
    never_null &= (d != NULL);
    memset(d, 0xee, 10);
  }
  TEST_ASSERT(never_null);
  TEST_END();
}

int main()
{
  mem_safe_printf("%s running\n", __FILE__);
  test_loop();
}
