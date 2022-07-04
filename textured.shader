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
  color = vec4(tex * sprite_color);
}
