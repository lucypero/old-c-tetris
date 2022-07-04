#include <ft2build.h>
#include FT_FREETYPE_H
#include <math.h>

#include "opengl_utils.cpp"

//some colors just to test
glm::vec4 c_red = glm::vec4(1.f, 0.f, 0.f, 1.f);
glm::vec4 c_white = glm::vec4(1.f,1.f,1.f,1.f);
glm::vec4 c_grey = glm::vec4(0.6f,0.6f,0.6f,1.f);

//shader structs:
struct SpriteShader
{
  GLuint id; //id of the gl program  
  //shader uniforms
  GLint un_projection,
        un_texture,
        un_model,
        un_sprite_color,
        un_uv_multiplier;
};

//TODO: TEMPORARY:
//     i'll use sprite_shader struct for
//     the grayscale tint shader for now
SpriteShader grayscale_tint_shader;

struct
{
  GLuint id;
  GLint un_color;
}
solid_color_shader;

struct text_shader
{
  GLuint id;
  //shader uniforms
  GLint un_tex;
  GLint un_tint_color;
  GLint att_coord;
  GLint un_mvp;
};

GLuint quad_vao;
SpriteShader sprite_shader;

//The pivot position of the sprite for positioning and rotation.
enum class PivotPosition
{
  center,
  bottom_left
};

//remember that if this is used to represent boxes on the screen,
//the x and y point will be on the bottom left part of the thing,
// because of the unique way that Peropero's engine works.
struct Rect
{
  i32 x, y, w, h;
};

struct char_info
{
  real32 ax; // advance.x
  real32 ay; // advance.y
  real32 bw; // bitmap.width;
  real32 bh; // bitmap.rows;
  real32 bl; // bitmap_left;
  real32 bt; // bitmap_top;
  real32 tx; // x offset of glyph in texture coordinates
};

struct Font
{
  GLuint tx_id;
  char_info *char_info;
  uint32 char_count;
  uint32 width;
  uint32 height;
};

struct FontSprite
{
  Font* font;

  glm::vec2 pos;
  PivotPosition pivot_pos;
  glm::vec4 color;
  r32 rot;
  r32 scale;
};

//Metadata of a texture loaded in GPU memory.
struct Texture
{
  GLint id;
  const char *res_path;
  i32 width;
  i32 height;
  GLint wrap_mode;
  GLint filtering_mode;
};

//helper function to generate a gl texture and return its id

//wrap modes: GL_CLAMP_TO_EDGE - GL_REPEAT
//filter modes: GL_NEAREST - GL_LINEAR
GLuint gen_texture(GLint wrap_mode, GLint filter_mode)
{
  GLuint tid;
  glGenTextures(1, &tid);
  glBindTexture(GL_TEXTURE_2D, tid);
 // Set Texture wrap and filter modes
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
      wrap_mode);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
      wrap_mode);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_mode);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_mode);
  return tid;
}

//Loads a texture (performs a malloc)
Texture* texture_new(const char* texture_file, 
    GLint wrap_mode = GL_CLAMP_TO_EDGE, GLint filter_mode = GL_NEAREST)
{
  //NOTE: loading a texture from file
  int32 width, height, channels;
  stbi_set_flip_vertically_on_load(true);
  uint8 *image = stbi_load(texture_file,
      &width,
      &height,
      &channels,
      STBI_rgb_alpha);

  GLuint tid = gen_texture(wrap_mode, filter_mode);
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
      width, height, 0,
      GL_RGBA, GL_UNSIGNED_BYTE,
      image);
 
  stbi_image_free(image);

  Texture *tex = (Texture*)malloc(sizeof(Texture));
  tex->id = tid;
  tex->res_path = texture_file;
  tex->width = width;
  tex->height = height;
  tex->wrap_mode = wrap_mode;
  tex->filtering_mode = filter_mode;

  return tex;
}


//Sprite API
//the purpose of the Sprite struct is to give the gameplay code
// a convenient and easy way to render textures to the screen.
struct Sprite
{
  Texture *tex;
  glm::vec2 pos;
  glm::vec2 scale;
  real32 rot;
  PivotPosition pivot_pos;
  glm::vec4 tint_color;
  real32 uv_mult;
  SpriteShader shader;
};

