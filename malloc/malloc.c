#include "utility/debug.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

#define MEMORY_SIZE (1 << 30)

__attribute__((aligned(16))) char memory[MEMORY_SIZE];
//static aligned_char memory[MEMORY_SIZE];

/*sizeof(data_t) = 32 - this code doesn't work if data_t isn't a multiple of 16,
 as then the pointer returned wouldn't be at the start of a block*/
typedef struct data {
  bool free;
  size_t size;
  struct data *next;
  struct data *previous;
} data_t;

static bool started = false;
static data_t *const head = (data_t*)memory;
static data_t *const end = (data_t*)(memory + MEMORY_SIZE);
static data_t *last;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *locked_malloc(size_t size);
void locked_free(void *ptr);
void *locked_calloc(size_t nmeb, size_t size);
void *locked_realloc(void *ptr, size_t size);


void *malloc(size_t size)
{
  pthread_mutex_lock(&mutex);
  void *ptr = locked_malloc(size);
  pthread_mutex_unlock(&mutex);
  return ptr;
}

void free(void *ptr)
{
  pthread_mutex_lock(&mutex);
  locked_free(ptr);
  pthread_mutex_unlock(&mutex);
  return;
}

void *calloc(size_t nmeb, size_t size)
{
  pthread_mutex_lock(&mutex);
  void *ptr = locked_calloc(nmeb, size);
  pthread_mutex_unlock(&mutex);
  return ptr;
}

void *realloc(void *ptr, size_t size)
{
  pthread_mutex_lock(&mutex);
  void *pointer = locked_realloc(ptr, size);
  pthread_mutex_unlock(&mutex);
  return pointer;
}


void *locked_malloc(size_t size)
{
  size = ((size + 15) / 16 * 16); //+15/16*16 makes it a multiple of 16
  if (size % 16 != 0)
  {
    mem_safe_printf("\nWRONG: padded_size: %d \n", size);
  }

  if (size == 0)
  {
    mem_safe_printf("RETURN NULL %d\n", __LINE__);
    return NULL;
  }

  if (size + sizeof(data_t) > MEMORY_SIZE)
  {
    mem_safe_printf("RETURN NULL %d\n", __LINE__);
    return NULL;
  }
  if (!started)
  {
    started = true;
    head->free = false;
    head->size = size;
    head->next = (data_t*)((char*)head + size + sizeof(data_t)); //use char star as size is 1 so header_padded_size is correct
    head->next->next = end;
    head->next->size = (ptrdiff_t)end - (ptrdiff_t)head->next - sizeof(data_t);
    head->next->free = true;
    head->previous = NULL;
    last = head->next;
    void *pointer = head + 1; //pointer arithmetic is * sizeof(pointer type) hence + 1 not + sizeof(data_t)

    assert((int)pointer % 16 == 0);
    return pointer;
  }

  data_t *current = head;
  data_t *previous;

  while (1) //iterates to end of already allocated memory, returns if free block that's big enough found
  {

    if (size % 16 != 0)
    {
      mem_safe_printf(" WRONG current_padded_size: %d\n", current->size);
    }

    //if allocated size will fit in current block + padding
    if (current->free && current->size >=size)
    {
      //if there's space for another header + at least 1 byte + padding in current block
      if(current->size - size >= sizeof(data_t) + 16)
      {
        data_t *c_next = current->next;
        size_t size_free = current->size - size - sizeof(data_t); //this should be a multiple of 16
        if (size_free % 16 != 0)
          mem_safe_printf("ERROR LINE %d", __LINE__);

        current->next = (data_t*)((char*)current + size + sizeof(data_t));
        current->free = false;
        current->size = size;

        current->next->next = c_next;
        c_next->previous = current->next;
        current->next->size = size_free;
        current->next->free = true;
        current->next->previous = current;
      }
      else
      {
        current->free = false;
      }

      void *pointer = current + 1;
      assert((int)pointer % 16 == 0);
      return pointer;
    }
    if (current->next == last)
    {
      previous = current;
      current = current->next;
      if (current != last)
        mem_safe_printf("ERROR LINE %d\n", __LINE__);
      break;
    }
    else
    {
      current = current->next;
    }
  }

  if ((ptrdiff_t)size > (ptrdiff_t)end - (ptrdiff_t)current)
  {
    mem_safe_printf("RETURN NULL %d\n", __LINE__);
    return NULL;
  }
  else
  {
    current->size = size;
    current->free = false;
    current->next = (data_t*)((char*)current + size + sizeof(data_t));

    current->previous = previous;
    last = current->next;

    current->next->next = end; //was NULL
    current->next->size = (ptrdiff_t)end - (ptrdiff_t)current->next
     - sizeof(data_t);
    current->next->previous = current;

    void *pointer = current + 1;

    assert((int)pointer % 16 == 0);
    return pointer;
  }

  mem_safe_printf("RETURN NULL %d\n", __LINE__);
  return NULL;
}

void locked_free(void *ptr)
{
  //mem_safe_printf("enter free\n");
  if ((data_t*)ptr >= head && (data_t*)ptr <= end)
  {
    data_t *current = (data_t*)ptr - 1;
    current->free = true;
    current->size = (ptrdiff_t)current->next - (ptrdiff_t)ptr;

    if ((ptrdiff_t)current->size != (ptrdiff_t)current->next - (ptrdiff_t)ptr)
    {
      mem_safe_printf("ERROR LINE %d - current->size: %d current->next - ptr: %d\n",
       __LINE__, (ptrdiff_t)current->size, (ptrdiff_t)current->next - (ptrdiff_t)ptr);

    }

    if (current->next == last)
    {
      current->next = end;

      current->size += sizeof(data_t) + last->size;

      last = current;
    }
    else if (current->next != end && current->next->free)
    {
      //size
      size_t size = current->next->size;
      current->size += sizeof(data_t) + size;
      //pointers
      current->next = current->next->next;
      current->next->previous = current;
    }

    if (current != head && current->previous->free)
    {
      //pointers
      if (current == last)
      {
        last = current->previous;
      }
      current->previous->next = current->next;
      current->next->previous = current->previous;
      //size
      current->previous->size += current->size + sizeof(data_t);
    }
    if (head->next == end)
    {
      started = false;
    }

    return;
  }

  return;
}

void *locked_calloc(size_t nmeb, size_t size)
{
  if (nmeb == 0 || size == 0)
  {
    return NULL;
  }
  size_t malloc_size = nmeb * size;
  void *pointer = locked_malloc(malloc_size);
  memset(pointer, 0, malloc_size);
  return pointer;
}

void *locked_realloc(void *ptr, size_t size)
{
  if (ptr == NULL)
  {
    void *pointer = locked_malloc(size);
    return pointer;
  }
  data_t *header = (data_t*)ptr - 1;
  size_t prev_size = header->size;
  void *pointer = locked_malloc(size);
  if (pointer == NULL)
  {
    return NULL;
  }
  memcpy(pointer, ptr, size < prev_size ? size : prev_size);
  return pointer;
}
