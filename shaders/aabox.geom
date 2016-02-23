#version 440
uniform mat4 mvp;
 
const vec3 cube_vertices[8] = vec3[8](
  vec3(-0.5, -0.5, -0.5),
  vec3( 0.5, -0.5, -0.5),
  vec3( 0.5,  0.5, -0.5),
  vec3(-0.5,  0.5, -0.5),
  vec3(-0.5, -0.5,  0.5),
  vec3( 0.5, -0.5,  0.5),
  vec3( 0.5,  0.5,  0.5),
  vec3(-0.5,  0.5,  0.5) 
); 


layout( points ) in;
in vec3 position[];
in vec3 scale[];

layout( line_strip, max_vertices = 16 ) out;

void main()
{
  const vec4 v0 = mvp * vec4( position[0] + cube_vertices[0] * scale[0], 1 );
  const vec4 v1 = mvp * vec4( position[0] + cube_vertices[1] * scale[0], 1 );
  const vec4 v2 = mvp * vec4( position[0] + cube_vertices[2] * scale[0], 1 );
  const vec4 v3 = mvp * vec4( position[0] + cube_vertices[3] * scale[0], 1 );
  const vec4 v4 = mvp * vec4( position[0] + cube_vertices[4] * scale[0], 1 );
  const vec4 v5 = mvp * vec4( position[0] + cube_vertices[5] * scale[0], 1 );
  const vec4 v6 = mvp * vec4( position[0] + cube_vertices[6] * scale[0], 1 );
  const vec4 v7 = mvp * vec4( position[0] + cube_vertices[7] * scale[0], 1 );
  
  gl_Position = v0;
  EmitVertex();
  
  gl_Position = v1;
  EmitVertex();
  
  gl_Position = v2;
  EmitVertex();
  
  gl_Position = v3;
  EmitVertex();
  
  gl_Position = v0;
  EmitVertex();
  
  gl_Position = v4;
  EmitVertex();
  
  gl_Position = v5;
  EmitVertex();
  
  gl_Position = v6;
  EmitVertex();
  
  gl_Position = v7;
  EmitVertex();
  
  gl_Position = v4;
  EmitVertex();
  EndPrimitive();
  
  gl_Position = v1;
  EmitVertex();
  gl_Position = v5;
  EmitVertex();
  EndPrimitive();
  
  gl_Position = v2;
  EmitVertex();
  gl_Position = v6;
  EmitVertex();
  EndPrimitive();
  
  gl_Position = v3;
  EmitVertex();
  gl_Position = v7;
  EmitVertex();
  EndPrimitive();
}