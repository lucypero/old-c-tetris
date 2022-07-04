#pragma once
// the board coordinates are:
//bottom left: (0,0)
//top right:(CELL_X_COUNT - 1, CELL_Y_COUNT - 1)

#include "utils/matrix.cpp"
#include "utils/timer.cpp"

#define BOARD_BG_COLOR c_grey

#define CELL_X_COUNT 10
#define CELL_Y_COUNT 20

#define MAX_BLOCK_CELL_COUNT 4*4
#define NEXT_BLOCK_COUNT 5

#define CELL_SIZE 32 // in pixels

#define BOARD_X_OFFSET CELL_SIZE //pixels that the board will be away
//   from the left border of the screen
#define BOARD_Y_OFFSET CELL_SIZE // same for bottom border 

#define INITIAL_FALL_STEP 0.3f
#define INITIAL_SOFT_DROP_STEP 0.05f

real32 fall_step = INITIAL_FALL_STEP; // in seconds
real32 soft_drop_step = INITIAL_SOFT_DROP_STEP;

//timers

//turn time starts when cur block touches another block
#define MAX_TURN_TIME 5.0f
uint32 turn_timer;

//commitment time is time it will pass between the events:
// - cur block touches another block
// - player moves or rotates block
#define COMMITMENT_TIME 0.5f
uint32 commitment_timer;

// amount of time it takes when holding a move key before
// the block enters "automatic move mode" (i just made that up)
#define KEY_REPEAT_DELAY 0.2f
#define KEY_REPEAT_RATE 0.05f
uint32 key_repeat_timer;
int32 cur_block_dir = 1;

uint32 fall_timer;

timer_handler tim_han(&total_time);

struct block
{
  mat pos; //matrix that stores the block layout
  glm::vec4 color;
};

//gets height of the block. it only counts the slots 
//  that have solid cells in them
i32 block_get_true_height(block *blk)
{
  //to get the true height, i will substract all the rows with only
  //  zeroes in them from blk->pos.h

  i32 true_height = blk->pos.h;


  for(u32 c=0;c<blk->pos.h;++c)
  {
    bool non_zero_found = false;
    //row iterator
    for(u32 i=0;i<blk->pos.w;++i)
    {
      if(blk->pos.vals[i+(c*blk->pos.w)] != 0)
      {
        non_zero_found = true;
        break;
      }
    }
    //if non zeroes are found in the row, then we subtract the height
    // by one:
    if(!non_zero_found)
      --true_height;
  }


  return true_height;
}


//gets width of the block. it only counts the slots 
//  that have solid cells in them
i32 block_get_true_width(block *blk)
{

  i32 true_width = blk->pos.w;

  for(u32 r=0;r<blk->pos.w;++r)
  {
    bool non_zero_found = false;
    //row iterator
    for(u32 i=0;i<blk->pos.h;++i)
    {
      if(blk->pos.vals[r+(i*blk->pos.w)] != 0)
      {
        non_zero_found = true;
        break;
      }
    }
    if(!non_zero_found)
      --true_width;
  }

  return true_width;
}

// blocks are stored in memory from the bottom left block
// row major
uint32 blk_t_pos[] = {
  0, 0, 0,
  1, 1, 1,
  0, 1, 0,
};

uint32 blk_i_pos[] = {
  0, 0, 0, 0,
  0, 0, 0, 0,
  1, 1, 1, 1,
  0, 0, 0, 0
};

uint32 blk_j_pos[] = {
  0, 0, 0,
  1, 1, 1,
  1, 0, 0
};

uint32 blk_l_pos[] = {
  0, 0, 0,
  1, 1, 1,
  0, 0, 1
};

uint32 blk_o_pos[] = {
  1, 1,
  1, 1
};

uint32 blk_s_pos[] = {
  0, 0, 0,
  0, 1, 1,
  1, 1, 0,
};

uint32 blk_z_pos[] = {
  0, 0, 0,
  1, 1, 0,
  0, 1, 1,
};

