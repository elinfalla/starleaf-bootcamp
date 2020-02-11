#include "debug.h"
#include "test.h"

void test_basic()
{
  TEST_START();
  void *alloc = malloc(10);
  TEST_ASSERT(alloc != NULL);
  TEST_END();
}

int main()
{
  mem_safe_printf("%s running\n", __FILE__);

  test_basic(); 
}

