#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define DICT_CAPACITY 4096 //2^12 = 4096

typedef struct entry {
  uint8_t* ptr;
  size_t size;
} entry_t;

static int decoder(int count, unsigned int codes[], entry_t dict[], FILE* file2);
static void reset_dict(entry_t dict[]);
static entry_t append(entry_t source_entry, char c);

static int marker = 256;

int main (int argc, char* argv[])
{
  if (argc != 3)
  {
    printf("Usage: lzw.c [compressed_file] [decompressed_file_name]\n");
    return 1;
  }

  char* input = argv[1];
  FILE* file = fopen(input, "r");
  if (file == NULL)
  {
    printf("File does not exist\n");
    return 1;
  }

  char* output = argv[2];
  FILE* file2 = fopen(output, "w");

  entry_t dict[DICT_CAPACITY];
  for (int i = 0; i < DICT_CAPACITY; i++)
  {
    dict[i].ptr = NULL;
    dict[i].size = 0;
  }

  for (int i = 0; i < 256; i++)
  {
    dict[i] = append(dict[i], i);
    if (dict[i].ptr == NULL)
    {
      printf("dict[i] pointer is null - initialise dict\n\n");
    }
  }

  unsigned char c[3];
  unsigned int codes[4096];
  int count = 0;

  while (1)
  {
    int bytes = fread(c, 1, 3, file);

    if (bytes == 3)
    {
      unsigned int code1 = ((c[0] << 4) | (c[1] >> 4)) % 4096;
      unsigned int code2 = ((c[1] & 0x0f) << 8) | (c[2]);
      codes[count] = code1;
      codes[count+1] = code2;
      count += 2;
    }
    else if (bytes == 2)
    {
      unsigned int code1 = (c[0] << 8 | c[1]);
      codes[count] = code1;
      count++;
    }

    if (count == 4096 || count == 4095)
    {
      decoder(count, codes, dict, file2);
      codes[0] = codes[count - 1];
      for (int i = 1; i < count; i++)
      {
        codes[i] = 0;
      }
      count = 1;
    }
    else if (feof(file) != 0)
    {
      decoder(count, codes, dict, file2);
      fwrite(dict[codes[count-1]].ptr, dict[codes[count-1]].size, 1, file2);

      break;
    }
  }
  fclose(file);
  fclose(file2);
}

static int decoder(int count, unsigned int codes[], entry_t dict[], FILE* file2)
{
  for (int i = 0; i < count; i++)
  {
    int code1 = codes[i];
    if (i == count-1)
    {
      return 1;
    }
    else
    {
      int code2 = codes[i+1];
      char s;
      if (dict[code2].ptr != NULL)
      {
        s = dict[code2].ptr[0];
      }
      else
      {
        s = dict[code1].ptr[0];
      }

      fwrite(dict[code1].ptr, dict[code1].size, 1, file2);

      dict[marker] = append(dict[code1], s);
      marker++;

      if (marker == DICT_CAPACITY)
      {
        reset_dict(dict);
        marker = 256;
      }
    }
  }
}

static void reset_dict(entry_t dict[])
{
  for (int i = 256; i < DICT_CAPACITY; i++)
  {
    free(dict[i].ptr);
    dict[i].ptr = NULL;
    dict[i].size = 0;
  }
}

static entry_t append(entry_t source_entry, char c)
{
  entry_t new_entry;
  if (source_entry.ptr == NULL)
  {
    printf("lzw malloc 1\n");
    new_entry.ptr = malloc(1);

    if (new_entry.ptr == NULL)
    {
      printf("Malloc failed in append()\n");
      exit(1);
    }
    else
    {
      new_entry.ptr[0] = c;
      new_entry.size = 1;
      printf("lzw malloc 1 SUCCESS\n");

    }
  }
  else
  {
    printf("lzw malloc source_entry.size +1\n");

    new_entry.ptr = malloc(source_entry.size + 1);
    if (new_entry.ptr == NULL)
    {
      printf("Malloc failed in append()\n");
      exit(1);
    }
    printf("lzw malloc source_entry.size +1 SUCCESS\n");

    new_entry.size = source_entry.size + 1;

    memcpy(new_entry.ptr, source_entry.ptr, source_entry.size);
    new_entry.ptr[source_entry.size] = c;
  }
  return new_entry;
}