block blk_t, blk_i,
      blk_j, blk_l,
      blk_o, blk_z,
      blk_s;

block *blocks[] = { &blk_t, &blk_i, &blk_j,
  &blk_l, &blk_o, &blk_z,
  &blk_s };

int32 cur_block_x,cur_block_y;
//memory will be allocated separately for the current block
// because it will mutate when it rotates
block cur_block;
uint32 cur_block_id;

//temporary block for calculations
block block_temp;

//hold slot (-1 when there is nothing in the hold slot)
int32 hold_block_id = -1;

//if player has used the hold key in this turn
bool was_hold_used;

//next blocks
//it's an array of indices to the blocks[] array
uint32 next_blocks[NEXT_BLOCK_COUNT];
int32 next_block_index = 0;

Sprite *spr_brick;
Sprite *spr_bg;
Sprite *spr_board_bg;
//storing the blocks placed on the board:
//for each cell, we need to know:
// if it is populated
// if it is, what color is it?

// NOTE: for now we'll just store cells as vec4's,
//  Replace with a cell struct if needed

// if color.x == -1, then cell is empty
glm::vec4 cells[CELL_X_COUNT*CELL_Y_COUNT];

//returns null ptr if out of range
glm::vec4* get_cell(int32 x, int32 y)
{
  if( y<0  ||
      x<0  ||
      x>=CELL_X_COUNT ||
      y>=CELL_Y_COUNT)
    return 0;
  return &cells[CELL_X_COUNT*y+x];
}

bool is_cell_empty(int32 x, int32 y)
{
  // cells that are above the board are always empty
  if(y >= CELL_Y_COUNT &&
     x >= 0 &&
     x < CELL_X_COUNT)
  {
    return true;
  }


  glm::vec4 *cell = get_cell(x,y);
  return cell != 0 && cell->x == -1;
}

block* get_random_block()
{
  return blocks[rand_pick((uint32)ARR_COUNT(blocks))];
}

void copy_block(block *blk_dest, block *block_src)
{
  memcpy(blk_dest->pos.vals, block_src->pos.vals,
      sizeof(uint32) * MAX_BLOCK_CELL_COUNT);
  blk_dest->pos.w = block_src->pos.w;
  blk_dest->pos.h = block_src->pos.h;
  blk_dest->color = block_src->color;
}

//returns true if block is unobstructed
// at the given position
bool does_block_fit(block *blk,
    int32 block_x,
    int32 block_y)
{
  //testing against other blocks
  for(uint32 i=0;i<blk->pos.w;++i)
  {
    for(uint32 j=0;j<blk->pos.h;++j)
    {
      if(blk->pos.vals[(j*(blk->pos.w))+i])
      {
        //testing if the coord is inside the board
        //otherwise it will trigger an assert if the block
        //is entering the board.

        // testing edges of screen
        uint32 pos_x = block_x+i;
        uint32 pos_y = block_y+j;

        if(pos_x >= CELL_X_COUNT ||
           pos_x < 0 ||
           pos_y < 0)
          return false;

        //if the block is entering the board (above), it fits
        // if(pos_y + 1 >= CELL_Y_COUNT)
        //   continue;

        if(!is_cell_empty(pos_x,pos_y))
          return false;
      }
    }
  }

  return true;
}


//if cur block is on top of placed block, commitment timer starts
void check_for_commitment()
{
  if(!does_block_fit(&cur_block,
        cur_block_x,
        cur_block_y - 1))
  {
    tim_han.resume(commitment_timer);
    tim_han.resume(turn_timer);
  }
}

