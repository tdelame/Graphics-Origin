#version 440
uniform mat4 projection;
uniform mat4 mv;

layout( points ) in;
in vec4 ball [];
in vec3 color[];

layout( triangle_strip, max_vertices = 4 ) out;
smooth out vec2 texture_coordinate;
out vec3 diffuse_color;
out vec4 ball_in_camera;

//todo: build formulae to have a tight fitting quad and check if it is faster
const float box_correction = 1.5;

void main()
{
  ball_in_camera = vec4( 
  	(mv * vec4( ball[0].xyz, 1.0 )).xyz,
  	 ball[0].w 
  );
  diffuse_color = color[0];
  
  texture_coordinate = box_correction * vec2( -1, -1 );
  gl_Position = projection * vec4( vec3( texture_coordinate * ball_in_camera.w, 0 ) + ball_in_camera.xyz, 1.0 );
  EmitVertex();
  
  texture_coordinate = box_correction * vec2( 1, -1 );
  gl_Position = projection * vec4( vec3( texture_coordinate * ball_in_camera.w, 0 ) + ball_in_camera.xyz, 1.0 );
  EmitVertex();
  
  texture_coordinate = box_correction * vec2( -1, 1 );
  gl_Position = projection * vec4( vec3( texture_coordinate * ball_in_camera.w, 0 ) + ball_in_camera.xyz, 1.0 );
  EmitVertex();
  
  texture_coordinate = box_correction * vec2( 1, 1 );
  gl_Position = projection * vec4( vec3( texture_coordinate * ball_in_camera.w, 0 ) + ball_in_camera.xyz, 1.0 );
  EmitVertex();  
   
  EndPrimitive();
}