#version 440
in vec3 center;
in vec3 hsides;
in vec3 color;

out vec3 box_center;
out vec3 box_scale;
out vec3 box_color;

void main()
{
  box_scale  = hsides;
  box_center = center;
  box_color  = color ;
}