//swaps the current block with one from the next block stack.
// if from_hold_slot flag is set to true, it swaps it with
// the block from the hold slot (if there is one)
void reset_current_block(bool from_hold_slot = false)
{
  //checking if there is a block in the hold slot
  if(from_hold_slot && hold_block_id == -1)
  {
    hold_block_id = cur_block_id;
    from_hold_slot = false;
  }

  uint32 prev_block_id = cur_block_id;

  if(!from_hold_slot)
    cur_block_id = next_blocks[next_block_index];
  else
    cur_block_id = hold_block_id;

  cur_block_x = CELL_X_COUNT / 2 - 1;
  cur_block_y = CELL_Y_COUNT;
  

  ASSERT(cur_block_id<ARR_COUNT(blocks));
  ASSERT(cur_block_id>=0);
  block *new_block = blocks[cur_block_id];
  copy_block(&cur_block, new_block);


  //if first row is all zeroes, place the block 1 cell lower
  {
    i32 empty_rows = 0;
    for(u32 row_pos=0;row_pos<cur_block.pos.h;++row_pos)
    {
      bool non_zero_found = false;
      //row iterator
      for(u32 i=0;i<cur_block.pos.w;++i)
      {
        if(cur_block.pos.vals[i+(cur_block.pos.w*row_pos)] != 0)
        {
          non_zero_found = true;
          break;
        }
      }
      if(!non_zero_found)
        ++empty_rows;
      else
        break;
    }
    //if non zeroes are found in the row, then we subtract the height
    // by one
    cur_block_y -= empty_rows;
  }


  if(!from_hold_slot)
  {
    next_blocks[next_block_index] =
      rand_pick((uint32)ARR_COUNT(blocks));
    ++next_block_index%=NEXT_BLOCK_COUNT;
  }
  else
  {
    hold_block_id = prev_block_id;
  }
  check_for_commitment();

  tim_han.reset(commitment_timer);
  tim_han.pause(commitment_timer);
  tim_han.reset(turn_timer);
  tim_han.pause(turn_timer);
}

//places block at the given position on the board
void place_block(block *blk, i32 x, i32 y)
{
  for(uint32 i=0;i<blk->pos.w;++i)
  {
    for(uint32 j=0;j<blk->pos.h;++j)
    {
      if(blk->pos.vals[(j*(blk->pos.w))+i] == 1)
      {
        glm::vec4 *cell = get_cell(x + i, y + j);
        ASSERT(cell);
        *cell = blk->color;
      }
    }
  }
}

//checks if there are solid lines
// (complete rows of blocks)
// and it clears them if there are
// and it places all the blocks on the top down
void clear_solid_lines()
{

  bool row_is_solid = true;
  //TODO: for optimization,
  // you actually just have to check
  // the rows where the last block fell

  //i'll loop from top to bottom
  //because this way, the row index
  //will not change even when shifting
  //the blocks when clearing lines
  for(int32 i=CELL_X_COUNT*CELL_Y_COUNT-1;i>=0;--i)
  {

    int32 x = i % CELL_X_COUNT;
    int32 y = i / CELL_X_COUNT;

    if(is_cell_empty(x,y))
    { 
      row_is_solid = false;
      //TODO:jump to next column
    }

    if(x == 0)
    {
      // row scanned, checking if all blocks were full
      //NOTE: do not read x and y values here.
      //     (they will be wrong when i=-1)
      if(row_is_solid)
      {
        //clear line
        int32 start_block = (y)*CELL_X_COUNT;
        int32 end_block = start_block+CELL_X_COUNT-1;

        for(int32 j=start_block;j<=end_block;j++)
        {
          cells[j] = glm::vec4(-1);
        }
        //shift all blocks above this line 1 row down

        //loops board from start of cleared row to top
        for(int32 j=start_block;j<CELL_X_COUNT*CELL_Y_COUNT;
            ++j)
        {

          // if on top tow, clear it
          if(j/CELL_X_COUNT == CELL_Y_COUNT - 1)
            cells[j] = glm::vec4(-1);
          //grabbing block on top of this one
          else
          {
            ASSERT(j+CELL_X_COUNT<CELL_X_COUNT*CELL_Y_COUNT);
            glm::vec4 shifted_block = cells[j+CELL_X_COUNT];
            cells[j] = shifted_block;  
          }

        }
      }
      row_is_solid = true;
    }
  }
}

