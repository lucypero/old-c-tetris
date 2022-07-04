#include "types.h"
#include <stdlib.h>

//random number between 0 and 1
real32 rand_real32_normalized()
{
  return rand() / (real32)RAND_MAX;
}

//picks random no. between 0 and to (not inclusive)
uint32 rand_pick(uint32 to)
{
  return rand() % to;
}

uint32 rand_pick(uint32 from, uint32 to)
{
  return rand() % (to - from) + from;
}

real32 rand_pick(real32 to)
{
  return (real32)rand()/(real32)(RAND_MAX/to);
}

real32 rand_pick(real32 from, real32 to)
{
  real32 scale = rand() / (real32) RAND_MAX;
  return from + scale * ( to - from );
}
