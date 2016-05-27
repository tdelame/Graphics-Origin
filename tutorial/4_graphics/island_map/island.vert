#version 440
in vec4 position;
uniform sampler2D terrain;
uniform float inv_radius;
    
void main( )
{
  vec2 tex_coordinates = position.xy * inv_radius * 0.5f + 0.5f;
  float height = texture( terrain, tex_coordinates ).a;
  gl_Position = vec4( position.xy, height, 1.0 );
}