void do_game_over()
{
  switch_game_state(GameState::menu);
}

void place_block_and_reset(block *blk,
    i32 x,
    i32 y)
{
  //checking for game over
  // THIS IF DOES NOT WORK!!!!
  if(y+blk->pos.h > CELL_Y_COUNT)
  {
    //game over
    //TODO: draw a text box on the screen saying (game over) or smth
    //go back to main menu

    do_game_over();

    return;
  }

  place_block(blk, x, y);
  clear_solid_lines();
  reset_current_block();
  was_hold_used = false;
}

//returns the y position that
//the current block would have if
//hard dropped on the current x pos
int32 get_fall_y_pos()
{
  //keep checking if block fits
  //and place it one y less than when it
  //doesn't
  i32 test_y = cur_block_y;

  while(does_block_fit(&cur_block,
        cur_block_x,
        test_y))
  {
    --test_y;
  }

  return ++test_y;
}

void do_hard_drop()
{
  i32 fall_y = get_fall_y_pos();
  place_block_and_reset(&cur_block, cur_block_x, 
      fall_y);
}

void fall_block()
{
  if(does_block_fit(&cur_block,
        cur_block_x,
        cur_block_y - 1))
  {
    --cur_block_y;
  }
  else
  {
    check_for_commitment();
  }

}

//runs every fall tick
void fall_block(uint32 timer_id, void* param)
{
  fall_block();
}

void force_hard_drop(uint32 timer_id, void* param)
{
  do_hard_drop();
}

//what soft drop does is simply change the tick rate
//of the drop to a faster one
void soft_drop_enable()
{
  fall_block();
  //TODO: reset soft drop timer and change interval
  // to fast drop
  tim_han.reset(fall_timer);
  tim_han.change_tick(fall_timer, soft_drop_step);
}

//restores drop tick rate
void soft_drop_disable()
{
  //TODO: reset soft drop timer and change interval
  // to slow drop
  tim_han.reset(fall_timer);
  tim_han.change_tick(fall_timer, fall_step);
}

void clear_board()
{
  for(uint32 i=0;i<CELL_X_COUNT*CELL_Y_COUNT;++i)
  {
    cells[i] = glm::vec4(-1);
  }
}

void move_cur_block(int32 dir)
{
  if(does_block_fit(&cur_block,
        cur_block_x + dir,
        cur_block_y))
  {
    cur_block_x = cur_block_x + dir;
    tim_han.reset(commitment_timer);
    check_for_commitment();
  }
}

//callback that handles move repeat
void enable_move_repeat(uint32 tim, void* param)
{
  int32 dir = (int32)(*((int32*)param));
  move_cur_block(dir);
  tim_han.change_tick(tim, KEY_REPEAT_RATE);
}

block block_init(uint32 *vals, u32 width, u32 height, glm::vec4 color)
{
  block blk;
  mat blk_mat;
  blk_mat.vals = vals;
  blk_mat.w = width;
  blk_mat.h = height;

  blk.pos = blk_mat;
  blk.color = color;
  
  return blk;
}

