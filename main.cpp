#include <cstdio>

//TODO: i changed this to just "SDL.h" to compile in windows.
// check if it works this way in GNU/Linux.
// #include "SDL2/SDL.h"
#include "SDL.h"
#include "glad/glad.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <string.h>
#include <time.h>

#ifdef LINUX
#include <unistd.h>
#include <pthread.h>
#endif

#include "utils/debug.h"
#include "utils/misc.cpp"
#include "utils/types.h"
#include "utils/file.cpp"
#include "utils/random.cpp"
#include "utils/string.cpp"
#include "utils/math.cpp"
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define WINDOW_TITLE "peropero's Tetris"
uint32 SCREEN_WIDTH = 800;
uint32 SCREEN_HEIGHT = 704;
bool is_game_running = 1;
//TODO: bad things will happen when enough time passes and
//      this overflows
real64 total_time = 0.f;

#define FPS_MAX 60

void quit_game()
{
  is_game_running = false;
}

#include "input.cpp"
#include "opengl.cpp"
#include "game.cpp"

// Time that the last frame was displayed on the screen
//   in seconds.
real32 frame_time_display;
char fps_string[30];

Uint32 refresh_framerate(Uint32 interval, void* param)
{
  sprintf(fps_string,"FPS: %.2f", 1.f / frame_time_display);

  return interval;
}

int main(int argc, char* argv[])
{
  srand(time(0));

  //NOTE: SDL init code
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
  {
    fprintf(stderr, "Failed to init SDL\n");
    return 1;
  }

  Uint32 WindowFlags = SDL_WINDOW_OPENGL;

  SDL_Window *Window = SDL_CreateWindow(
      WINDOW_TITLE, 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, WindowFlags);
  ASSERT(Window);

  SDL_GL_SetAttribute(
      SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE
      );

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  //NOTE: setting opengl version to 3.3

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

  SDL_SetWindowResizable(Window, SDL_FALSE);

  SDL_GLContext context = SDL_GL_CreateContext(Window);

  if(!gladLoadGL()) {
    fprintf(stderr, "Error loading GLAD\n");
    exit(-1);
  }
  printf("OpenGL version: %d.%d\n", GLVersion.major, GLVersion.minor);

  int si = SDL_GL_GetSwapInterval();
  printf("Swap Interval: %i\n", si);

  //NOTE: i commented this because sdl throws an error
  //         "that operation is not supported"
  //         (it only errors on my laptop though)
  // int err = SDL_GL_SetSwapInterval(1);
  // if(err != 0)
  // {
  //   fprintf(stderr,
  // 	    "error at sdl swap interval\nsdl error:%s\n",
  //           SDL_GetError());
  //   exit(1);
  // }

  GL_ERROR_CHECK();


  input_init();
  opengl_init();

  //init game data

  game_init();

  real32 frame_time;
  uint64 ticks_old = SDL_GetPerformanceCounter();
  uint64 ticks_new = 0;

  //min frame time in seconds, to enforce FPS_MAX
  real32 min_dt = (1000.0f / FPS_MAX) / 1000.0f;


  SDL_AddTimer(200, refresh_framerate, 0);

  while(is_game_running)
  {
    //calculate delta time
    ticks_new = SDL_GetPerformanceCounter();
    //frame time is in seconds
    frame_time = (ticks_new - ticks_old) /
                        (real32) SDL_GetPerformanceFrequency();
    ticks_old = SDL_GetPerformanceCounter();

    //enforcing FPS_MAX
    //if the frame lasted less than the enforced minimum
    if(frame_time < min_dt)
    {
      //the thread will sleep until the frame lasts the minimum
      real32 diff = min_dt - frame_time;
    //  printf("difference: %f seconds"
    //         ". the thread should sleep for that amount.\n", diff);
      //nsleep takes miliseconds so we multiply by 1000

      SDL_Delay(diff * 1000);
      // nsleep(diff * 1000);

      //we measure ticks again
      ticks_new = SDL_GetPerformanceCounter();
      frame_time += (ticks_new - ticks_old) /
        (real32) SDL_GetPerformanceFrequency();
      frame_time_display = frame_time;
      ticks_old = SDL_GetPerformanceCounter();
    }

    //printing FPS
    //printf("frame time: %f seconds - FPS: %f\n", frame_time, 1.f / frame_time);
    //print it in text
    // Clear color buffer
    glClearColor(0.f,0.f,0.f,1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    //Capture input
    if(process_input())
      is_game_running = 0;

    total_time+= frame_time;

    //frame_time is in seconds
    game_loop(frame_time);

    //TODO: do this with new text api
    //draw_text(fps_string, glm::vec2(SCREEN_WIDTH - 100,0));

    GL_ERROR_CHECK();

    SDL_GL_SwapWindow(Window);
  }

  //Destroy window
  SDL_DestroyWindow(Window);
  Window = NULL;

  //Quit SDL subsystems
  SDL_Quit();

  return 0;
}

