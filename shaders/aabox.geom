#version 440
mat4 projection, view, model;
 
const vec4 cube_vertices[8] = vec4[8](
  vec4(-0.5, -0.5, -0.5, 1),
  vec4( 0.5, -0.5, -0.5, 1),
  vec4( 0.5,  0.5, -0.5, 1),
  vec4(-0.5,  0.5, -0.5, 1),
  vec4(-0.5, -0.5,  0.5, 1),
  vec4( 0.5, -0.5,  0.5, 1),
  vec4( 0.5,  0.5,  0.5, 1),
  vec4(-0.5,  0.5,  0.5, 1) 
); 


layout( points ) in;
in vec3 scale[];

layout( points, max_vertices = 4 ) out;

void main()
{
  mat4 mvp = projection * view * model;
  
  vec4 scale = vec4( scale[0], 0.0 );
  
  gl_Position = mvp * ( gl_in[0].gl_Position + cube_vertices[0] * scale );
  EmitVertex();
  
  gl_Position = mvp * ( gl_in[0].gl_Position + cube_vertices[1] * scale );
  EmitVertex();
  
  gl_Position = mvp * ( gl_in[0].gl_Position + cube_vertices[2] * scale );
  EmitVertex();
  
  gl_Position = mvp * ( gl_in[0].gl_Position + cube_vertices[3] * scale );
  EmitVertex();
  
  EndPrimitive();
}