#include "debug.h"
#include "test.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

void test_equivalent()
{
  TEST_START();
  TEST_ASSERT(strcmp("hello, world", "hello, world") == 0);
  TEST_END();
}

void test_identical()
{
  TEST_START();
  const char* s1 = "hello, world";
  TEST_ASSERT(strcmp(s1, s1) == 0);
  TEST_END();
}

void test_empty()
{
  TEST_START();
  TEST_ASSERT(strcmp("", "") == 0);
  TEST_END();
}

void test_first_bigger()
{
  TEST_START(); 
  TEST_ASSERT(strcmp("aaa", "bbb") != 0);
  TEST_END();
}

void test_second_bigger()
{
  TEST_START(); 
  TEST_ASSERT(strcmp("bbb", "aaa") != 0);
  TEST_END();
}

void test_first_longer()
{
  TEST_START(); 
  TEST_ASSERT(strcmp("aaaa", "aaa") != 0);
  TEST_END();
}

void test_second_longer()
{
  TEST_START(); 
  TEST_ASSERT(strcmp("aa", "aaa") != 0);
  TEST_END();
}

int main()
{
  printf("%s running\n", __FILE__);
  test_equivalent();
  test_identical();
  test_empty();  
  
  test_first_bigger();
  test_second_bigger();
  test_first_longer();
  test_second_longer();

  printf("%s passed\n", __FILE__);
  return 0;
}

