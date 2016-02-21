#version 440
mat4 projection, view, model;

layout( points ) in;
in vec3 geom_center[];
in vec3 geom_half_sides[];

layout( line_strip, max_vertices = 4 ) out;

void main()
{
  mat4 MVP = projection * view * model;
  vec3 c = geom_center[0];
  gl_Position = MVP * vec4( vec3( 
  		c.x - geom_half_sides[0].x, 
  		c.y - geom_half_sides[0].y, 
  		c.z - geom_half_sides[0].z), 1 );
  EmitVertex();
  
  gl_Position = MVP * vec4( vec3( 
  		c.x + geom_half_sides[0].x, 
  		c.y - geom_half_sides[0].y, 
  		c.z - geom_half_sides[0].z), 1 );
  EmitVertex();
  
  gl_Position = MVP * vec4( vec3( 
  		c.x + geom_half_sides[0].x, 
  		c.y + geom_half_sides[0].y, 
  		c.z - geom_half_sides[0].z), 1 );
  EmitVertex();
  
  gl_Position = MVP * vec4( vec3( 
  		c.x - geom_half_sides[0].x, 
  		c.y + geom_half_sides[0].y, 
  		c.z - geom_half_sides[0].z), 1 );
  EmitVertex();

  EndPrimitive();

}