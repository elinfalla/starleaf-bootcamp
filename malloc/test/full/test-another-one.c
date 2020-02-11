#include "debug.h"
#include "test.h"

// Ensures that blocks are being correctly freed
void test_overwritten()
{
  TEST_START();
  bool notoverwritten = true;
  
  char* aaa = malloc(100);
  for (int i = 0; i < 100; i++)
  {
    aaa[i] = 'a';
  }
  char* bbb = malloc(100);
  for (int i = 0; i < 100; i++)
  {
    bbb[i] = 'b';
  }
  
  mem_safe_printf("aaa: %i, bbb: %i\n", aaa, bbb);
  for (int i = 0; i < 100; i++)
  {
    notoverwritten &= (aaa[i] == 'a');
  }
  mem_safe_printf("not overwritten\n");
  TEST_ASSERT(notoverwritten);
  TEST_END();
}

int main()
{
  mem_safe_printf("%s running\n", __FILE__);
  test_overwritten();
}

