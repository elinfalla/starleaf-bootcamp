#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define WORD_LEN 45

typedef struct node {
  char place[WORD_LEN];
  int dist;
  bool fixed;
} node_t;

int findDist(int start_index, int destination_index, int num_places,
  int** matrix, node_t places[num_places]);
int matrix_pos(int x, int y, int num_places);

int main (int argc, char* argv[])
{
  if (argc != 2)
  {
    fprintf(stderr, "Please provide an input file\n");
    return 1;
  }

  char* in = argv[1];
  FILE* input = fopen(in, "r");

  int num_places;
  if (fscanf(input, " %i\n", &num_places) != 1)
  {
    fprintf(stderr, "Incorrect file format - num_places\n");
    return 1;
  }

  node_t places[num_places];
  int result;
  for (int i = 0; i < num_places; i++)
  {
    if (fscanf(input, " %s", places[i].place) != 1)
    {
      fprintf(stderr, "Incorrect file format - places, i: %d\n", i);
      return 1;
    }
    places[i].fixed = false;
    places[i].dist = 0;
  }

  int num_paths;
  if (fscanf(input, " %i\n", &num_paths) != 1)
  {
    fprintf(stderr, "Incorrect file format - num_paths\n");
    return 1;
  }
  printf("killed yet? line 55\n");
  int** matrix = malloc(sizeof(int*) * num_places);
  for (int i = 0; i < num_places; i++)
  {
    matrix[i] = malloc(sizeof(int) * num_places);
    for (int j = 0; j < num_places; j++)
    {
      if (i == j)
      {
        matrix[i][j] = 0;
        //matrix_pos(i, j, num_places) = 0;
      }
      else
      {
        matrix[i][j] = -1;
        //matrix_pos(i, j, num_places) = 0;
      }
    }
  }
  printf("killed yet? line 74\n");
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

    for (int i = 0; i < num_places; i++)
    {

      if (strcmp(start, places[i].place) == 0)
      {
        start_index = i;
      }
      if (strcmp(dest, places[i].place) == 0)
      {
        destination_index = i;
      }
      if (destination_index != -1 && start_index != -1)
      {
        break;
      }

    }
    /*if (start_index == -1 || destination_index == -1)
    {
      fprintf(stderr, "Spelling error - paths\n");
      return 1;
    }*/
    if (matrix[start_index][destination_index] == -1 ||
      matrix[start_index][destination_index] > distance)
    {
      matrix[start_index][destination_index] = distance;
      matrix[destination_index][start_index] = distance;
    }
    start_index = -1;
    destination_index = -1;

  }
  /*for (int i = 0; i < num_places; i++)
  {
    for (int j = 0; j < num_places; j++)
    {
      printf("%3i ", matrix[i][j]);
    }
    printf("\n");
  }*/

  while (feof(input) == 0)
  {
    char A[WORD_LEN];
    char B[WORD_LEN];

    if (fscanf(input, " %s %s\n", A, B) != 2)
    {
      fprintf(stderr, "Incorrect file format - tests\n");
    }
    //printf("A: %s B: %s\n", A, B);
    start_index = -1;
    destination_index = -1;
    for (int i = 0; i < num_places; i++)
    {
      if (strcmp(A, places[i].place) == 0)
      {
        start_index = i;
        //printf("start_index: %d\n", start_index);
      }
      if (strcmp(B, places[i].place) == 0)
      {
        destination_index = i;
      }
    }
    if (start_index == -1 || destination_index == -1)
    {
      fprintf(stderr, "Spelling error - tests\n");
      return 1;
    }
    /*for (int i = 0; i < num_places; i++)
    {
      printf("%s\n", places[i].place);
    }*/
    //printf("start_index: %d destination_index: %d num_places: %d\n", start_index, destination_index, num_places);
    int final_dist = findDist(start_index, destination_index, num_places,
       matrix, places);
    if (final_dist == -1)
    {
      printf("\n%s and %s are not connected\n", places[start_index].place, places[destination_index].place);
    }
    else
    {
      printf("\nXOXOXOXOXOXOXO %s -> %s: %d XOXOXOXOXOXOXO\n\n", places[start_index].place,
       places[destination_index].place, final_dist);
    }

    //reset all beenHere values to false
    for (int i = 0; i < num_places; i++)
    {
      places[i].dist = 0;
      places[i].fixed = false;
    }
  }
  for (int i = 0; i < num_places; i++)
  {
    free(matrix[i]);
  }
  free(matrix);
  fclose(input);
}

int findDist(int start_index, int destination_index, int num_places,
  int** matrix, node_t places[num_places])
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
    int smallest_dist_index = -1;

    //updating loop
    for (int j = 0; j < num_places; j++)
    {
      //if .fixed == false means if havent been here before
      if (matrix[current_node_index][j] > 0 && places[j].fixed == false)
      {
        //printf("into if statement\n");
        //if this route to the node is shorter, update
        if (places[current_node_index].dist
          + matrix[current_node_index][j] < places[j].dist
          || places[j].dist == 0)
        {
          places[j].dist =
          places[current_node_index].dist + matrix[current_node_index][j];
        }

        //printf("places[j].place: %s  places[j].dist: %d  places[j].fixed: %d\n",
         //places[j].place, places[j].dist, places[j].fixed);

      }
      /*if (smallest_dist_index == -1)
      {
        printf("\nsmallest_dist_index == -1\n");
      }
      else
      {
        printf("smallest: %d\n", smallest_dist_index);
      }*/
    }
    //find smallest
    for (int i = 0; i < num_places; i++)
    {
      if ((smallest_dist_index == -1 || places[i].dist < places[smallest_dist_index].dist)
         && places[i].dist > 0 && places[i].fixed == false)
      {
        smallest_dist_index = i;
        //printf("smallest_dist_index: %d  j: %d\n", smallest_dist_index, j);
      }
    }
    current_node_index = smallest_dist_index;
    num_loops++;
  }
}

/*
current node
look at all adjacent nodes
pick the  unfixed node with the shortest distance from start node
fix tht node
current node = that node

while B.fixed == false
*/
