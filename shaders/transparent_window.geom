#version 440
uniform mat4 vp;

layout( points ) in;
in vec3 window_center[];
in vec3 window_v1[];
in vec3 window_v2[];
in vec4 window_color[];

layout( triangle_strip, max_vertices = 4 ) out;
out vec4 fragment_color;

void main()
{
  fragment_color = window_color[0];
  gl_Position = vp * vec4( window_center[0] - window_v1[0], 1.0 );
  EmitVertex();
  
  gl_Position = vp * vec4( window_center[0] - window_v2[0], 1.0 );
  EmitVertex();
    
  gl_Position = vp * vec4( window_center[0] + window_v2[0], 1.0 );
  EmitVertex();
  
  gl_Position = vp * vec4( window_center[0] + window_v1[0], 1.0 );
  EmitVertex();
  EndPrimitive();
}