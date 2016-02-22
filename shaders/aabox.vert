#version 440
mat4 projection, view;

in vec3 center;
in vec3 hsides;

out vec3 scale;

void main()
{
  scale = hsides;
  gl_Position = projection * view * vec4( center, 1.0 );
}