#pragma once
#include <stdio.h>
#include <stdlib.h>

#ifdef DEBUG
#ifdef _MSC_VER
#define DBREAK() __debugbreak()
#else
#include <signal.h>
#define DBREAK() raise(SIGTRAP)
#endif
#endif
#ifndef DEBUG
#define DBREAK() 
#endif

#define ASSERT(x) if(!(x)) \
{\
  fprintf(stderr,"%s:%i: error: Assertion failed: %s\n", \
      __FILE__,__LINE__,#x); \
  DBREAK();\
  exit(1);\
}

#define _a ASSERT

#define _l(...) printf(__VA_ARGS__);printf("\n")
//OP printf
#define _dl(...) \
  printf("%s:%i - %s() - ",__FILE__,__LINE__,__FUNCTION__);\
  printf(__VA_ARGS__);printf("\n")
