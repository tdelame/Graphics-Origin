#version 440
in mat4 model;
in vec4 color;

out mat4 window_model;
out vec4 window_color;

void main()
{
  window_model = model;
  window_color = color;
} 