void gameplay_init()
{

  // initializing blocks
  blk_t = block_init(blk_t_pos, 3, 3,
                     glm::vec4(1.f, 0.019f, 0.925f, 1.f));

  blk_i = block_init(blk_i_pos, 4, 4,
    glm::vec4(0.058f, 0.960f, 1.f, 1.f));
  blk_j = block_init(blk_j_pos, 3, 3,
    glm::vec4(0.f, 0.129f, 0.878f, 1.f));
  blk_l = block_init(blk_l_pos, 3, 3,
    glm::vec4(1.f, 0.721f, 0.2f, 1.f));
  blk_o = block_init(blk_o_pos, 2, 2,
    glm::vec4(1.f, 0.964f, 0.419f, 1.f));
  blk_s = block_init(blk_s_pos, 3, 3,
    glm::vec4(0.113f, 0.839f, 0.f, 1.f));
  blk_z = block_init(blk_z_pos, 3, 3,
    glm::vec4(0.921f, 0.f, 0.058f, 1.f));

  spr_brick = sprite_new("brick-2.png",
      GL_REPEAT, GL_NEAREST);
  //this scale will fill 1 pixel
  r32 one_pix_scale = (1.f / (r32)spr_brick->tex->width);
  spr_brick->shader = grayscale_tint_shader;
  spr_brick->scale = glm::vec2(one_pix_scale * (r32)CELL_SIZE);

  spr_bg = sprite_new(spr_brick->tex);
  spr_bg->shader = grayscale_tint_shader;
  spr_bg->pos = glm::vec2(0,0);
  spr_bg->scale = glm::vec2(one_pix_scale * (r32)SCREEN_WIDTH);
  spr_bg->tint_color = glm::vec4(0.2f,0.2f,0.2f,1.f);
  spr_bg->uv_mult = SCREEN_WIDTH / (real32)CELL_SIZE;

  spr_board_bg = sprite_new("white.jpg");
  spr_board_bg->pos = glm::vec2(BOARD_X_OFFSET, BOARD_Y_OFFSET);
  spr_board_bg->scale = glm::vec2(one_pix_scale * CELL_SIZE * CELL_X_COUNT,
                      one_pix_scale * CELL_SIZE * CELL_Y_COUNT);
  spr_board_bg->tint_color = BOARD_BG_COLOR;

  //allocating space for cur block and temp block in one
  //malloc call
  cur_block.pos.vals = 
    (uint32*)malloc(sizeof(uint32)*MAX_BLOCK_CELL_COUNT*2);
  
  //placing pointer same as cur_block but shifted a bit
  block_temp.pos.vals = 
    cur_block.pos.vals + MAX_BLOCK_CELL_COUNT;
  // block_temp.pos.vals = 
  //   (uint32*)malloc(sizeof(uint32)*MAX_BLOCK_CELL_COUNT);

  //initializing next blocks
  for(uint32 i=0;i<NEXT_BLOCK_COUNT;++i)
  {
    next_blocks[i] = rand_pick((uint32)ARR_COUNT(blocks));
  }

  //initializing timers
  fall_timer = tim_han.add(fall_step, fall_block);
  turn_timer = tim_han.add(MAX_TURN_TIME, force_hard_drop);
  commitment_timer = tim_han.add(COMMITMENT_TIME, force_hard_drop);
  key_repeat_timer = tim_han.add(KEY_REPEAT_DELAY, enable_move_repeat);

  tim_han.pause(turn_timer);
  tim_han.pause(commitment_timer);
  tim_han.pause(key_repeat_timer);

  clear_board();
  reset_current_block();
}

//draws a checkered background over the board
void draw_checkerboard_bg()
{
  for(uint32 i=0;i<CELL_X_COUNT;++i)
  {
    for(uint32 j=0;j<CELL_Y_COUNT;++j)
    {
      glm::vec2 cell_pos;
      cell_pos.x = BOARD_X_OFFSET + i*CELL_SIZE;
      cell_pos.y = BOARD_Y_OFFSET + j*CELL_SIZE;

      glm::vec4 cell_color;
      if((i%2 + j%2) % 2 == 0 )
      {

        cell_color = c_white;
      }
      else
      {
        cell_color = c_grey;
      }

      // TODO: do the draw

      // draw_quad(cell_pos, glm::vec2(CELL_SIZE),
      //     0.f, cell_color, gl.tex_white);
    }
  }
}

void draw_cell(uint32 ix, uint32 iy, glm::vec4 color)
{
  glm::vec2 cell_pos;
  cell_pos.x = BOARD_X_OFFSET + ix*CELL_SIZE;
  cell_pos.y = BOARD_Y_OFFSET + iy*CELL_SIZE;

  spr_brick->pos = cell_pos;
  spr_brick->tint_color = color;
  sprite_draw(spr_brick);
  // draw_quad(cell_pos, glm::vec2(CELL_SIZE), 0.f,
  //     color, tex_brick, grayscale_tint_shader);
}

