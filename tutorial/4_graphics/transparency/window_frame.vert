#version 440
in vec3 center;
in vec3 v1;
in vec3 v2;
in float size;
in float depth;

out vec3 frame_center;
out vec3 frame_v1;
out vec3 frame_v2;
out float frame_size;
out float frame_depth;

void main()
{
  frame_center = center;
  frame_v1 = v1;
  frame_v2 = v2;
  frame_size = size;
  frame_depth = depth;
} 