#version 440
uniform mat4 vp;

const vec4 window_vertices[4] = vec4[4](
  vec4( 0,-1, 1,1),
  vec4( 0,-1,-1,1),
  vec4( 0, 1, 1,1),
  vec4( 0, 1,-1,1),
);

layout( points ) in;
in mat4 window_model[];
in vec4 window_color[];

layout( triangle_strip, max_vertices = 4 ) out;
out vec4 fragment_color;

void main()
{
  const mat4 mvp = vp * window_model[0];
  
  fragment_color = window_color[0];
  gl_Position = mvp * window_vertices[0];
  EmitVertex();
  
  gl_Position = mvp * window_vertices[1];
  EmitVertex();
  
  gl_Position = mvp * window_vertices[2];
  EmitVertex();
  
  gl_Position = mvp * window_vertices[3];
  EmitVertex();
  EndPrimitive();
}