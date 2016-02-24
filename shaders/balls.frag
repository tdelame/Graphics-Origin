#version 440
smooth in vec3 texture_coordinate;
in vec3 diffuse_color;
in vec4 ball_in_camera;

out vec4 final_color;

void main()
{
  vec3 fragment_position = ball_in_camera.xyz + vec3( ball_in_camera.w * texture_coordinate, 0 );
  vec3 ray_direction = normalize( fragment_position );
  
  float B = 2.0 * dot( ray_direction, -ball_in_camera.xyz );
  float C = dot( ball_in_camera.xyz, ball_in_camera.xyz );
  float det = (B*B) - 4 * C;
  if( det < 0.0 )
  	discard;
  	
  det = sqrt( det );
  fragment_position = ray_direction * min( -B + det, -B - det ) * 0.5;
  vec3 fragment_normal = normalize( fragment_position - ball_in_camera.xyz );
  
  final_color = ( 0.2 + max( dot( -fragment_position 
  
  
  gl_FragDepth = ((gl_DepthRange.diff * ndcDepth) + gl_DepthRange.near + gl_DepthRange.far) * 0.5;

}