#include <stdio.h>
#include "../utils/timer.cpp"

real64 cur_time = 0.f;
timer_handler timer_han(&cur_time);
uint32 counter = 0;

void count(uint32 id)
{
  ++counter;
  printf("counted up. counter is %i\n", counter);
}

uint32 timer_to_pause;

void print_hello(uint32 timer_id)
{
  printf("hello\n");
}

bool is_paused = false;
 
void pausing_timer(uint32 timer_id)
{
  if(!is_paused)
  {
    printf("pausing pause timer\n");
    timer_han.pause(timer_to_pause);
    is_paused = true;
  }
  else
  {
    printf("resuming pause timer\n");
    timer_han.resume(timer_to_pause);
    timer_han.del(timer_id);
  }
}

void print_paused(uint32 timer_id)
{
  printf("timer id: %i  - hello - timer to pause\n", timer_id);
  printf("cur time: %f\n", cur_time);
}

int main()
{
  real32 timer_tick = 1000.f;
  bool timer_deleted = false;

  // uint32 counter_id = timer_han.add(timer_tick, count);
  // uint32 hello_printer_id = timer_han.add(500.f, print_hello);

  //testing pause feature
  uint32 timer_that_pauses = timer_han.add(1000.f, pausing_timer);
  timer_to_pause = timer_han.add(3000.f, print_paused);

  while(true)
  {
    real32 delta = 10.f;
    nsleep((uint32)delta);
    cur_time+= delta;
    timer_han.tick();

//     if(counter == 5)
//     {
//       if(!timer_deleted)
//       {
//         printf("deleting hello timer\n");
//         timer_han.del(hello_printer_id);
//         timer_deleted = true;
//       }

//       counter = 0;
//       timer_han.reset(counter_id);
//       timer_tick /= 2.f;
//       if(timer_tick <= 20.f)
//         timer_tick = 1000.f;
//       timer_han.change_tick(counter_id, timer_tick);
//     }
  }
}
