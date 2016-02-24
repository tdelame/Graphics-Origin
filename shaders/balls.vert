#version 440
in vec4  ball_attribute;
in vec3 color_attribute;

out vec4 ball ;
out vec3 color;

void main()
{
  ball  =  ball_attribute;
  color = color_attribute;
}