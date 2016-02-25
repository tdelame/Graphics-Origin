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

const vec2 quad_directions[4] = vec2[4](
	vec2( -1, -1 ),
	vec2(  1, -1 ),
	vec2( -1,  1 ),
	vec2(  1,  1 )
);


void main()
{
  ball_in_camera   = mv * vec4( ball[0].xyz, 1.0 );
  ball_in_camera.w = ball[0].w;
  diffuse_color = color[0];
  
  for( int i = 0; i < 4; ++ i )
    {
      texture_coordinate = box_correction * quad_directions[i];
      gl_Position = projection * vec4( 
      	ball_in_camera.xyz + vec3( texture_coordinate * ball_in_camera.w, 0 ),
      	1.0 ); 
      EmitVertex();
    }
  EndPrimitive();
}