//the API consumer will have two ways to initialize a sprite.
// it will be able to supply a texture path and the initializer will
// take care of the texture initialization.
Sprite* sprite_new(const char* tex_path,
    GLint wrap_mode = GL_CLAMP_TO_EDGE, GLint filter_mode = GL_NEAREST)
{
  Sprite *spr = (Sprite*)malloc(sizeof(Sprite));
  spr->tex = texture_new(tex_path, wrap_mode, filter_mode);
  spr->pos = glm::vec2(0,0);
  spr->scale = glm::vec2(1);
  spr->rot = 0.f;
  spr->pivot_pos = PivotPosition::bottom_left;
  spr->tint_color = c_white;
  spr->uv_mult = 1.f;
  spr->shader = sprite_shader;
  return spr;
}

// or, in the case that there is already a texture at hand, the user
//  code has the option to supply the already initialized Texture to
// the sprite.
Sprite* sprite_new(Texture *tex)
{
  Sprite *spr = (Sprite*)malloc(sizeof(Sprite));
  spr->tex = tex;
  spr->pos = glm::vec2(0,0);
  spr->scale = glm::vec2(1);
  spr->rot = 0.f;
  spr->pivot_pos = PivotPosition::bottom_left;
  return spr;
}

//draws the sprite 
void sprite_draw(Sprite *spr)
{
  glUseProgram(spr->shader.id);
  glBindTexture(GL_TEXTURE_2D, spr->tex->id);

  glm::mat4 model(1.0f);

  model = glm::translate(model, glm::vec3(spr->pos, 0.0f));  
  model = glm::rotate(model, spr->rot, glm::vec3(0.0f, 0.0f, 1.0f)); 
  model = glm::scale(model, glm::vec3(spr->scale.x * spr->tex->width, spr->scale.y * spr->tex->height, 1.0f)); 

  if(spr->pivot_pos == PivotPosition::center)
  {
    model = glm::translate(model,
          glm::vec3(-0.5f, -0.5f, 0.0f));  
  }

  glUniformMatrix4fv(sprite_shader.un_model, 1, GL_FALSE,
      glm::value_ptr(model));

  glUniform4f(sprite_shader.un_sprite_color,
      spr->tint_color.x,
      spr->tint_color.y,
      spr->tint_color.z,
      spr->tint_color.w
  );

  glUniform1f(sprite_shader.un_uv_multiplier, spr->uv_mult);

  glBindVertexArray(quad_vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);

  GL_ERROR_CHECK();
}


glm::mat4 projection;
//text related data:

//TODO: put this all into a struct
#define TEXT_ATLAS_COUNT 2
FT_Library  ft;
GLuint text_vbo;
GLuint text_vao;
     
real32 sx, sy;
text_shader text_shader;

Font *txat_ubuntu_20;
Font *txat_ubuntu_40;
//text related data end

//Coordinate space names:

// View Space:
//   - The final space that OpenGL uses to draw vertices
//   x:[-1,1]
//   y:[-1,1]
// Screen Space:
//   - The space of the window in pixels
//   x:[0,SCREEN_WIDTH] from left to right
//   y:[0,SCREEN_HEIGHT] from the bottom to the top
  
//Space translation functions:
glm::vec2 screen_to_view(glm::vec2 vec)
{
  //converting view space to screen space
  // [0,SCREEN_WIDTH] -> [-1,1]
  // [0,SCREEN_HEIGHT] -> [-1,1]

  vec.x /= SCREEN_WIDTH;
  vec.x *= 2;
  vec.x -= 1;

  vec.y /= SCREEN_HEIGHT;
  vec.y *= 2;
  vec.y -= 1;

  return vec;
}

glm::vec2 view_to_screen(glm::vec2 vec)
{
  vec.x += 1.f;
  vec.x /= 2.f;
  vec.x *= SCREEN_WIDTH;

  vec.y += 1.f;
  vec.y /= 2.f;
  vec.y *= SCREEN_HEIGHT;

  return vec;
}

char* shader_get_vert_source(const char* string)
{
  return get_substr(string, "#shader vert", "#shader frag");
}

char* shader_get_frag_source(const char* string)
{
  return get_substr(string, "#shader frag", 0);
}