// draws the blocks that are already
// in the board
void draw_board()
{
  for(uint32 i=0;i<CELL_X_COUNT*CELL_Y_COUNT;++i)
  {

    int32 x = i % CELL_X_COUNT;
    int32 y = i / CELL_X_COUNT;

    if(!is_cell_empty(x,y))
      draw_cell(x,y,cells[i]);

    // 6 7 8 - y = 2
    // 3 4 5 - y = 1
    // 0 1 2 - y = 0
  }
}

//it draws block at ix iy, where:
//ix = left border of block
//iy = BOTTOM border of block
//so, if u want to place an o block at (0,0)
//, which would be the bottom left edge of the
//board, you would call draw_block(blk_o, 0, 0)
void draw_block(block *blk, uint32 ix, uint32 iy,
    real32 alpha = 1.f)
{
  for(uint32 i=0;i<blk->pos.w;++i)
  {
    for(uint32 j=0;j<blk->pos.h;++j)
    {
      if(blk->pos.vals[(j*(blk->pos.w))+i])
      {
        glm::vec4 final_color = blk->color;
        final_color.w = alpha;
        draw_cell(ix + i, iy + j, final_color);
      }
    }
  }
}

void draw_preview_blocks()
{

  int32 cur_y_pos=CELL_Y_COUNT;

  for(uint32 i=0;i<NEXT_BLOCK_COUNT;++i)
  {
    uint32 picked_blk = next_block_index + i;
    picked_blk%=NEXT_BLOCK_COUNT;

    block *blk = blocks[next_blocks[picked_blk]];
    cur_y_pos-=blk->pos.h;
    draw_block(blk,
        CELL_X_COUNT+1,
        cur_y_pos);
    --cur_y_pos;
  }

}

void toggle_hold()
{
  if(was_hold_used)
    return;
  reset_current_block(true);
  was_hold_used = true;
}

void draw_hold_block()
{

  if(hold_block_id != -1)
  {

    draw_block(blocks[hold_block_id], CELL_X_COUNT + 7,
        CELL_Y_COUNT - blocks[hold_block_id]->pos.h);
  }

}

//shifts block until it fits or fails
//returns (-1,-1) if it failed
//TODO:- spiral rotation is buggy. 
//     It segfaulted once!! revise it
glm::vec2 block_shift(block *blk, uint32 x, uint32 y)
{
  //spiral testing
  //
  //it will test for free positions on a spiral
  //
  // shift_amount: 1

  // try to shift -> by shift_amount
  // try to shift <- by shift_amount
  // try to shift ^ by shift_amount
  // try to shift ->^ by shift_amount
  // try to shift <-^ by shift_amount

  // ++shift_amount
  // goto start
  
  uint32 max_shift_amount = blk->pos.w;
  uint32 shift = 1;
  
  while(shift <= max_shift_amount)
  {
    if(does_block_fit(blk, x+shift,y))
      return glm::vec2(x+shift,y);
    if(does_block_fit(blk, x-shift,y))
      return glm::vec2(x-shift,y);
    if(does_block_fit(blk, x,y+shift))
      return glm::vec2(x,y+shift);

    if(does_block_fit(blk, x+shift,y+shift))
      return glm::vec2(x+shift,y+shift);
    if(does_block_fit(blk, x-shift,y+shift))
      return glm::vec2(x-shift,y+shift);

    ++shift;
  }
  
  return glm::vec2(-1,-1);
}

