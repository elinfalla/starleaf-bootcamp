#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define WORD_LEN 45
#define TABLE_SIZE 100000

typedef struct node {
  char place[WORD_LEN];
  int dist;
  bool fixed;
  int heap_index;
} node_t;

typedef struct list_entry {
  int index;
  int dist;
  struct list_entry* next;
} list_entry_t;

typedef struct hashtable_entry {
  int index;
  struct hashtable_entry* next;
} hashtable_entry_t;

int findDist(int start_index, int destination_index, int num_places,
   node_t places[num_places], list_entry_t* paths[], int* heap_size);
int findIndex(char place[], node_t places[], hashtable_entry_t* hashtable[]);
static void heap_shift_up(int* heap[], int heap_index);
static void heap_shift_down(int* heap[], int heap_index, int* heap_size);
static void heap_insert(int heap_entry, int* heap[], int* heap_size);
static int extract_heap_min(int* heap[], int* heap_size);

unsigned long
hash_function(char str[])
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    hash = hash % TABLE_SIZE;
    return hash;
}

static list_entry_t *list_append(list_entry_t *paths[], int index)
{
  list_entry_t* new_entry = malloc(sizeof(list_entry_t));
  assert(new_entry != NULL);
  memset(new_entry, 0, sizeof(list_entry_t));

  if (paths[index] == NULL) {
    paths[index] = new_entry;
    new_entry->next = NULL;
  }
  else
  {
    new_entry->next = paths[index];
    paths[index] = new_entry;
  }
  return new_entry;
}

static void initialise_hashtable(hashtable_entry_t* hashtable[])
{
  for (int i = 0; i < TABLE_SIZE; i++)
  {
    hashtable[i] = NULL;
  }
}

static void initialise_list(list_entry_t* paths[], int num_places)
{
  for (int i = 0; i < num_places; i++)
  {
    paths[i] = NULL;
  }
}

static hashtable_entry_t *hashtable_append(hashtable_entry_t *hashtable[], int index)
{
  hashtable_entry_t* new_entry = malloc(sizeof(hashtable_entry_t));
  assert(new_entry != NULL);
  memset(new_entry, 0, sizeof(hashtable_entry_t));

  if (hashtable[index] == NULL) {
    hashtable[index] = new_entry;
    new_entry->next = NULL;
  }
  else
  {
    new_entry->next = hashtable[index];
    hashtable[index] = new_entry;
  }
  return new_entry;
}

int main (int argc, char* argv[])
{
  if (argc != 2)
  {
    fprintf(stderr, "Please provide an input file\n");
    return 1;
  }

  char* in = argv[1];
  FILE* input = fopen(in, "r");

  //scanning in the number of places
  int num_places;
  if (fscanf(input, " %i\n", &num_places) != 1)
  {
    fprintf(stderr, "Incorrect file format - num_places\n");
    return 1;
  }

  node_t places[num_places];
  hashtable_entry_t* hashtable[TABLE_SIZE];
  initialise_hashtable(hashtable);

  //scanning in the place names
  for (int i = 0; i < num_places; i++)
  {
    if (fscanf(input, " %s", places[i].place) != 1)
    {
      fprintf(stderr, "Incorrect file format - places, i: %d\n", i);
      return 1;
    }
    places[i].fixed = false;
    places[i].dist = 0;
    places[i].heap_index = -1; //??
    int hash = hash_function(places[i].place);

    hashtable_entry_t* new_entry = hashtable_append(hashtable, hash);
    new_entry->index = i;

  }

  //scanning in the number of paths
  int num_paths;
  if (fscanf(input, " %i\n", &num_paths) != 1)
  {
    fprintf(stderr, "Incorrect file format - num_paths\n");
    return 1;
  }

  list_entry_t* paths[num_places];
  initialise_list(paths, num_places);

  char start[WORD_LEN];
  char dest[WORD_LEN];
  int distance;
  int start_index = -1;
  int destination_index = -1;
  for (int i = 0; i < num_paths; i++)
  {
    if (fscanf(input, "%s %s %i\n", start, dest, &distance) != 3)
    {
      fprintf(stderr, "Incorrect file format - paths\n");
      return 1;
    }
    start_index = findIndex(start, places, hashtable);
    destination_index = findIndex(dest, places, hashtable);

    list_entry_t* new_entry = list_append(paths, start_index);
    new_entry->index = destination_index;
    new_entry->dist = distance;

    new_entry = list_append(paths, destination_index);
    new_entry->index = start_index;
    new_entry->dist = distance;


  }

  int* heap[num_places];
  int* heap_size = malloc(sizeof(int));
  *heap_size = 1;

  while (feof(input) == 0)
  {
    char A[WORD_LEN];
    char B[WORD_LEN];

    if (fscanf(input, " %s %s\n", A, B) != 2)
    {
      fprintf(stderr, "Incorrect file format - tests\n");
    }

    start_index = findIndex(A, places, hashtable);
    destination_index = findIndex(B, places, hashtable);


    int final_dist = findDist(start_index, destination_index, num_places,
       places, paths, heap, heap_size); //??
    if (final_dist == -1)
    {
      printf("\n%s and %s are not connected\n", places[start_index].place, places[destination_index].place);
    }
    else
    {
      printf("%s -> %s: %d\n", places[start_index].place,
       places[destination_index].place, final_dist);
    }

    //reset all places values
    for (int i = 0; i < num_places; i++)
    {
      places[i].dist = 0;
      places[i].fixed = false;
      places[i].heap_index = -1;
      *heap_size = 1;
      //reinitialise heap[] ??
    }
  }
  //free the heap_size??
}

