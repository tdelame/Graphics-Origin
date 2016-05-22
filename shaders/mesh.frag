# version 440
out vec4 frag_color;

in vec3 camera_normal;
in vec3 camera_vertex;

uniform vec3 light_position;// = vec4(0,0,0,1);
const vec4 light_ambient  = vec4(0.3, 0.3, 0.3, 1);
const vec4 light_specular = vec4(1.0, 1.0, 1.0, 1);
const vec4 light_diffuse  = vec4(1.0, 1.0, 1.0, 1);
const vec4 mesh_color     = vec4(0.3, 0.5, 0.9, 1);

void main()
{
  vec3 L = normalize( light_position - camera_vertex );
  vec3 E = normalize( -camera_vertex );
  vec3 R = -reflect( L, camera_normal );

  vec3 i_ambient  = light_ambient.xyz;
  vec3 i_diffuse  = light_diffuse.xyz * max( dot( camera_normal, L ), 0.0 );
  vec3 i_specular = light_specular.xyz * pow( max( dot(R, E ), 0.0 ), 5 );
  frag_color =  vec4(i_ambient + clamp( i_diffuse, 0, 1 ) + clamp( i_specular, 0, 1 ),1)*mesh_color;
}
