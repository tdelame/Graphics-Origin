#version 440    
in vec2 tex_coordinates;
in float depth;
out vec4 fragment;

//uniform sampler2D diffuse;
//uniform sampler2D terrain;
//uniform sampler2D noise_tile;

//vec3 incident = normalize(vec3(1.0, 0.2, 0.5));
//vec4 light = vec4(1.0, 0.95, 0.9, 1.0) * 1.1;


struct gradient 
{
  vec4 color;
  float height;
};

 int number_of_gradients = 8;
 gradient gradients[8] = gradient[8](
  gradient( vec4( 0.00f, 0.00f, 0.50f, 1.0f ), -1.0000f ), //Point(-1.0000, noise::utils::Color(  0,   0, 128, 255)); // deeps
  gradient( vec4( 0.00f, 0.00f, 1.00f, 1.0f ), -0.2500f ), //Point(-0.2500, noise::utils::Color(  0,   0, 255, 255)); // shallow
  gradient( vec4( 0.00f, 0.50f, 1.00f, 1.0f ),  0.0000f ), //Point( 0.0000, noise::utils::Color(  0, 128, 255, 255)); // shore
  gradient( vec4( 0.94f, 0.94f, 0.25f, 1.0f ),  0.0625f ), //Point( 0.0625, noise::utils::Color(240, 240,  64, 255)); // sand
  gradient( vec4( 0.12f, 0.93f, 0.00f, 1.0f ),  0.1250f ), //Point( 0.1250, noise::utils::Color( 32, 160,   0, 255)); // grass
  gradient( vec4( 0.87f, 0.87f, 0.00f, 1.0f ),  0.3750f ), //Point( 0.3750, noise::utils::Color(224, 224,   0, 255)); // dirt
  gradient( vec4( 0.50f, 0.50f, 0.50f, 1.0f ),  0.7500f ), //Point( 0.7500, noise::utils::Color(128, 128, 128, 255)); // rock
  gradient( vec4( 1.00f, 1.00f, 1.00f, 1.0f ),  1.0000f )  //Point( 1.0000, noise::utils::Color(255, 255, 255, 255)); // snow
);

vec4 color_gradient( float height )
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
   fragment = color_gradient( depth );
 
    //vec3 normal = normalize(texture(terrain, texcoord).xyz);
    //vec4 color = texture(diffuse, texcoord);
    //float noise_factor = texture(noise_tile, texcoord*32).r+0.1;

    //float dot_surface_incident = max(0, dot(normal, incident));

    //color = color * light * noise_factor * (max(0.1, dot_surface_incident)+0.05)*1.5;
    //fragment = mix(color, color*0.5+vec4(0.5, 0.5, 0.5, 1.0), depth*2.0);
}