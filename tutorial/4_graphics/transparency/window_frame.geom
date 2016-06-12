#version 440
uniform mat4 vp;

layout( points ) in;
in vec3 frame_center[];
in vec3 frame_v1[];
in vec3 frame_v2[];
in float frame_size[];
in float frame_depth[];

layout( triangle_strip, max_vertices = 30 ) out;

void main()
{
  float inv_v1_length = float(1.0) / length( frame_v1[0] );
  float inv_v2_length = float(1.0) / length( frame_v2[0] );
  vec3 v3_shift = normalize( cross( frame_v1[0], frame_v2[0] ) ) * frame_depth[0];
  
  vec3 v1_exterior_shift = frame_v1[0] * ( 1 + inv_v1_length * frame_size[0] );
  vec3 v1_interior_shift = frame_v1[0] * ( 1 - inv_v1_length * frame_size[0] );
  vec3 v2_exterior_shift = frame_v2[0] * ( 1 + inv_v2_length * frame_size[0] );
  vec3 v2_interior_shift = frame_v2[0] * ( 1 - inv_v2_length * frame_size[0] );
  
  gl_Position = vp * vec4( frame_center[0] - v1_exterior_shift + v3_shift, 1.0 );
  EmitVertex();
  gl_Position = vp * vec4( frame_center[0] - v1_interior_shift + v3_shift, 1.0 );
  EmitVertex();
  
  gl_Position = vp * vec4( frame_center[0] - v2_exterior_shift + v3_shift, 1.0 );
  EmitVertex();
  gl_Position = vp * vec4( frame_center[0] - v2_interior_shift + v3_shift, 1.0 );
  EmitVertex();
  
  gl_Position = vp * vec4( frame_center[0] + v1_exterior_shift + v3_shift, 1.0 );
  EmitVertex();
  gl_Position = vp * vec4( frame_center[0] + v1_interior_shift + v3_shift, 1.0 );
  EmitVertex();
  
  gl_Position = vp * vec4( frame_center[0] + v2_exterior_shift + v3_shift, 1.0 );
  EmitVertex();
  gl_Position = vp * vec4( frame_center[0] + v2_interior_shift + v3_shift, 1.0 );
  EmitVertex();
  
  gl_Position = vp * vec4( frame_center[0] - v1_exterior_shift + v3_shift, 1.0 );
  EmitVertex();
  gl_Position = vp * vec4( frame_center[0] - v1_interior_shift + v3_shift, 1.0 );
  EmitVertex();
  EndPrimitive();
  
  gl_Position = vp * vec4( frame_center[0] - v1_exterior_shift - v3_shift, 1.0 );
  EmitVertex();
  gl_Position = vp * vec4( frame_center[0] - v1_interior_shift - v3_shift, 1.0 );
  EmitVertex();
  
  gl_Position = vp * vec4( frame_center[0] - v2_exterior_shift - v3_shift, 1.0 );
  EmitVertex();
  gl_Position = vp * vec4( frame_center[0] - v2_interior_shift - v3_shift, 1.0 );
  EmitVertex();
  
  gl_Position = vp * vec4( frame_center[0] + v1_exterior_shift - v3_shift, 1.0 );
  EmitVertex();
  gl_Position = vp * vec4( frame_center[0] + v1_interior_shift - v3_shift, 1.0 );
  EmitVertex();
  
  gl_Position = vp * vec4( frame_center[0] + v2_exterior_shift - v3_shift, 1.0 );
  EmitVertex();
  gl_Position = vp * vec4( frame_center[0] + v2_interior_shift - v3_shift, 1.0 );
  EmitVertex();
  
  gl_Position = vp * vec4( frame_center[0] - v1_exterior_shift - v3_shift, 1.0 );
  EmitVertex();
  gl_Position = vp * vec4( frame_center[0] - v1_interior_shift - v3_shift, 1.0 );
  EmitVertex();
  EndPrimitive();
  
  
  gl_Position = vp * vec4( frame_center[0] - v1_exterior_shift - v3_shift, 1.0 );
  EmitVertex();
  gl_Position = vp * vec4( frame_center[0] - v1_exterior_shift + v3_shift, 1.0 );
  EmitVertex();
  
  gl_Position = vp * vec4( frame_center[0] - v2_exterior_shift - v3_shift, 1.0 );
  EmitVertex();
  gl_Position = vp * vec4( frame_center[0] - v2_exterior_shift + v3_shift, 1.0 );
  EmitVertex();
  
  gl_Position = vp * vec4( frame_center[0] + v1_exterior_shift - v3_shift, 1.0 );
  EmitVertex();
  gl_Position = vp * vec4( frame_center[0] + v1_exterior_shift + v3_shift, 1.0 );
  EmitVertex();
  
  gl_Position = vp * vec4( frame_center[0] + v2_exterior_shift - v3_shift, 1.0 );
  EmitVertex();
  gl_Position = vp * vec4( frame_center[0] + v2_exterior_shift + v3_shift, 1.0 );
  EmitVertex();
  
  gl_Position = vp * vec4( frame_center[0] - v1_exterior_shift - v3_shift, 1.0 );
  EmitVertex();
  gl_Position = vp * vec4( frame_center[0] - v1_exterior_shift + v3_shift, 1.0 );
  EmitVertex();  
  EndPrimitive();
}