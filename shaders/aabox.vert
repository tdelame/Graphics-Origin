#version 440
in vec3 center;
in vec3 half_sides;

out vec3 geom_center;
out vec3 geom_half_sides;

void main()
{
  geom_center = center;
  geom_half_sides = half_sides;
}