//NOTE: gameplay data

//how many pixels are the paddles away from the screen borders
#define PADDLE_PADDING 10.f
#define PADDLE_WIDTH 20.f
#define PADDLE_HEIGHT 100.f
#define PADDLE_COLOR glm::vec3(0.f,0.f,0.f)
#define PADDLE_VEL 700.f

#define BALL_WIDTH 20.f
#define BALL_COLOR glm::vec3(0.f,0.f,0.f)

#define BALL_VEL_X 430.f
#define BALL_VEL_Y 430.f
#define MAX_COL_X_DISTANCE 10.f
#define SCORE_MAX 5

struct entity
{
  glm::vec2 pos;
  glm::vec2 size;
  glm::vec3 color;
  GLuint tex_id;
};

entity* entities[3];
entity player_1;
entity player_2;
entity ball;
uint32 player_1_score = 0, player_2_score = 0;
glm::vec2 ball_velocity(BALL_VEL_X,BALL_VEL_Y);

glm::vec2 get_screen_middle()
{
  return glm::vec2((real32)SCREEN_WIDTH / 2.f,
      (real32)SCREEN_HEIGHT / 2.f);

}

void score_reset()
{
  player_1_score = 0;
  player_2_score = 0;
}

void reset_positions()
{
  player_1.pos = glm::vec2(
      PADDLE_PADDING,
      SCREEN_HEIGHT / 2.f - PADDLE_HEIGHT / 2.f);
  player_2.pos = glm::vec2(
      SCREEN_WIDTH - PADDLE_PADDING - PADDLE_WIDTH,
      SCREEN_HEIGHT / 2.f - PADDLE_HEIGHT / 2.f);


  ball.pos = get_screen_middle();
//  sleep(1);//todo: this doesn't work lol
}

void game_reset()
{
  score_reset();
  reset_positions();
}

void score_add(uint32 player)
{
  if(player == 1)
    ++player_1_score;
  else if (player == 2)
    ++player_2_score;
  printf("player %i scored\n",player);
  if(player_1_score == SCORE_MAX)
  {
    printf("player 1 won\n");
    game_reset();
  }
  if(player_2_score == SCORE_MAX)
  {
    printf("player 2 won\n");
    game_reset();
  }
}

void wasd_control(glm::vec2 *pos)
{
  if(key_get("w"))
    pos->y += 2.f;
  if(key_get("s"))
    pos->y -= 2.f;
  if(key_get("d"))
    pos->x += 2.f;
  if(key_get("a"))
    pos->x -= 2.f;
}

void game_init()
{
//NOTE: gameplay code setup
  player_1 = (entity){
    glm::vec2(
        PADDLE_PADDING,
        SCREEN_HEIGHT / 2.f - PADDLE_HEIGHT / 2.f),
      glm::vec2(PADDLE_WIDTH, PADDLE_HEIGHT),
      PADDLE_COLOR,
      gl.tex_white
  };

  player_2 = (entity){
    glm::vec2(
        SCREEN_WIDTH - PADDLE_PADDING - PADDLE_WIDTH,
        SCREEN_HEIGHT / 2.f - PADDLE_HEIGHT / 2.f),
      glm::vec2(PADDLE_WIDTH, PADDLE_HEIGHT),
      PADDLE_COLOR,
      gl.tex_white
  };

  ball = (entity){
    glm::vec2(
        SCREEN_WIDTH / 2.f - BALL_WIDTH / 2.f,
        SCREEN_HEIGHT / 2.f - BALL_WIDTH / 2.f),
      glm::vec2(BALL_WIDTH, BALL_WIDTH),
      BALL_COLOR,
      gl.tex_circle
  };

  entities[0] = &player_1;
  entities[1] = &player_2;
  entities[2] = &ball;

  glUseProgram(gl.sprite_shader.id);
}
glm::vec2 text_pos(
    SCREEN_WIDTH / 2.f,
    SCREEN_HEIGHT / 2.f);
void game_loop(real32 delta_time)
{
  //Update positions
  if(key_get("w"))//w
    player_1.pos.y+=PADDLE_VEL * delta_time; 
  if(key_get("up"))//up arrow
    player_2.pos.y+=PADDLE_VEL * delta_time; 
  if(key_get("s"))//s
    player_1.pos.y-=PADDLE_VEL * delta_time;
  if(key_get("down"))//down arrow
    player_2.pos.y-=PADDLE_VEL * delta_time;

  wasd_control(&text_pos);

  draw_text("hi how are uuuu",
      text_pos);

  glm::vec2 lolpos(0,0);
  //limit paddle pos to screen borders

  if(player_2.pos.y + PADDLE_HEIGHT > 
      SCREEN_HEIGHT)
    player_2.pos.y = SCREEN_HEIGHT - PADDLE_HEIGHT;
  if(player_2.pos.y < 0.f)
    player_2.pos.y = 0.f;

  //debug:: reset paddle pos
  if(key_get_down("+"))
    reset_positions();

  if(player_1.pos.y + PADDLE_HEIGHT > 
      SCREEN_HEIGHT)
    player_1.pos.y = SCREEN_HEIGHT - PADDLE_HEIGHT;
  if(player_1.pos.y < 0.f)
    player_1.pos.y = 0.f;

  //moving ball
  ball.pos += ball_velocity * delta_time;

  //Run ball collision detection
  // ball vertical border check
  if(ball.pos.y + BALL_WIDTH > SCREEN_HEIGHT)
    ball_velocity.y = -BALL_VEL_Y;
  if(ball.pos.y < 0.f)
    ball_velocity.y = BALL_VEL_Y;

  // ball to paddle check
  if(ball.pos.y + ball.size.y >= player_1.pos.y &&
      ball.pos.y <= player_1.pos.y + player_1.size.y &&
      ball.pos.x <= player_1.pos.x + player_1.size.x)
    //distance check
  {
    //distance betw ball left edge 
    //and paddle inner edge
    real32 distance = player_1.pos.x + player_1.size.x - ball.pos.x;
    if(distance <= MAX_COL_X_DISTANCE)
    {
      ball_velocity.x = BALL_VEL_X;
    }
  }

  // ball to paddle check
  if(ball.pos.y + ball.size.y >= player_2.pos.y &&
      ball.pos.y <= player_2.pos.y + player_2.size.y &&
      ball.pos.x + BALL_WIDTH >= player_2.pos.x)
    //distance check
  {
    //distance betw ball left edge 
    //and paddle inner edge
    real32 distance = - player_2.pos.x + (ball.pos.x + BALL_WIDTH);
    if(distance <= MAX_COL_X_DISTANCE)
    {
      ball_velocity.x = -BALL_VEL_X;
    }
  }
  // win condition

  //player 2 scored
  if(ball.pos.x + ball.size.x <= 0.f)
  {
    score_add(2);
    reset_positions();
  }
  //player 1 scored
  if(ball.pos.x >= SCREEN_WIDTH)
  {
    score_add(1);
    reset_positions();
  }

  //NOTE:Rendering game
  for(uint32 i = 0; i<ARR_COUNT(entities);++i)
  {
    entity ent = *entities[i];
    draw_quad(ent.pos, ent.size, 0.f, ent.color, ent.tex_id);
  }
}


