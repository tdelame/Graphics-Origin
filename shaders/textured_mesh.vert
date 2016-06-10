#version 440
uniform mat3 nit;
uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

in vec3 position;
in vec3 normal;
in vec2 texture;

out vec3 frag_position;
out vec3 frag_normal;
out vec2 frag_texture;

void main()
{
  frag_position = vec3(view * model * vec4( position, 1.0 ));
  frag_normal = (view * vec4( nit * normal, 0 )).xyz;
  frag_texture = texture; 
  gl_Position = projection * vec4( frag_position, 1.0);
}