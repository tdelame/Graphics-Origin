# version 440
in vec3 frag_position;
in vec3 frag_normal;
in vec2 frag_texture;

uniform sampler2D sampler;

uniform vec3 light_position;// = vec4(0,0,0,1);
const vec4 light_ambient  = vec4(0.3, 0.3, 0.3, 1);
const vec4 light_specular = vec4(1.0, 1.0, 1.0, 1);
const vec4 light_diffuse  = vec4(1.0, 1.0, 1.0, 1);
const vec4 mesh_color     = vec4(0.3, 0.5, 0.9, 1);

out vec4 final_color;

void main()
{
  vec3 L = normalize( light_position - frag_position );
  vec3 E = normalize( -frag_position );
  vec3 R = -reflect( L, frag_normal );

  vec3 i_ambient  = light_ambient.xyz;
  vec3 i_diffuse  = light_diffuse.xyz * max( dot( frag_normal, L ), 0.0 );
  vec3 i_specular = light_specular.xyz * pow( max( dot(R, E ), 0.0 ), 5 );
  final_color =  vec4( texture( sampler, frag_texture ).rgb * ( i_ambient + clamp( i_diffuse, 0, 1 ) + clamp( i_specular, 0, 1 )),1);
}
