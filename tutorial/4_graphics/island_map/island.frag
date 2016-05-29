#version 440    
in vec2 tex_coordinates;
in vec3 world_coordinates;

out vec4 fragment;

uniform sampler2D terrain;

uniform vec3 camera_position;

struct gradient 
{
  vec3 color;
  float height;
};

 int number_of_gradients = 8;
 gradient gradients[8] = gradient[8](
  gradient( vec3( 0.00f, 0.00f, 0.50f ), -1.0000f ), //Point(-1.0000, noise::utils::Color(  0,   0, 128, 255)); // deeps
  gradient( vec3( 0.00f, 0.00f, 1.00f ), -0.2500f ), //Point(-0.2500, noise::utils::Color(  0,   0, 255, 255)); // shallow
  gradient( vec3( 0.00f, 0.50f, 1.00f ),  0.0000f ), //Point( 0.0000, noise::utils::Color(  0, 128, 255, 255)); // shore
  gradient( vec3( 0.94f, 0.94f, 0.25f ),  0.0625f ), //Point( 0.0625, noise::utils::Color(240, 240,  64, 255)); // sand
  gradient( vec3( 0.12f, 0.93f, 0.00f ),  0.1250f ), //Point( 0.1250, noise::utils::Color( 32, 160,   0, 255)); // grass
  gradient( vec3( 0.87f, 0.87f, 0.00f ),  0.3750f ), //Point( 0.3750, noise::utils::Color(224, 224,   0, 255)); // dirt
  gradient( vec3( 0.50f, 0.50f, 0.50f ),  0.7500f ), //Point( 0.7500, noise::utils::Color(128, 128, 128, 255)); // rock
  gradient( vec3( 1.00f, 1.00f, 1.00f ),  1.0000f )  //Point( 1.0000, noise::utils::Color(255, 255, 255, 255)); // snow
);

vec3 color_gradient( float height )
{
  int index_position = 0;
  for( ; index_position < number_of_gradients; ++ index_position )
  {
    if( height < gradients[ index_position ].height )
      break;
  }
   
  int index0 = clamp( index_position - 1, 0, number_of_gradients - 1 );
  int index1 = clamp( index_position    , 0, number_of_gradients - 1 );
  
  if( index0 == index1 ) 
    return gradients[ index1 ].color; 
    
  float input0 = gradients[index0].height;
  float input1 = gradients[index1].height;
  float coeff  = (height - input0) / (input1 - input0);
  return mix( gradients[index0].color, gradients[index1].color, coeff );
}


void main(){
  vec3 surfel_to_light = normalize(camera_position - world_coordinates);
  float diffuse_factor = max( 0.0, dot( texture(terrain, tex_coordinates).xyz, surfel_to_light ));
  vec3 ground_color = color_gradient( world_coordinates.z );
  
  fragment =
      vec4( diffuse_factor * ground_color, 1.0 ) ;
}