#version 440
uniform mat3 nit;
uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

in vec3 position;
in vec3 normal;

out vec3 camera_vertex;
out vec3 camera_normal;

void main()
{
  camera_vertex = vec3(view * model * vec4( position, 1.0 ));
  camera_normal = nit * normal;
  gl_Position = projection * vec4(camera_vertex,1.0);
}