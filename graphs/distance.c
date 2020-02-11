#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define WORD_LEN 45

typedef struct node {
  int dist;
  bool beenHere;
} node_t;

int findDist(char A[], char B[], int already_travelled, int full_distance,
  int num_places, node_t matrix[num_places][num_places], char places[num_places][WORD_LEN]);
int matrix_pos(int x, int y, int num_places);

int main (int argc, char* argv[])
{
  if (argc != 2)
  {
    printf("Please provide an input file\n");
    return 1;
  }

  char* in = argv[1];
  FILE* input = fopen(in, "r");

  int num_places;
  if (fscanf(input, " %i\n", &num_places) != 1)
  {
    printf("Incorrect file format - num_places\n");
    return 1;
  }

  char places[num_places][WORD_LEN];
  int result;
  for (int i = 0; i < num_places; i++)
  {
    if (fscanf(input, " %s", places[i]) != 1)
    {
      printf("Incorrect file format - places, i: %d\n", i);
      return 1;
    }
    //printf("place number %i: %s\n", i, places[i]);
  }

  int num_paths;
  if (fscanf(input, " %i\n", &num_paths) != 1)
  {
    printf("Incorrect file format - num_paths\n");
    return 1;
  }

  //int *matrix = malloc(num_places * num_places * sizeof(int));
  node_t matrix[num_places][num_places];
  for (int i = 0; i < num_places; i++)
  {
    for (int j = 0; j < num_places; j++)
    {
      matrix[i][j].beenHere = false;

      if (i == j)
      {
        matrix[i][j].dist = 0;
        //matrix_pos(i, j, num_places) = 0;
      }
      else
      {
        matrix[i][j].dist = -1;
        //matrix_pos(i, j, num_places) = 0;
      }
    }
  }
  char start[WORD_LEN];
  char dest[WORD_LEN];
  int distance, start_position, dest_position;

  for (int i = 0; i < num_paths; i++)
  {
    if (fscanf(input, "%s %s %i\n", start, dest, &distance) != 3)
    {
      printf("Incorrect file format - paths\n");
      return 1;
    }

    for (int i = 0; i < num_places; i++)
    {

      if (strcmp(start, places[i]) == 0)
      {
        start_position = i;
      }
      if (strcmp(dest, places[i]) == 0)
      {
        dest_position = i;
      }
    }
    if (matrix[start_position][dest_position].dist == -1 ||
      matrix[start_position][dest_position].dist > distance)
    {
      matrix[start_position][dest_position].dist = distance;
      matrix[dest_position][start_position].dist = distance;
    }
    /*if (matrix_pos(start_position, dest_position, num_places) == -1 || matrix_pos(start_position, dest_position, num_places) > distance)
    {
      matrix_pos(start_position, dest_position, num_places) = distance;
      matrix_pos(dest_position, start_position, num_places) = distance;
    }*/

  }
  //print matrix
  for (int i = 0; i < num_places; i++)
  {
    for (int j = 0; j < num_places; j++)
    {
      printf("%3i ", matrix[i][j].dist);
    }
    printf("\n");
  }

  while (feof(input) == 0)
  {
    char A[WORD_LEN];
    char B[WORD_LEN];

    fscanf(input, " %s %s\n", A, B);
    int final_dist = findDist(A, B, 0, 0, num_places, matrix, places);
    printf("%s -> %s: %d\n", A, B, final_dist);

    //reset all beenHere values to false
    for (int i = 0; i < num_places; i++)
    {
      for (int j = 0; j < num_places; j++)
      {
        matrix[i][j].beenHere = false;
      }
    }
  }
}

int findDist(char A[], char B[], int already_travelled, int full_distance,
  int num_places, node_t matrix[num_places][num_places], char places[num_places][WORD_LEN])
{
  //for loop = iterate through places to find the correct column in the matrix
  //(use if strcmp of places[i] and A = 0, then use ith column of matrix)
  int column = 0;
  for (int i = 0; i < num_places; i++)
  {
    if (strcmp(places[i], A) == 0)
    {
      column = i;
      break;
    }
  }
  //iterate down the column of the matrix
  for (int j = 0; j < num_places; j++)
  {
    if (matrix[column][j].dist > 0 && matrix[column][j].beenHere == false)
    {
      int to_travel = already_travelled + matrix[column][j].dist;

      if (full_distance == 0 || to_travel < full_distance)
      {
        if (strcmp(places[j], B) == 0)
        {
          full_distance = to_travel;
        }
        else
        {
          matrix[column][j].beenHere = true;
          matrix[j][column].beenHere = true;
          full_distance = findDist(places[j], B, to_travel, full_distance, num_places, matrix, places);
          matrix[column][j].beenHere = false;
          matrix[j][column].beenHere = false;
        }
      }
    }
  }
  return full_distance;
}
