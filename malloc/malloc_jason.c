#include "utility/debug.h"

#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#define USE_DEBUG 0

#define DEBUG(fmt, ...)\
  if (USE_DEBUG) mem_safe_printf(fmt, ##__VA_ARGS__)


typedef struct header
{
  size_t size;
  bool in_use;
  struct header *next;
  bool filler;
}
header;

#define MEMORY_SIZE (1 << 30)
#define ALIGNMENT 16
static char memory[MEMORY_SIZE] = { 0 };
static char *memory_start = NULL; 
static char *memory_end = NULL;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int round_up(int number)
{
  if (number % ALIGNMENT == 0)
  {
    return number;
  }
  else
  {
    int rounded = number - number % ALIGNMENT + ALIGNMENT;
    return rounded;
  }
}

int round_down(int number)
{
  int rounded = number - number % ALIGNMENT;
  return rounded;
}

static void try_merge(header *cursor)
{
  while (cursor->next < (header *)(memory_end) && !cursor->next->in_use)
  {
//    DEBUG("merge: %li with %li;   ", cursor, cursor->next);
    if (cursor->next->next == NULL)
    {
      cursor->size = memory_end - (char *)cursor - sizeof(header);
      cursor->next = (header *)(memory_end);
    }
    else
    {
      cursor->size = cursor->size + cursor->next->size + sizeof(header);
      cursor->next = cursor->next->next;
    }

//    DEBUG("end of merge: %li\n", cursor->next);
  }
}

void* locked_malloc(size_t size)
{
  bool start = false;
  
  if (memory_start == NULL || memory_end == NULL)
  {
    memory_start = (char *)((ptrdiff_t)memory + ALIGNMENT - (ptrdiff_t)memory % ALIGNMENT);
    memory_end = memory_start + MEMORY_SIZE - ALIGNMENT;
    start = true;
  }

  if (size == 0)
  {
    return NULL;
  }
  
  header *cursor = (header *)memory_start;
  DEBUG("memory end: %li\n", memory_end);
  
  if (start)
  {
    cursor->size = memory_end - memory_start;
    cursor->next = (header *)memory_end;
    start = false;
  }
  
  while ((ptrdiff_t)cursor < (ptrdiff_t)memory_end)
  {
    DEBUG("cursor: %li, cursor->size: %li, cursor->in_use: %i, cursor->next: %li\n", cursor, cursor->size, cursor->in_use, cursor->next);
    if ((ptrdiff_t)cursor->next > (ptrdiff_t)memory_end)
    {
      DEBUG("exit\n");
      exit(1);
    }
    if (!cursor->in_use)
      try_merge(cursor);
    
    if (!cursor->in_use && round_up(size) + sizeof(header) <= cursor->size && cursor->size > sizeof(header))
      break;
 
    cursor = cursor->next;
//    DEBUG("cursor: %li, cursor->size: %i\n", cursor, cursor->size);
  }
  
  if ((char *)cursor + size + sizeof(header) > memory_end)
  {
    return NULL;
  }
  
  header *tmp;
  size_t sizefree;
  if (cursor->next)
  {
    tmp = cursor->next;
    sizefree = cursor->size - size - (size_t)sizeof(header);
//    mem_safe_printf("sizefree: %i\n", sizefree);
  }
  else
  {
    tmp = (header *)(memory_end);
    sizefree = (size_t)(memory_end - (char *)cursor - size - sizeof(header));
  }
  
  cursor->size = round_up(size);
//  mem_safe_printf("update: %li, cursor->size: %i, cursor->next: %li\n", cursor, cursor->size, cursor->next);
  cursor->in_use = true;
  cursor->next = (header *)((char *)cursor + round_up(size) + sizeof(header));
  DEBUG("update: %li, cursor->size: %li, cursor->in_use: %i, cursor->next: %li\n", cursor, cursor->size, cursor->in_use, cursor->next);
  
  if (cursor->next != tmp)
  {
    cursor->next->next = tmp;
    cursor->next->size = round_down(sizefree);
    cursor->next->in_use = false;
  }
  DEBUG("update: %li, cursor->size: %li, cursor->in_use: %i, cursor->next: %li\n", cursor->next, cursor->next->size, cursor->next->in_use, cursor->next->next);
//  DEBUG("SET   : %li, cursor->size: %i, cursor->in_use: %i, cursor->next: %li\n", cursor, cursor->size, cursor->in_use, cursor->next);

  header *location = cursor + 1;

  DEBUG("malloc returns: %li      (max = %li)\n", location, memory_end);
  
  return location;
}

void *malloc(size_t size)
{
  pthread_mutex_lock(&mutex);
  DEBUG("malloc called: %i\n", size);
  void *out = locked_malloc(size);
  pthread_mutex_unlock(&mutex);
  return out;
}

void *calloc(size_t nmemb, size_t size)
{
  pthread_mutex_lock(&mutex);
  DEBUG("calloc called\n");
  void *out = locked_malloc(nmemb * size);
  memset(out, 0, nmemb * size);
  pthread_mutex_unlock(&mutex);
  return out;
}

void locked_free(void* ptr)
{
  DEBUG("free called: %li\n", ptr);
  
  if (memory_start + sizeof(header) <= (char *)ptr && (char *)ptr <= memory_end + sizeof(header))
  {
    header *pointer = (header *)ptr - 1;
    DEBUG("free executed\n");
    pointer->in_use = false;
  }
}

void free(void *ptr)
{
  pthread_mutex_lock(&mutex);
  locked_free(ptr);
  DEBUG("free complete\n");
  pthread_mutex_unlock(&mutex);
}

void *realloc(void *ptr, size_t size)
{
  pthread_mutex_lock(&mutex);
  DEBUG("realloc called\n");
  
  char *new_block;
  
  if (!ptr)
  {
    new_block = locked_malloc(size);
  }
  else
  {
    header *pointer = (header *)ptr - 1;
    DEBUG("realloc: %li, pointer->size: %li, pointer->in_use: %i, pointer->next: %li\n", pointer, pointer->size, pointer->in_use, pointer->next);
    DEBUG("realloc called: pointer %li, size %i\n", ptr, size);
    new_block = locked_malloc(size);
    memcpy(new_block, ptr, round_up(size));
    DEBUG("memory copied\n");
    locked_free(ptr);
  }
  pthread_mutex_unlock(&mutex);
  return new_block;
}
