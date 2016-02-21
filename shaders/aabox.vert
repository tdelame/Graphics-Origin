#version 440

mat4 projection, view, model;

in vec3 center;
in vec3 half_sides;

out vec3 scale;

void main()
{
  scale = half_sides;
  gl_Position = projection * view * model * vec4( center, 1.0 );
}