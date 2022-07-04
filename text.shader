#shader vert
#version 330 core
layout (location = 0) in vec4 coord;

out vec2 tex_coord;

uniform mat4 mvp;

void main()
{
  tex_coord = coord.zw;
  gl_Position = mvp * vec4(coord.xy, 0, 1);
} 

#shader frag
#version 330 core

in vec2 tex_coord;
out vec4 color;

uniform sampler2D tex;
uniform vec4 tint_color;

void main()
{
//color = vec4(1,1,1,1);
  color = vec4(1, 1, 1, texture(tex, tex_coord).r) * tint_color;
}


