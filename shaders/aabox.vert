#version 440
in vec3 center;
in vec3 hsides;

out vec3 position;
out vec3 scale;

void main()
{
  scale = hsides;
  position = center;
}