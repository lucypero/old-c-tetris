/*

   Rendering API:

   we have Sprites.

   when initing a sprite:

   sprite s = sprite_new(texture_path, position);

   this fills the `sprite` struct:

   struct sprite
   {
     texture *tex;
     glm::vec2 pos;
     glm::vec2 scale;
     real32 rot;
     coord_origin origin;
   };

   we will also need a `texture` struct:

   struct texture
   {
     Glint tex_id;
     char *res_path;
     uint32 width;
     uint32 height;
     GLint filtering_mode;
   };

   if we wanna change the origin:

   s.origin = ORIGIN_CENTER | ORIGIN_TOPLEFT;

   when drawing


   //we set the position or whatever to the desired one before drawing:
   s.pos = glm::vec2(20,10);



   //at scale=1, it will draw the texture as it is on the disk. same width and same height of pixels.
   s.draw();

   //we can draw the same sprite on the same frame if desired:

   for(int i = 0;i<5;++i)
   {
     s.pos.x += 50.f;
     s.draw();
   }

   */

