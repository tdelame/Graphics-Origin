#version 440
layout( points ) in;

layout( triangle_strip, max_vertices = 4 ) out;
out vec2 texture_coordinate;

void main()
{
  gl_Position = vec4(-1,-1,0,1);
  texture_coordinate = vec2(0,0);
  EmitVertex();
  
  gl_Position = vec4(-1,1,0,1);
  texture_coordinate = vec2(0,1);
  EmitVertex();
  
  gl_Position = vec4(1,-1,0,1);
  texture_coordinate = vec2(1,0);
  EmitVertex();
  
  gl_Position = vec4(1,1,0,1);
  texture_coordinate = vec2(1,1);
  EmitVertex();
  EndPrimitive();
}

