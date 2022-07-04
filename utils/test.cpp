#include "types.h"
#include "linux.h"
#include "random.cpp"
#include <stdio.h>
#include <assert.h>
#include <time.h>

#define TEST_ITERATIONS 5000

int main()
{
  char lol[] = "lol string";
  int x = 24;
  printf_error("lollol %s %i", lol, x);

  //testing random.cpp
  srand(time(0));
  
  for(uint32 i = 0; i<TEST_ITERATIONS;i++)
  {
    real32 r = rand_real32_normalized();
    assert(r >= 0.f);
    assert(r <= 1.f);
  }

  for(uint32 i = 0; i<TEST_ITERATIONS;i++)
  {
    uint32 r2 = ((rand() + 1) % 20) +1;
    uint32 r = rand_pick(r2);
    assert(r >= 0);
    assert(r < r2);
  }
  
  for(uint32 i = 0; i<TEST_ITERATIONS;i++)
  {
    uint32 r_from = 20;
    uint32 r_to = 50;
    uint32 r = rand_pick(r_from, r_to);
    assert(r >= r_from);
    assert(r < r_to);
  }

  for(uint32 i = 0; i<TEST_ITERATIONS;i++)
  {
    real32 r_from = 200.f;
    real32 r_to = 400.f;
    real32 r = rand_pick(r_from, r_to);
    assert(r >= r_from);
    assert(r < r_to);
  }
    
}
