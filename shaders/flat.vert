#version 400

uniform mat4 mvp;

in vec3 position;
in vec3 color;

out vec3 fragment_color;

void main()
{
  gl_Position = mvp * vec4( position, 1.0 );
  fragment_color = color;
}