# version 440
out vec4 frag_color;

in vec3 out_normal;
in vec3 out_vertex;
noperspective in vec3 dist;

uniform vec3 light_position;// = vec4(0,0,0,1);
const vec4 light_ambient  = vec4(0.1, 0.1, 0.1, 1);
const vec4 light_specular = vec4(1.0, 1.0, 1.0, 1);
const vec4 light_diffuse  = vec4(1.0, 1.0, 1.0, 1);
const vec4 mesh_color     = vec4(0.3, 0.5, 0.9, 1);

void main()
{
  vec3 L = normalize( light_position - out_vertex );
  vec3 E = normalize( -out_vertex );
  vec3 R = -reflect( L, out_normal );

  vec3 i_ambient  = light_ambient.xyz;
  vec3 i_diffuse  = light_diffuse.xyz * max( dot( out_normal, L ), 0.0 );
  vec3 i_specular = light_specular.xyz * pow( max( dot(R, E ), 0.0 ), 5 );

  float nearD = min(min(dist[0],dist[1]),dist[2]);
  float edge_intensity = exp2( -1.0 * nearD * nearD );
  
  frag_color =  edge_intensity * vec4(0.1,0.1,0.1,1.0) 
    + (1.0 - edge_intensity) * ( vec4(i_ambient + clamp( i_diffuse, 0, 1 ) + clamp( i_specular, 0, 1 ),1)*mesh_color);
}
