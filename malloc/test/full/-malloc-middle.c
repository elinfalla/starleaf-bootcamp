#include "debug.h"
#include "test.h"



void test_middle(void)
{
  TEST_START();
  bool never_null = true;
  for (int i = 0; i < 6; i++)
  {
    if (i == 3)
    {
      void* pointer = malloc(30);
      never_null &= (pointer != NULL);
    }

  }
  TEST_ASSERT(never_null);
  TEST_END();
}

int main()
{
  mem_safe_printf("%s running\n", __FILE__);
  test_middle();
}