GLuint load_gl_program(const char* shader_source_file)
{
  GLuint vert_shader, frag_shader, gl_program;
  vert_shader = glCreateShader(GL_VERTEX_SHADER);

  char* shader_source = read_entire_file(shader_source_file);

  char* vert_shader_source = shader_get_vert_source(shader_source);

  glShaderSource(vert_shader, 1, &vert_shader_source, 0);
  glCompileShader(vert_shader);

  //NOTE: shader compile error checking
  GLint success;
  GLchar info_log[1024];
  glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
  if(!success)
  {
    glGetShaderInfoLog(vert_shader, 1024, 0, info_log);
    fprintf(stderr,"vert shader compile error:\n%s\nexiting...\n",info_log);
    exit(1);
  }

  free(vert_shader_source);

  char* frag_shader_source = shader_get_frag_source(shader_source);
  frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(frag_shader, 1, &frag_shader_source, 0);
  glCompileShader(frag_shader);

  //NOTE: shader compile error checking
  glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
  if(!success)
  {
    glGetShaderInfoLog(frag_shader, 1024, 0, info_log);
    fprintf(stderr,"frag shader compile error:\n%s\nexiting...\n",info_log);
    exit(1);
  }

  free(frag_shader_source);
  free(shader_source);

  gl_program = glCreateProgram();
  glAttachShader(gl_program, vert_shader);
  glAttachShader(gl_program, frag_shader);
  glLinkProgram(gl_program);

  glGetProgramiv(gl_program, GL_LINK_STATUS, &success);
  if(!success)
  {
    glGetProgramInfoLog(gl_program, 1024, 0, info_log);
    fprintf(stderr,"opengl program link error:\n%s\nexiting...\n",info_log);
    exit(1);
  }

  glDeleteShader(vert_shader);
  glDeleteShader(frag_shader);

  GL_ERROR_CHECK();

  return gl_program;
}


// generates a new text atlas for a specified font and font size
// only ascii for now
Font* font_new(const char *font_file, uint32 font_size)
{
  FT_Face face;

  if(FT_New_Face(ft, font_file, 0, &face))
    panic("could not open font %s\n", font_file);

  FT_Set_Pixel_Sizes(face, 0, font_size);
  //this could use texture_new instead of gen_texture, then i could
  // get rid of gen_texture
  GLuint atlas_texture = gen_texture(GL_CLAMP_TO_EDGE, GL_LINEAR);
  FT_GlyphSlot g = face->glyph;

  //finding out atlas dimensions
  int w = 0;
  int h = 0;

  for(int i = 32; i < 128; i++) 
  {
    if(FT_Load_Char(face, i, FT_LOAD_RENDER)) 
    {
      fprintf(stderr, "Loading character %c failed!\n", i);
      continue;
    }

    w += g->bitmap.width;
    h = max(h, g->bitmap.rows);
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
 #define CHAR_COUNT 128
  char_info *c = (char_info*)malloc(sizeof(char_info) * CHAR_COUNT);
  uint32 x = 0;

  for(int i = 32; i < CHAR_COUNT; i++) 
  {
    if(FT_Load_Char(face, i, FT_LOAD_RENDER))
      continue;
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0,
        g->bitmap.width, g->bitmap.rows, GL_RED,
        GL_UNSIGNED_BYTE, g->bitmap.buffer);

    c[i].ax = g->advance.x >> 6;
    c[i].ay = g->advance.y >> 6;

    c[i].bw = g->bitmap.width;
    c[i].bh = g->bitmap.rows;

    c[i].bl = g->bitmap_left;
    c[i].bt = g->bitmap_top;

    c[i].tx = (float)x / w;
    x += g->bitmap.width;
  }

  Font *font = (Font*)malloc(sizeof(Font));

  font->tx_id = atlas_texture;
  font->width = w;
  font->height = h;
  font->char_count = CHAR_COUNT;
  font->char_info = c;

  return font;
}

FontSprite* font_sprite_new(Font* font)
{
  FontSprite *font_spr = (FontSprite*)malloc(sizeof(FontSprite));
  font_spr->font = font;
  font_spr->pos = glm::vec2(0,0);
  font_spr->pivot_pos = PivotPosition::center;
  font_spr->color = glm::vec4(1);
  font_spr->rot = 0.f;
  font_spr->scale = 1.f;
  return font_spr;
}

