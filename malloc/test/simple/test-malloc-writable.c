#include "debug.h"
#include "test.h"

void test_writable()
{
  TEST_START();
  size_t size = 100;
  uint8_t *d = malloc(size);
  memset(d, 0xee, size);
  bool written = true;
  for (size_t i = 0; i < size; ++i) {
    written &= (d[i] == 0xee);  
  }
  TEST_ASSERT(written);
  TEST_END();
}

int main()
{
  mem_safe_printf("%s running\n", __FILE__);
  test_writable();
}

