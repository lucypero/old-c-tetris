#include <unistd.h>
#include <string.h>
#include <cstdio>
#include <cstdarg>
#include <cstdlib>

int get_last_slash_index(char* path)
{
  int i = 0, slash_index = 0;
  while(path[i] != '\0')
  {
    if(path[i] == '/')
      slash_index = i;
    ++i;
  }
  return slash_index;
}

//sets working dir relative to exe path
int set_working_dir(const char* path)
{
  //getting exe dir
  char path_buffer[200];
  readlink("/proc/self/exe",path_buffer,200);
  
  //cutting out the name of the executable
  {
    int i = get_last_slash_index(path_buffer);
    //replace that index +1 with '\0'
    path_buffer[i+1] = '\0';
  }

  //appending argument to path
  strcat(path_buffer, path);
  
  //setting working dir
  return chdir(path_buffer);
}

//NOTE: perror with printf capabilities
void printf_error(const char *format, ...)
{
  //TODO: buffer overflow safety
  va_list argptr;
  va_start(argptr, format);
  char* buffer = (char*)malloc(40);
  vsprintf(buffer, format, argptr);
  va_end(argptr);
  perror(buffer);
  free(buffer);
}
