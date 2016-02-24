#version 440
uniform mat4 projection;
smooth in vec2 texture_coordinate;
in vec3 diffuse_color;
in vec4 ball_in_camera;

out vec4 final_color;

void main()
{
  // position of the fragment: first on the quad then on the ball
  vec3 fragment_position = 
		ball_in_camera.xyz 
	  + vec3( ball_in_camera.w * texture_coordinate, 0 );
  
  // discard fragment with .z < 0 ?
  
  vec3 ray_direction = normalize( fragment_position );
  
  float B = 2.0 * dot( ray_direction, -ball_in_camera.xyz );
  float C = dot( ball_in_camera.xyz, ball_in_camera.xyz ) - ball_in_camera.w * ball_in_camera.w;
  float det = (B*B) - 4 * C;
  if( det < 0.0 )
  	discard;
  	
  det = sqrt( det );
  fragment_position = ray_direction * min( -B + det, -B - det ) * 0.5;
  vec3 fragment_normal = normalize( fragment_position - ball_in_camera.xyz );
  
  vec4 point_clip = projection * vec4( fragment_position, 1.0 );
  float ndcDepth = point_clip.z / point_clip.w;
  gl_FragDepth = ((gl_DepthRange.diff * ndcDepth) + gl_DepthRange.near + gl_DepthRange.far) * 0.5;
  
  final_color =
    vec4(
	   ( 0.2 + max( dot( -ray_direction, fragment_normal ), 0.0 )) * diffuse_color,
	   1.0 );
}