// it takes a grayscale as a texture
// white -> white
// 0.5 -> sprite_color
// black -> black


#shader vert
#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 tex_coord;

uniform mat4 model;
uniform mat4 projection;
uniform float uv_multiplier;

void main()
{
  tex_coord = vertex.zw * uv_multiplier;
  gl_Position = projection
       * model * vec4(vertex.xy, 0.0, 1.0);
}


#shader frag
#version 330 core
in vec2 tex_coord;
out vec4 color;

uniform sampler2D image;
uniform vec4 sprite_color;

void main()
{
  vec4 tex = texture(image, tex_coord);
//  vec3 final_col = mix(vec3(1,1,1),
//                       sprite_color.xyz,
//                       tex.r);
//  color = vec4(final_col,tex.a * sprite_color.a);
  /* my version
  vec3 tex_2 = tex.xyz * 2;
  vec3 dark_part = mix(vec3(0,0,0),
                       sprite_color.xyz,
                       tex.r * 2);

  vec3 light_part = mix(sprite_color.xyz,
                        vec3(1,1,1),
                        (tex.r - 0.5) * 2);
  if(tex.r < 0.5)
    color = vec4(dark_part, tex.a * sprite_color.a);
  else
    color = vec4(light_part, tex.a * sprite_color.a);
    */


  //d7's version

float  intensity = tex.r * 2.0f; // sampled grayscale
color = vec4(
         vec3(
           mix(
             min(intensity, 1.0f) * sprite_color.xyz,
             vec3(1.0f),
             max(intensity - 1.0f, 0.0f))),
         sprite_color.a * tex.a);
}  
