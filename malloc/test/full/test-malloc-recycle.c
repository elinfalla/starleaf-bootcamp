#include "debug.h"
#include "test.h"

// Ensures that blocks are being correctly freed
void test_recycling()
{
  TEST_START();
  void* d = NULL;
  bool never_null = true;

  for (int i = 0; i < 200; ++i) {
    d = malloc(10000);     
    never_null &= (d != NULL);
    memset(d, 0xee, 10000);
    free(d);
    if (!never_null) {
      mem_safe_printf("malloc returned NULL @ loop iteration %i\n", i);
      break;
    }
  }
  TEST_ASSERT(never_null);
  TEST_END();
}

int main()
{
  mem_safe_printf("%s running\n", __FILE__);
  test_recycling();
}

