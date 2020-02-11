#include "utility/debug.h"

#include <pthread.h>
#include <unistd.h>

#define MEMORY_SIZE (1 << 16)
static char memory[MEMORY_SIZE];
int num_filled = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* malloc(size_t size)
{
  pthread_mutex_lock(&mutex);
  if (MEMORY_SIZE - num_filled < size || size == 0)
  {
    pthread_mutex_unlock(&mutex);
    return NULL;
  }

  int pointer = num_filled;
  num_filled += size;

  pthread_mutex_unlock(&mutex);
  return &memory[pointer];
}

void free(void* ptr)
{
  // NOOP
}
