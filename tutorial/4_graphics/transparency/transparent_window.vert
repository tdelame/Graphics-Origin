#version 440
in vec3 center;
in vec3 v1;
in vec3 v2;
in vec4 color;

out vec3 window_center;
out vec3 window_v1;
out vec3 window_v2;
out vec4 window_color;

void main()
{
  window_center = center;
  window_v1 = v1;
  window_v2 = v2;
  window_color = color;
} 