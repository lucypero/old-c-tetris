#include "utils/types.h"

#define STB_DS_IMPLEMENTATION
#include "stb/stb_ds.h"


int32 key_strings[] = 
{
  'a', 's', 'd', 'w',
  SDLK_KP_PLUS, SDLK_KP_MINUS,
  SDLK_LEFT, SDLK_RIGHT, SDLK_DOWN,
  SDLK_UP, SDLK_SPACE, SDLK_RETURN, 'r', 'h',
  'j', 'u', 'l', 'k'
};

#define KEYS_COUNT ARR_COUNT(key_strings)

//keyboard snapshot of previous frame
bool keys_prev_frame[KEYS_COUNT] = {};
//true when key is being pressed
bool keys[KEYS_COUNT] = {};

struct
{
  int32 key;
  uint32 value;
}
*keys_map = 0;

void input_init()
{
  for(uint32 i=0;i<KEYS_COUNT;++i)
    hmput(keys_map, key_strings[i], i);
}

//returns 1 if the program should quit
bool process_input()
{
  SDL_Event event;

  memcpy(keys_prev_frame, keys, KEYS_COUNT);

  while(SDL_PollEvent(&event))
  {
    if(event.type == SDL_QUIT)
    {
      return 1;
    }

    if(event.type == SDL_KEYDOWN ||
        event.type == SDL_KEYUP)
    {
      int32 keycode = (int32)event.key.keysym.sym;

      if(hmgeti(keys_map, keycode) < 0)
        continue;

      bool keydown = event.type == SDL_KEYDOWN;
      uint32 key = hmget(keys_map, keycode);
      keys[key] = keydown;
    }
  }
  return 0;
}

//returns true if key is being held down
bool key_get(int32 key_name)
{
  uint32 key = hmget(keys_map, key_name);
  ASSERT(key >= 0 && key <= KEYS_COUNT);
  return keys[key];
}

//returns true on the first frame that the key is down
bool key_get_down(int32 key_name)
{
  uint32 key = hmget(keys_map, key_name);
  ASSERT(key >= 0 && key <= KEYS_COUNT);
  return keys[key] && !keys_prev_frame[key];
}

//returns true on the first frame that the key is up
bool key_get_up(int32 key_name)
{
  uint32 key = hmget(keys_map, key_name);
  ASSERT(key >= 0 && key <= KEYS_COUNT);
  return !keys[key] && keys_prev_frame[key];
}