int findDist(int start_index, int destination_index, int num_places,
   node_t places[num_places], list_entry_t* paths[], int* heap_size)
{
  int num_loops = 0;
  int current_node_index;
  current_node_index = start_index;
  //printf("places[start_index].place: %s\n", places[start_index].place);

  while (1)
  {
    if (num_loops > num_places)
    {
      return -1;
    }
    if (destination_index == current_node_index)
    {
      //printf("places[destination_index].dist: %d\n", places[destination_index].dist);
      return places[destination_index].dist;
    }

    places[current_node_index].fixed = true;
    //int smallest_dist_index = -1;

    //updating loop
    list_entry_t* entry = paths[current_node_index];
    while (entry != NULL)
    {
      //if .fixed == false means if havent been here before
      if (entry->dist > 0 && places[entry->index].fixed == false)
      {

        //if this route to the node is shorter, update
        if (places[current_node_index].dist
          + entry->dist < places[entry->index].dist
          || places[entry->index].dist == 0)
        {
          places[entry->index].dist =
          places[current_node_index].dist + entry->dist;
          heap_insert(current_node_index, heap, heap_size); //make heap_size an int pointer??
        }
      }

      entry = entry->next;
    }

    //find smallest
    smallest_dist_index = extract_heap_min(heap, heap_size);
    /*for (int i = 0; i < num_places; i++)
    {
      if ((smallest_dist_index == -1 || places[i].dist < places[smallest_dist_index].dist)
         && places[i].dist > 0 && places[i].fixed == false)
      {
        smallest_dist_index = i;
        //printf("smallest_dist_index: %d  j: %d\n", smallest_dist_index, j);
      }
    }*/
    current_node_index = smallest_dist_index;
    num_loops++;
  }
}

int findIndex(char place[], node_t places[], hashtable_entry_t* hashtable[])
{
  int hash = hash_function(place);
  hashtable_entry_t* head = hashtable[hash];
  if (head->next == NULL)
  {
    return head->index;
  }
  while (head)
  {
    if (strcmp(places[head->index].place, place) == 0)
    {
      return head->index;
    }
    head = head->next;
  }
  printf("Error in findIndex\n");
  return 1; //??
}

static void heap_shift_up(int* heap[], int heap_index)
{
  while (heap_index > 1)
  {
    parent_place_index = heap[heap_index / 2]
    child_place_index = heap[heap_index]
    if (place[parent_place_index].dist > place[child_place_index].dist)
    {
      place[parent_place_index].heap_index = heap_index;
      place[child_place_index].heap_index = heap_index / 2;

      int tmp = child_place_index;
      child_place_index = parent_place_index;
      parent_place_index = tmp;

      heap_index /= 2;
    }
    else
    {
      break;
    }
  }
}

static void heap_shift_down(int* heap[], int heap_index, int* heap_size)
{
  while (heap_index * 2 < *heap_size)
  {
    child_place_index = heap[heap_index * 2]
    parent_place_index = heap[heap_index]
    if (place[parent_place_index].dist > place[child_place_index].dist)
    {
      place[child_place_index].heap_index = heap_index;
      place[parent_place_index].heap_index = heap_index * 2;

      int tmp = child_place_index;
      child_place_index = parent_place_index;
      parent_place_index = tmp;

      heap_index *= 2;
    }
    else
    {
      break;
    }
  }
}


//INITIAL SIZE OF HEAP IS 1
static void heap_insert(int heap_entry, int* heap[], int* heap_size)
{
  heap[*heap_size] = heap_entry;
  place[heap_entry].heap_index = *heap_size;
  heap_shift_up(heap, heap_size);
}

static int extract_heap_min(int* heap[], int* heap_size)
{
  int min = heap[1];
  heap[1] = heap[*heap_size]

  *heap_size--;
  heap_shift_down(heap, 1, heap_size);
  return min;
}
