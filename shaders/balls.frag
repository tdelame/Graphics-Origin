#version 440
uniform mat4 projection;
smooth in vec3 fragment_position;
in vec4 diffuse_color;
// contains:
//   in xyz: the ball center in camera coordinates
//   in w: 4 times ( squared distance to camera - squared radius ) 
in vec4 data;

out vec4 final_color;

void main()
{
  vec3 ray_direction = normalize( fragment_position );
  
  float B = - 2.0 * dot( ray_direction, data.xyz );
  float det = B * B - data.w;
  if( det < 0.0 )
  	discard;
  det = sqrt( det );
  
  // the first intersection if for the minimum t solution
  // such minimum is min( -B + det, -B - det ) * 0.5
  vec3 sphere_position = ray_direction * min( -B + det, -B - det ) * 0.5;
  vec3 fragment_normal = normalize( sphere_position - data.xyz );
  
  // the depth of the fragment has been updated. Indeed, it does not lie on the
  // quad anymore, but on the ball's surface
  vec4 point_clip = projection * vec4( sphere_position, 1.0);
  float ndcDepth = point_clip.z / point_clip.w;
  gl_FragDepth = ((gl_DepthRange.diff * ndcDepth) + gl_DepthRange.near + gl_DepthRange.far) * 0.5;
  
  final_color =
    vec4( ( 0.2 + max( dot( -ray_direction, fragment_normal ), 0.0 )) * diffuse_color.xyz,
	   diffuse_color.w );
}