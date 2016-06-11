#version 440
uniform sampler2D sampler;

in vec2 texture_coordinate;

out vec4 final_color;

void main()
{
  final_color = texture( sampler, texture_coordinate );
}
 