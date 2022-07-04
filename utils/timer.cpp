#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "misc.cpp"
#ifndef STB_DS_IMPLEMENTATION
#define STB_DS_IMPLEMENTATION
#include "stb/stb_ds.h"
#endif

typedef void (*timer_callback)(uint32, void* param);

struct timer_handler
{
  struct timer
  {
    real64 start_time;
    int32 tick_count;
    bool reset_flag;
    bool paused;
    real32 tick_period;
    real32 elapsed_time; // sub tick period elapsed time. used for 
                         //pausing/resuming
    timer_callback callback;
    void *param;
#ifdef DEBUG
    bool debug;
#endif
  };

  struct id_timer
  {
    uint32 key;
    timer value;
  };

  real64 *cur_time;

  id_timer *timers = 0;
  uint32 last_timer = 0;

  timer_handler(real64 *cur_time)
  :cur_time(cur_time)
  {

  }
  
  uint32 add(real32 tick_period, timer_callback callback, void *param = 0
#ifdef DEBUG
    ,bool debug = false
#endif
      )
  {
    timer new_timer; 
    new_timer.start_time = *cur_time;
    new_timer.tick_count = 0;
    new_timer.reset_flag = false;
    new_timer.paused = false;
    new_timer.tick_period = tick_period;
    new_timer.elapsed_time = 0.f;
    new_timer.callback = callback;
    new_timer.param = param;
#ifdef DEBUG
    new_timer.debug = debug;
#endif
    
    uint32 timer_id = last_timer++;
    hmput(timers, timer_id, new_timer);
    return timer_id;
  }

  timer* get_timer(uint32 id)
  {
    ASSERT(hmgeti(timers,id) >= 0);
    timer *tim = &hmget(timers,id);
    return tim;
  }

  void set_param(uint32 id, void *param)
  {
    timer *tim = get_timer(id);
    tim->param = param;
  }

  void del(uint32 id)
  {
    hmdel(timers, id);
  }

  void pause(uint32 id)
  {
    timer *tim = get_timer(id);

    //storing elapsed time
    //in start_time
    tim->elapsed_time =  *cur_time - 
      (tim->start_time+tim->tick_period *
                      (tim->tick_count));
    tim->paused = true;
  }

  void resume(uint32 id)
  {
    timer *tim = get_timer(id);
    if(!tim->paused)
      return;
    tim->start_time = *cur_time - tim->elapsed_time;
    tim->tick_count = 0;
    tim->paused = false;
  }

  void toggle(uint32 id)
  {
    timer *tim = get_timer(id);
    if(tim->paused)
      resume(id);
    pause(id);
  }

  void reset(uint32 id)
  {
    timer *tim = get_timer(id);
    tim->reset_flag = true;
  }

  void reset_all()
  {
    for(uint32 i=0;i<hmlen(timers);++i)
    {
      timer *tim = &timers[i].value;
      tim->reset_flag = true;
    }
  }

  void change_tick(uint32 id, real32 new_tick)
  {
    timer *tim = get_timer(id);
    tim->tick_period = new_tick;
  }

  void tick()
  {
    for(uint32 i=0;i<hmlen(timers);++i)
    {
      timer *tim = &timers[i].value;

#ifdef DEBUG
      if(tim->debug)
      {
        printf("--------\n");
        printf("DEBUG - ticking timer id n: %i\n"
               "start time: %f\n"
               "cur time: %f\n"
               "elapsed time: %f\n"
               "tick count: %i\n"
               "tick period: %f\n"
               "paused? %i\n",

               timers[i].key,
               tim->start_time,
               *cur_time,
               tim->elapsed_time,
               tim->tick_count,
               tim->tick_period,
               tim->paused);

      }

#endif

      if(tim->reset_flag)
      {
#ifdef DEBUG
        if(tim->debug)
          printf("timer %i RESET\n", timers[i].key);
#endif
        tim->start_time = *cur_time;
        tim->tick_count = 0;
        tim->reset_flag = false;
        tim->elapsed_time = 0.f;
      }
      else if(tim->paused)
      {

      }
      else if(*cur_time > tim->start_time +
          tim->tick_period *
          (tim->tick_count + 1))
      {
        tim->callback(timers[i].key, tim->param);
        ++tim->tick_count;
      }
    }
  }
};

