#pragma once
#include <stdio.h>
#include <time.h>

#define ARR_COUNT(arr) (sizeof(arr) / sizeof(arr[0]))
//prints to stderr and exits with error code 1
void panic(const char *format, ...)
{
  //TODO: buffer overflow safety
  va_list argptr;
  va_start(argptr, format);
  char* buffer = (char*)malloc(40);
  vsprintf(buffer, format, argptr);
  va_end(argptr);
  fprintf(stderr, "%s", buffer);
  exit(1);
  free(buffer);
}

// int nsleep(long miliseconds)
// {
//   struct timespec req, rem;

//   if(miliseconds > 999)
//   {
//     req.tv_sec = (int)(miliseconds / 1000);
//     req.tv_nsec =
//       (miliseconds - ((long)req.tv_sec * 1000)) * 1000000;
//   }
//   else
//   {
//     req.tv_sec = 0;
//     req.tv_nsec = miliseconds * 1000000;
//   }

//   return nanosleep(&req , &rem);
// }