//TODO: rotation is broken
// return: the rect that takes up on the screen, in pixels
Rect font_sprite_draw(FontSprite* font_spr, const char* text)
{
  struct point 
  {
    GLfloat x;
    GLfloat y;
    GLfloat s;
    GLfloat t;
  };

  //TODO: use 4 verts instead of 6 and render with GL_TRIANGLE_STRIP
  point *coords = (point*)malloc(sizeof(point) * 6 * strlen(text));

  Font* font = font_spr->font;

  glm::vec2 snapped_pos = glm::vec2(
      _floor(font_spr->pos.x),
      _floor(font_spr->pos.y)
      );

  glm::vec2 view_pos = screen_to_view(snapped_pos);

  //TODO: _floor position for pixel snapping

  //setting context
  glUseProgram(text_shader.id);
  glBindTexture(GL_TEXTURE_2D, font->tx_id);

  glUniform4f(text_shader.un_tint_color, font_spr->color.x,font_spr->color.y,font_spr->color.z,font_spr->color.w);

  uint32 n = 0;

  //store the x and y positions
  //this is the top left corner position of the text

  real32 x_pos = view_pos.x;
  real32 y_pos = view_pos.y;

  //to figure out the center of the text, all
  //we need to do is add w/2 and h/2 to x and y
  real32 total_width = 0.f;
  real32 total_height = 0.f;

  for(const char *p = text; *p; p++) 
  { 
    uint32 c_i = (uint32)(*p);
    real32 x2 =  view_pos.x + font->char_info[c_i].bl * sx;
    real32 y2 = -view_pos.y - font->char_info[c_i].bt * sy;
    real32 w = font->char_info[c_i].bw * sx;
    real32 h = font->char_info[c_i].bh * sy;

    /* Advance the cursor to the start of the next character */
    view_pos.x += font->char_info[c_i].ax * sx;
    view_pos.y += font->char_info[c_i].ay * sy;

    total_width += font->char_info[c_i].ax * sx;
    //this only works because this always renders the text in one line
    total_height = max(total_height,h);

    /* Skip glyphs that have no pixels */
    if(!w || !h)
      continue;

    point a;
    a.x = x2;
    a.y = -y2;
    a.s = font->char_info[c_i].tx;
    a.t = 0;

    coords[n++] = a;

    a.x = x2 + w;
    a.y = -y2;
    a.s = font->char_info[c_i].tx + font->char_info[c_i].bw / font->width;
    a.t = 0;
    coords[n++] = a;

    a.x = x2;
    a.y = -y2-h;
    a.s = font->char_info[c_i].tx;
    a.t = font->char_info[c_i].bh / font->height;
    coords[n++] = a;

    //remember: each glyph occupies a different amount of vertical space
    a.x = x2 + w;
    a.y = -y2;
    a.s = font->char_info[c_i].tx + font->char_info[c_i].bw / font->width;
    a.t = 0;
    coords[n++] = a;

    a.x = x2;
    a.y = -y2 - h;
    a.s = font->char_info[c_i].tx;
    a.t = font->char_info[c_i].bh / font->height;
    coords[n++] = a;

    a.x = x2 + w;
    a.y = -y2 - h;
    a.s = font->char_info[c_i].tx + font->char_info[c_i].bw / 
      font->width;
    a.t = font->char_info[c_i].bh / font->height;
    coords[n++] = a;
  }

  // here we obtain the center position of the text

  x_pos += total_width / 2.f;
  y_pos += total_height / 2.f;

  glm::mat4 model(1.0f);
  //move text back to center, transform, then place it back.
  model = 
    glm::translate(model, glm::vec3(x_pos, y_pos, 0.f)); 

  if(font_spr->pivot_pos == PivotPosition::center)
  {
    glm::vec2 translation = 
      glm::vec2(total_width / 2.f, total_height / 2.f);
    translation = view_to_screen(translation);
    //snapping to pixel
    translation.x = _floor(translation.x);
    translation.y = _floor(translation.y);

    translation = screen_to_view(translation);

    model = glm::translate(model, 
        glm::vec3(
          -(translation.x), -(translation.y), 0.f));
  }
  model = 
    glm::rotate(model, font_spr->rot, glm::vec3(0.0f, 0.0f, 1.0f)); 
  model = 
    glm::scale(model, glm::vec3(font_spr->scale, font_spr->scale, 1.0f)); 
  model = 
    glm::translate(model, glm::vec3(-x_pos, -y_pos, 0.f)); 

  //multiplying model with projection
  // glm::mat4 mvp = projection * model;

  glUniformMatrix4fv(text_shader.un_mvp, 1, GL_FALSE,
      glm::value_ptr(model));

  glBindVertexArray(text_vao);
  glBufferData(GL_ARRAY_BUFFER, sizeof(point) * 6 * strlen(text), coords, GL_DYNAMIC_DRAW);


  //TODO: use 4 verts instead of 6 and render with GL_TRIANGLE_STRIP
  glDrawArrays(GL_TRIANGLES, 0, n);
  glBindVertexArray(0);

  free(coords);

  // height and width in pixels
  i32 width_in_px = i32(total_width * (SCREEN_WIDTH / 2.f));
  i32 height_in_px = i32(total_height * (SCREEN_HEIGHT / 2.f));

  // 1 screen_width/2
  // 

  Rect rect;

  if(font_spr->pivot_pos == PivotPosition::center)
  {
    rect.x = snapped_pos.x - (width_in_px / 2);
    rect.y = snapped_pos.y - (height_in_px / 2); 
  }
  else
  {
    rect.x = snapped_pos.x;
    rect.y = snapped_pos.y;
  }

  rect.w = width_in_px;
  rect.h = height_in_px;

  return rect;
}


