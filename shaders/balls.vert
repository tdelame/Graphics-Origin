#version 440
in vec4  ball_attribute;
in vec4 color_attribute;

out vec4 ball ;
out vec4 color;

void main()
{
  ball  =  ball_attribute;
  color = color_attribute;
}