// rotates block
// rot_types:
//   1 = 90 deg clockwise
//   2 = 90 deg counter-clockwise
//   3 = 180 degs
void block_rotate(block *blk, uint32 rot_type)
{
  ASSERT(rot_type <= 3);
  //rotation
  
  //TODO: check if block fits after rotating

  //using temporary block for calulations
  copy_block(&block_temp, blk);

  switch(rot_type)
  {
    case 1:
      mat_rotate_90(&block_temp.pos, true);
      break;
    case 2:
      mat_rotate_90(&block_temp.pos, false);
      break;
    case 3:
      mat_rotate_180(&block_temp.pos);
      break;
  }

  bool rotate_failure = false;

  //if it doesn't fit in place...
  if(!does_block_fit(&block_temp, cur_block_x, cur_block_y))
  {
    glm::vec2 new_pos;
    new_pos = 
      block_shift(&block_temp, cur_block_x, cur_block_y);

    if(new_pos.x != -1)
    {
      cur_block_x = new_pos.x;
      cur_block_y = new_pos.y;
    }
    else
      rotate_failure = true;
  }

  //perform rotation only if it can rotate
  if(!rotate_failure)
  {
    copy_block(blk, &block_temp);
  }

}

void reset_game()
{

  real32 fall_step = INITIAL_FALL_STEP; // in seconds
  real32 soft_drop_step = INITIAL_SOFT_DROP_STEP;

  //reset all the timers
  tim_han.change_tick(fall_timer, fall_step);
  tim_han.change_tick(turn_timer, MAX_TURN_TIME);
  tim_han.change_tick(commitment_timer, COMMITMENT_TIME);

  tim_han.reset_all();

  clear_board();
  reset_current_block();

  //initializing next blocks
  for(uint32 i=0;i<NEXT_BLOCK_COUNT;++i)
  {
    next_blocks[i] = rand_pick((uint32)ARR_COUNT(blocks));
  }
  next_block_index = 0;

  hold_block_id = -1;
  was_hold_used = false;
}

void player_move_or_rotate(bool is_rotate, i32 dir)
{

  //player move is a "move block to the side" move
  if(!is_rotate)
  {
      move_cur_block(cur_block_dir);

      //key repeat
      tim_han.change_tick(key_repeat_timer, KEY_REPEAT_DELAY);
      tim_han.set_param
        (key_repeat_timer, (void*)&cur_block_dir);
      tim_han.reset(key_repeat_timer);
      tim_han.resume(key_repeat_timer);
  }
  //player move is a rotate move
  else
  {
      block_rotate(&cur_block,dir);
  }

  tim_han.reset(commitment_timer);
  check_for_commitment();
}

//frame_time is in seconds
void gameplay_loop(real32 frame_time)
{
  //timer tick
  tim_han.tick();

  //drawing background
  sprite_draw(spr_bg);

  //draw_checkerboard_bg();
  sprite_draw(spr_board_bg);

    if(key_get_down('a') || key_get_down('d'))
    {
      cur_block_dir = key_get_down('a') ? -1 : 1;
      player_move_or_rotate(false, cur_block_dir);
    }

    if(key_get_up('d') || key_get_up('a'))
    {
      tim_han.pause(key_repeat_timer);
    }

    //only for debug purposes
#ifdef DEBUG
    if(key_get_down('r'))
    {
      reset_game();
    }
#endif

    if(key_get_down(SDLK_SPACE))
      do_hard_drop();
    if(key_get_down('s'))
      soft_drop_enable();
    else if(key_get_up('s'))
      soft_drop_disable();

    //rotation
    if(key_get_down('h'))
    {
      player_move_or_rotate(true, 1);
    }
    if(key_get_down('j'))
    {
      player_move_or_rotate(true, 2);
    }
    if(key_get_down('k'))
    {
      player_move_or_rotate(true, 3);
    }

    if(key_get_down('u'))
      toggle_hold();

    if(key_get_down('r'))
      reset_game();

    //draw block shadow
    draw_block(&cur_block,
        cur_block_x,
        get_fall_y_pos(),
        0.5f);

    //draw current block
    draw_block(&cur_block,
        cur_block_x,
        cur_block_y);

    draw_board();

    draw_preview_blocks();

    draw_hold_block();

}
