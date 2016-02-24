#version 440

layout( points ) in;
in float ball_radius[];
in vec3  ball_color [];

layout( triangle_strip, max_vertices = 4 ) out;
smooth out vec2 texture_coordinate;
out vec3 diffuse_color;
out vec4 ball_in_camera;

//todo: build formulae to have a tight fitting quad and check if it is faster
const float box_correction = 1.5;

void main()
{
  center_in_camera = vec4(gl_in[0].gl_Position.xyz, ball_radius[0]);
  diffuse_color = ball_color[0];
  
  texture_coordinate = box_correction * vec2( -1, -1 );
  gl_Position = projection * ( vec4( texture_coordinate * ball_radius[0], 0, 0 ) + gl_in[0].gl_Position );
  EmitVertex();
  
	texture_coordinate = box_correction * vec2( 1, -1 );
  gl_Position = projection * ( vec4( texture_coordinate * ball_radius[0], 0, 0 ) + gl_in[0].gl_Position );
  EmitVertex();
  
  texture_coordinate = box_correction * vec2( -1, 1 );
  gl_Position = projection * ( vec4( texture_coordinate * ball_radius[0], 0, 0 ) + gl_in[0].gl_Position );
  EmitVertex();
  
	texture_coordinate = box_correction * vec2( 1, 1 );
  gl_Position = projection * ( vec4( texture_coordinate * ball_radius[0], 0, 0 ) + gl_in[0].gl_Position );
  EmitVertex();  
   
  EndPrimitive();
}