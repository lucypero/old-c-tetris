#pragma once

#define MENU_GAP 40

const char* options[] = {"START", "MULTIPLAYER", "EXIT"};
FontSprite *f_spr_title;
FontSprite *f_spr_option;

int32 current_option = 0;

Sprite *spr_arrow;

void menu_init()
{
  f_spr_title = font_sprite_new(fonts.pixely_50);
  f_spr_title->pos = get_screen_middle();
  f_spr_title->pos.y = SCREEN_HEIGHT - 70.f;

  f_spr_option = font_sprite_new(fonts.pixely_20);
  
  spr_arrow = sprite_new("arrow.png");
  spr_arrow->tint_color = glm::vec4(0.8f,0.8f,0.8f,1.f);
  spr_arrow->pivot_pos = PivotPosition::center;
}

void menu_loop(real32 frame_time)
{
  if(key_get_down(SDLK_UP))
  {
    --current_option;
    if(current_option < 0)
     current_option = 0;
  }

  if(key_get_down(SDLK_DOWN))
  {
    ++current_option;
    if(current_option > ARR_COUNT(options) - 1)
      current_option = ARR_COUNT(options) - 1;
  }

  font_sprite_draw(f_spr_title, "PEROPERO'S TETRIS");

  f_spr_option->pos = get_screen_middle();

  for(int32 i = 0; i< ARR_COUNT(options); ++i)
  {
    Rect font_rect = font_sprite_draw(f_spr_option, options[i]);

    if(i == current_option)
    {
      spr_arrow->pos = f_spr_option->pos;
      spr_arrow->pos.x = font_rect.x - 20.f;
      sprite_draw(spr_arrow);
    }

    f_spr_option->pos.y -= MENU_GAP;
  }

  if(key_get_down(SDLK_RETURN))
  {
    switch (current_option)
    {
      case 0:
        switch_game_state(GameState::gameplay);
        break;
      case 2:
        quit_game();
        break;
    }
  }
}
