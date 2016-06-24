#version 440
in vec2 position;
in vec2 texcoord;

out vec2 tc_texcoord;

uniform sampler2D terrain;
    
void main( )
{
  tc_texcoord = texcoord;
  gl_Position = vec4( position, texture( terrain, texcoord ).a, 1.0 );
}