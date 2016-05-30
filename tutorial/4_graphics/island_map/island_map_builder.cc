/*  Created on: May 25, 2016
 *      Author: T. Delame (tdelame@gmail.com) */
# include "island_map_builder.h"
# include "island.h"
# include <noise/noiseutils.h>
# include <chrono>
namespace graphics_origin {
  namespace application {

    class squared_length_module
      : public noise::module::Module {
    public:
      squared_length_module()
        : noise::module::Module(0),
          m_offset{0}, m_scale{1.0}
      {}

      void SetOffset( double offset )
      {
        m_offset = offset;
      }

      void SetScale( double scale )
      {
        m_scale = scale;
      }

      int GetSourceModuleCount() const override
      {
        return 0;
      }

      double GetValue( double x, double y, double z ) const
      {
        return m_offset + m_scale * ( x * x + y * y + z * z );
      }
    private:
      double m_offset;
      double m_scale;
    };

    static void
    debug_noise(
        noise::module::Module& module,
        float radius,
        const std::string& filename )
    {
      noise::utils::NoiseMap noise_map;
      noise::utils::NoiseMapBuilderPlane noise_map_builder;
      noise_map_builder.SetSourceModule( module );
      noise_map_builder.SetDestNoiseMap( noise_map );
      noise_map_builder.SetDestSize( 4096, 4096 );
      noise_map_builder.SetBounds( -radius, radius, -radius, radius );
      noise_map_builder.Build();

      noise::utils::Image noise_image;
      noise::utils::RendererImage renderer;
      renderer.SetSourceNoiseMap( noise_map );
      renderer.SetDestImage( noise_image );
      renderer.ClearGradient();
      renderer.AddGradientPoint(-1.0000, noise::utils::Color(  0,   0, 128, 255)); // deeps
      renderer.AddGradientPoint(-0.2500, noise::utils::Color(  0,   0, 255, 255)); // shallow
      renderer.AddGradientPoint( 0.0000, noise::utils::Color(  0, 128, 255, 255)); // shore
      renderer.AddGradientPoint( 0.0625, noise::utils::Color(240, 240,  64, 255)); // sand
      renderer.AddGradientPoint( 0.1250, noise::utils::Color( 32, 160,   0, 255)); // grass
      renderer.AddGradientPoint( 0.3750, noise::utils::Color(224, 224,   0, 255)); // dirt
      renderer.AddGradientPoint( 0.7500, noise::utils::Color(128, 128, 128, 255)); // rock
      renderer.AddGradientPoint( 1.0000, noise::utils::Color(255, 255, 255, 255)); // snow
      renderer.EnableLight();
      renderer.SetLightContrast(3.0);
      renderer.SetLightBrightness(2.0);
      renderer.Render();

      noise::utils::WriterBMP writer;
      writer.SetSourceImage( noise_image );
      writer.SetDestFilename( filename );
      writer.WriteDestFile();
    }

    island_map_builder::parameters::parameters()
      : m_land_frequency{ 1.0 }, m_land_lacunarity{ 2.0 }, m_land_persistance{ 0.5 },
        m_land_threshold{ 0.3 }, m_land_octaves{ 4 }, m_land_seed( std::chrono::system_clock::now().time_since_epoch().count() ),

        m_radius{ 5000 },
        m_min_elevation{ -150 }, m_max_elevation{ 1500 },

        m_heightmap_size{ 8192 }
    {}

    island_map_builder::island_map_builder( const parameters& params, island& output )
      : m_params{ params }, m_output{ output }
    {
      // land: a perlin noise
      noise::module::Perlin base_land_definition;
      base_land_definition.SetFrequency( m_params.m_land_frequency );
      base_land_definition.SetLacunarity( m_params.m_land_lacunarity );
      base_land_definition.SetPersistence( m_params.m_land_persistance );
      base_land_definition.SetOctaveCount( m_params.m_land_octaves );
      base_land_definition.SetSeed( m_params.m_land_seed );

      // this module can be seen as the probability to have land depending on
      // the distance to the center: the closer to the center we are, the more
      // we should have lands. We build here the opposite of this probability
      // to add it to the land module later.
      squared_length_module s2;
      s2.SetOffset( 1.0 - 2.0 * m_params.m_land_threshold );
      s2.SetScale( -double(2.0) / (m_params.m_radius * m_params.m_radius ) );

      // pretty useless for now, just to have a clear view of the lands
      noise::module::Const constant;
      constant.SetConstValue( 0.5 );

      // if the land noise is higher than the s2 module, we have land, otherwise
      // we have water. To implement this test, we simply add the land to the
      // opposite of the s2 module (the parameters of this module are such that
      // we already have the opposite). This gives us the control module.
      noise::module::Add control_module;
      control_module.SetSourceModule( 0, base_land_definition );
      control_module.SetSourceModule( 1, s2 );





      // land_selector: when the control module is in the range [0,+inf], we
      // have land and output the constant module. Otherwise, this is water
      // and we use (for now) the control module itself to render it in an
      // image.
      noise::module::Select land_selector;
      land_selector.SetSourceModule( 0, control_module );
      land_selector.SetSourceModule( 1, constant );
      land_selector.SetControlModule( control_module );
      land_selector.SetBounds( 0, 5 );

      output.set_radius( m_params.m_radius );
      output.set_heightmap( control_module, m_params.m_heightmap_size );//and_selector, m_params.m_heightmap_size );
      output.set_resolution( 0.0005 );


//      debug_noise( control_module, params.m_radius, "land.bmp");
//      debug_noise( land_selector, params.m_radius, "land.bmp");
    }

    void island_map_builder::output_results()
    {

    }
  }
}