bool is_shader_used(GLuint program)
{
  GLint cur_shader;
  glGetIntegerv(GL_CURRENT_PROGRAM, &cur_shader);
  return cur_shader == program;
}

bool is_array_buffer_bound(GLuint buffer)
{
  GLint cur_buffer;
  glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &cur_buffer);
  return cur_buffer == buffer;
}

void init_text()
{
  //TODO: this should only apply to text quads
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  if(FT_Init_FreeType(&ft)) 
    panic("Could not init Free Type\n");
 
  //NOTE:Loading text shader
  text_shader.id = load_gl_program("text.shader");
  glUseProgram(text_shader.id);
  text_shader.att_coord = glGetAttribLocation(text_shader.id, "coord");
  if(text_shader.att_coord == -1)
    panic("wooow wtf\n");

  //Getting uniform locations
  text_shader.un_tex = 
    glGetUniformLocation(text_shader.id, "tex");
  text_shader.un_tint_color = 
    glGetUniformLocation(text_shader.id, "tint_color");
  text_shader.un_mvp =
    glGetUniformLocation(text_shader.id, "mvp");
  
  //Initializing uniforms
  glUniform1i(text_shader.un_tex, 0);
  glUniform4f(text_shader.un_tint_color, 1.f, 1.f, 1.f, 1.f);

  glGenBuffers(1, &text_vbo);

/* Set up the VBO for our vertex data */
  //TODO: i don't really know what this line does

  //NOTE: Creating text vao

  glGenVertexArrays(1, &text_vao);
  glBindVertexArray(text_vao);

  glBindBuffer(GL_ARRAY_BUFFER, text_vbo);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(text_shader.att_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);

    
  glBindVertexArray(0);
  sx = 2.0f / SCREEN_WIDTH;
  sy = 2.0f / SCREEN_HEIGHT;

  // initializing all the fonts that will be used in the game

  txat_ubuntu_20 = font_new("ubuntu.ttf", 20);
  txat_ubuntu_40 = font_new("ubuntu.ttf", 40);
}

