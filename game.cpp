#pragma once

enum class GameState
{
  menu,
  gameplay
} game_state;

struct
{
  Font *pixely_20;
  Font *pixely_50;
} fonts;

void wasd_control(glm::vec2 *pos)
{
  if(key_get('w'))
    pos->y += 2.f;
  if(key_get('s'))
    pos->y -= 2.f;
  if(key_get('d'))
    pos->x += 2.f;
  if(key_get('a'))
    pos->x -= 2.f;
}

void plus_minus_control(uint32 *val)
{
  if(key_get_down('+'))
    *val += 2;
  if(key_get_down('-'))
    *val -= 2;
}

void plus_minus_control(real32 *val)
{
  if(key_get_down('+'))
    *val += 2.f;
  if(key_get_down('-'))
    *val -= 2.f;
}

void plus_minus_control(int32 *val)
{
  if(key_get_down('+'))
    *val += 2;
  if(key_get_down('-'))
    *val -= 2;
}

//how many pixels are the paddles away from the screen borders
glm::vec2 get_screen_middle()
{
  return glm::vec2((real32)SCREEN_WIDTH / 2.f,
      (real32)SCREEN_HEIGHT / 2.f);
}

void switch_game_state(GameState);

#include "menu.cpp"
#include "gameplay.cpp"

void switch_game_state(GameState state)
{
  game_state = state;
  if(game_state == GameState::gameplay)
    reset_game();
}

void game_init()
{
  //loading all the fonts of the game
  fonts.pixely_20 = font_new("Minercraftory.ttf", 20);
  fonts.pixely_50 = font_new("Minercraftory.ttf", 50);

  menu_init();
  gameplay_init();
}

void game_loop(real32 frame_time)
{
  switch(game_state)
  {
    case GameState::menu:
      menu_loop(frame_time);
      break;
    case GameState::gameplay:
      gameplay_loop(frame_time);
      break;
  }
}
