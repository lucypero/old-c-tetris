#include <stdio.h>
#include <stdlib.h>

size_t get_file_size(FILE *file)
{
  long current_pos = ftell(file);
  size_t size = 0;
  fseek(file, 0, SEEK_END);
  size = ftell(file);
  fseek(file, current_pos, SEEK_SET);
  return size;
}

//NOTE: allocates a buffer and puts entire file in it
char* read_entire_file(const char* file_name)
{
  FILE *file = fopen(file_name, "r");
  if(!file)
  {
    perror("Error reading file");
    exit(1);
  }

  size_t file_size = get_file_size(file);

  char *file_buf = (char*)malloc(file_size+1);

  char char_read;
  unsigned int cursor = 0;
  
  while((char_read = fgetc(file)) != EOF)
  {
    file_buf[cursor] = char_read;
    ++cursor;
  }

  file_buf[cursor] = 0;

  fclose(file);

  // printf("file contents:\n%s", file_buf);

  return file_buf;
}
