# version 440
uniform vec2 window_dimensions;

layout( triangles ) in;
in vec3 camera_vertex[];
in vec3 camera_normal[];

layout( triangle_strip, max_vertices = 3 ) out;
out vec3 out_normal;
out vec3 out_vertex;
noperspective out vec3 dist;

void main(void)
{
  vec2 p0 = window_dimensions * gl_in[0].gl_Position.xy / gl_in[0].gl_Position.w;
  vec2 p1 = window_dimensions * gl_in[1].gl_Position.xy / gl_in[1].gl_Position.w;
  vec2 p2 = window_dimensions * gl_in[2].gl_Position.xy / gl_in[2].gl_Position.w;

  vec2 v0 = p2 - p1;
  vec2 v1 = p2 - p0;
  vec2 v2 = p1 - p0;
  float area = abs(v1.x*v2.y - v1.y * v2.x);
  
  dist = vec3( area / length(v0), 0, 0 );
  out_vertex = camera_vertex[0];
  out_normal = camera_normal[0];
  gl_Position = gl_in[0].gl_Position;
  EmitVertex();  

  dist = vec3( 0, area / length(v1), 0 );
  out_vertex = camera_vertex[1];
  out_normal = camera_normal[1];
  gl_Position = gl_in[1].gl_Position;
  EmitVertex();
  
  
  dist = vec3( 0, 0, area / length(v2) );
  out_vertex = camera_vertex[2];
  out_normal = camera_normal[2];
  gl_Position = gl_in[2].gl_Position;
  EmitVertex();
  
  EndPrimitive();  
}