void opengl_init()
{
  // OpenGL configuration

  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  // I could not make opengl render the front face of the quad
  //   without changing the winding order idk
  //  glFrontFace(GL_CW);
  //glEnable(GL_CULL_FACE); 
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


  //NOTE: Setting up textures
  glActiveTexture(GL_TEXTURE0);

  // NOTE: Configure VAO/VBO
  GLfloat vertices[] = { 
    // Pos      // Tex
    0.0f, 0.0f, 0.0f, 0.0f, // 2
    1.0f, 0.0f, 1.0f, 0.0f,// 3
    0.0f, 1.0f, 0.0f, 1.0f, // 0
    1.0f, 1.0f, 1.0f, 1.0f // 1
  };
  // 0     1
  //
  //
  // 2     3

  GLushort indices[] = {
    2,3,0,1
  };

  glGenVertexArrays(1, &quad_vao);

  glBindVertexArray(quad_vao);

  // vertex buffer
  GLuint vbo;
  glGenBuffers(1, &vbo);  
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // element array buffer
  GLuint vao;
  glGenBuffers(1, &vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vao);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);

  glBindVertexArray(0);
  GL_ERROR_CHECK();

  //NOTE: initializing shaders:
  //NOTE:Loading sprite shader
  sprite_shader.id = load_gl_program("textured.shader");
  glUseProgram(sprite_shader.id);

  //getting uniform locations
  //NOTE: setting up projection
  //ortho(left, right, bottom, top, near zplane, far zplane);
  projection = 
    glm::ortho(0.0f,
        (GLfloat)SCREEN_WIDTH,
        0.f,
        (GLfloat)SCREEN_HEIGHT, -1.0f, 1.0f);

  //Getting uniform locations
  sprite_shader.un_projection = 
    glGetUniformLocation(sprite_shader.id, "projection");
  sprite_shader.un_texture = 
    glGetUniformLocation(sprite_shader.id, "image");
  sprite_shader.un_model = 
    glGetUniformLocation(sprite_shader.id, "model");
  sprite_shader.un_sprite_color = 
    glGetUniformLocation(sprite_shader.id, "sprite_color");
  sprite_shader.un_uv_multiplier = 
    glGetUniformLocation(sprite_shader.id, "uv_multiplier");


  //Initializing uniforms
  glUniformMatrix4fv(sprite_shader.un_projection,
      1, GL_FALSE,
      glm::value_ptr(projection));

  glUniform1i(sprite_shader.un_texture, 0);
  glUniform1f(sprite_shader.un_uv_multiplier, 1.f);

  //initializing grayscale tint shader:

  grayscale_tint_shader.id = 
    load_gl_program("grayscale_tint.shader");
  glUseProgram(grayscale_tint_shader.id);

  grayscale_tint_shader.un_projection = 
    glGetUniformLocation(grayscale_tint_shader.id,
        "projection");
  grayscale_tint_shader.un_texture = 
    glGetUniformLocation(grayscale_tint_shader.id,
        "texture");
  grayscale_tint_shader.un_model = 
    glGetUniformLocation(grayscale_tint_shader.id,
        "model");
  grayscale_tint_shader.un_sprite_color = 
    glGetUniformLocation(grayscale_tint_shader.id,
        "sprite_color");
  grayscale_tint_shader.un_uv_multiplier = 
    glGetUniformLocation(grayscale_tint_shader.id, "uv_multiplier");


  //Initializing uniforms
  glUniformMatrix4fv(grayscale_tint_shader.un_projection,
      1, GL_FALSE,
      glm::value_ptr(projection));

  glUniform1i(grayscale_tint_shader.un_texture, 0);

  init_text();

  GL_ERROR_CHECK();
}

void draw_quad(glm::vec2 pos, glm::vec2 size, real32 rotate,
    glm::vec4 color, GLuint tex, SpriteShader shader,
    real32 uv_multiplier = 1.f, bool from_center = false)
{
  
  glUseProgram(shader.id);
  glBindTexture(GL_TEXTURE_2D, tex);

  glm::mat4 model(1.0f);
  if(from_center)
  {

  model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));  

  }
  model = glm::translate(model, glm::vec3(pos, 0.0f));  
  model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f)); 
  model = glm::scale(model, glm::vec3(size, 1.0f)); 

  glUniformMatrix4fv(shader.un_model, 1, GL_FALSE,
      glm::value_ptr(model));
  glUniform4f(shader.un_sprite_color, color.x, color.y, color.z, color.w);
  glUniform1f(shader.un_uv_multiplier, uv_multiplier);

  glBindVertexArray(quad_vao);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);

  GL_ERROR_CHECK();
}

void draw_quad(glm::vec2 pos, glm::vec2 size, real32 rotate,
    glm::vec4 color, GLuint tex, bool from_center = false)
{
  draw_quad(pos, size, rotate, color, tex, sprite_shader, 1.f, from_center);
}
