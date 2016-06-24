/* Created on: May 23, 2016
 *     Author: T.Delame (tdelame@gmail.com)
 */
# include "../../graphics-origin/application/gl_helper.h"
# include "../../graphics-origin/application/window.h"
# include "../../graphics-origin/application/renderable.h"
# include "../../graphics-origin/application/shader_program.h"
# include "../../graphics-origin/tools/resources.h"
# include "../../graphics-origin/tools/random.h"
# include "../../graphics-origin/tools/log.h"

# include <GL/glew.h>

# include "common/simple_camera.h"
# include "common/simple_qml_application.h"
# include "common/simple_gl_renderer.h"

# include "island_map/island.h"

# include <chrono>

# include <QGuiApplication>

# include <noise/noiseutils.h>
# include <noise/noise.h>

/**
 * This tutorial is about the generation of an island map. I took a great
 * pleasure while building this tutorial, playing with parameters, building
 * more and more complex noises to obtain what I had in mind. I played so
 * much that the result was a little too obscure for a tutorial. So this
 * version is really simple and I let you read those two references to have
 * more ideas to obtain the terrain YOU want:
 * - [1] http://www-cs-students.stanford.edu/~amitp/game-programming/polygon-map-generation/
 * - [2] http://libnoise.sourceforge.net/index.html (in particular the complex_planet.cc example)
 */

namespace graphics_origin {
  namespace application {

    /**@brief Define the place where the island(s) will be.
     *
     * We wand to have an island to render. Thus we need to have some land to
     * appear in our square map, centered at the origin,  with a side equals to
     * two time the radius R.
     * In particular, the probability to have land at a position p should increase
     * with ||p||, such that having land at the boundaries of the map is unlikely.
     * One method method to do so [1] is to compare the value of a coherent noise
     * b(p) with s2[c,K](p) = c.||p||^2 + K, with c > 0:
     * if b(p) > s2[c,K](p) we have land.
     *
     * b(p) is (mostly) in [-1,1], and c.||p||^2 + K >= K.
     * - K should then be < 1 to have some land in the map.
     * - we take c = 1 / R^1 such that s2[c,0](p) < 1 when ||p|| < R. Thus,
     * for any p s.t. ||p|| < R, we have s2[c,K](p) in [K,K+1].
     * - finally, s2[c,K](p) should be >= 1 to constrain the existence of lands in
     * the map. This means K >= -1.
     *
     * In conclusion, if c = 1 / R^1 and K in [-1,1]:
     * - s2[c,K](p) in [K,K+1] included in [-1,2] for p such that ||p|| <= R
     * - land can exist for || p || <= R
     * - land is unlikely to exist for || p || > R
     *
     * Given the constant c,K and R, as well as the parameters for the coherent
     * noise b, this class implements the test to determine if a point is a land.
     */
    class land_definition {
    public:
      struct parameters {
        double m_base_frequency;
        double m_base_lacunarity;
        double m_base_persistance;
        uint m_base_octaves;
        int m_base_seed;

        double m_constant_threshold;
        double m_quadratic_threshold;

        parameters( double radius )
          : m_base_frequency{ double(1.0/radius) }, m_base_lacunarity{ 2.0 },
            m_base_persistance{ 0.5 }, m_base_octaves{ 6 }, m_base_seed{ 0 },

            m_constant_threshold{ 0 }, m_quadratic_threshold{ double(1.0/(radius*radius))}
        {}
      };

      land_definition( const parameters& params ) :
        m_constant_threshold{ params.m_constant_threshold },
        m_quadratic_threshold{ params.m_quadratic_threshold }
      {
        m_base.SetFrequency( params.m_base_frequency );
        m_base.SetLacunarity( params.m_base_lacunarity );
        m_base.SetPersistence( params.m_base_persistance );
        m_base.SetOctaveCount( params.m_base_octaves );
        m_base.SetSeed( params.m_base_seed );
      }

      /**@brief Get the value of the test.
       *
       * Get the value of the land test for a point p(x,y,z). If this value
       * is negative, then p is in water. If this value is positive, then
       * p is in a land. For a value around 0, then p is likely to be on a shore.
       * @param x The x component of point p
       * @param y The y component of point p
       * @param z The z component of point p
       * @return The test value b(p) - s2[c,K](p)
       */
      double land_value( double x, double y, double z ) const
      {
        return m_base.GetValue( x, y, z )
          - m_constant_threshold
          - m_quadratic_threshold * ( x * x + y * y + z * z );
      }
    private:
      noise::module::Perlin m_base;
      double m_constant_threshold;
      double m_quadratic_threshold;
    };

    struct island_map_metrics {
      double m_radius;
      double m_maximum_elevation;
      double m_horizontal_resolution;

      island_map_metrics( double radius )
        : m_radius{ radius },
          m_maximum_elevation{ 1000 },
          m_horizontal_resolution{ 1.0 }
      {}
    };

    struct island_map_parameters {
      island_map_metrics m_metrics;
      land_definition::parameters m_land_definition;
      double m_shore_threshold;

      island_map_parameters( double radius )
        : m_metrics{ radius },
          m_land_definition{ radius },
          m_shore_threshold{ 0.05 }
      {}
    };

    /**@brief Noise module to generate the height map.
     *
     * This class defines the height of any position in the map. It can then
     * be used to generate an height map sent to the GPU for rendering.
     *
     * The class is quite simple: it uses two modules, one for the land, when
     * the land test is positive, and one for the water. If the land test value
     * is inside a range [-shore_threshold,shore_threshold], it returns a linear
     * interpolation of the value of the water module and of the land module.
     * I recommend you to improve this class as well as the definition of the
     * modules to have better results, according to what you have in mind. You
     * can also add a shore module to better control the junction between land
     * and water.
     */
    class island_map_module
      : public noise::module::Module {
    public:
      island_map_module(
        const island_map_parameters& parameters,
        noise::module::Module& water,
        noise::module::Module& land )
        : noise::module::Module(0),
          m_params{ parameters },
          m_water{ water }, m_land{ land },
          m_land_definition{ parameters.m_land_definition }
      {}

      // To implement the noise::module::Module interface.
      int GetSourceModuleCount() const override
      {
        return 0;
      }

      /**@brief Get the height at a particular point.
       *
       * Get the height a point p(x,y,0).
       * @param x The x component of point p
       * @param y The y component of point p
       * @param z This value is not used and kept to be conform with the
       * noise::module::Module interface that defines "solid" noises.
       * @return The height at point p.
       */
      double GetValue( double x, double y, double z ) const
      {
        double land_factor = m_land_definition.land_value( x, y, 0 );
        if( land_factor > m_params.m_shore_threshold )
          return m_params.m_metrics.m_maximum_elevation * m_land.GetValue( x, y, 0 );
        else if( land_factor < -m_params.m_shore_threshold )
          return m_params.m_metrics.m_maximum_elevation* m_water.GetValue( x, y, 0 );

        double a = m_params.m_metrics.m_maximum_elevation * m_water.GetValue( x, y, 0 );
        double b = m_params.m_metrics.m_maximum_elevation * m_land.GetValue( x, y, 0 );
        double t = land_factor / (2.0 * m_params.m_shore_threshold ) + 0.5;
        return a * ( 1.0 - t ) + b * t;
      }

    private:
      island_map_parameters m_params;
      noise::module::Module& m_water;
      noise::module::Module& m_land;
      land_definition m_land_definition;
    };

    class simple_gl_window
      : public window {
    public:
      simple_gl_window( QQuickItem* parent = nullptr )
        : window{ parent }
      {
        initialize_renderer( new simple_gl_renderer );
        std::string shader_directory = tools::get_path_manager().get_resource_directory( "shaders" );
        shader_program_ptr island_program =
            std::make_shared<shader_program>( std::list<std::string>{
              shader_directory + "island.vert",
              shader_directory + "island.tcs",
              shader_directory + "island.tes",
              shader_directory + "island.frag"});

        // we create a new set of parameters, for a map of 5km, with a resolution of 50 cm
        // the mountains should be lower than 500 m
        // for any point with a land test value in [-0.05, 0.05], we consider this is a shore
        island_map_parameters params(5000);
        params.m_metrics.m_horizontal_resolution = 0.5;
        params.m_metrics.m_maximum_elevation = 500;
        params.m_land_definition.m_base_seed = std::chrono::system_clock::now().time_since_epoch().count();
        params.m_shore_threshold = 0.05;

        //
        // definition of the land module: keep in mind that you SHOULD improve it yourself :-}
        ////
        // a base perlin noise for the land
        noise::module::Perlin base_land_definition;
        base_land_definition.SetSeed( std::chrono::system_clock::now().time_since_epoch().count() );
        base_land_definition.SetFrequency( 2.0 / params.m_metrics.m_radius );
        base_land_definition.SetPersistence( 0.5 );
        base_land_definition.SetLacunarity( 2.0 );
        base_land_definition.SetOctaveCount( 10 );
        base_land_definition.SetNoiseQuality( noise::QUALITY_STD );

        // this module change the values of the base module such that the height is always >0
        noise::module::Curve land;
        land.SetSourceModule( 0, base_land_definition );
        land.AddControlPoint(-2.0000, 0.01 );
        land.AddControlPoint(-1.0000, 0.10 );
        land.AddControlPoint( 0.0000, 0.20 );
        land.AddControlPoint( 0.2500, 0.25 );
        land.AddControlPoint( 1.0000, 1.00 );
        land.AddControlPoint( 2.0000, 2.00 );

        // definition of the water module. Have a look at the possibilities of the libnoise
        // library, with its generators, modifiers, combiners and so on...
        ////
        noise::module::Const water;
        water.SetConstValue( 0.0 );

        island_map_module module ( params, water, land );

        island* map = new island{};
        map->set_radius( params.m_metrics.m_radius );
        map->set_heightmap( module, 4096 );
        map->set_resolution( params.m_metrics.m_horizontal_resolution );
        map->set_maximum_elevation( params.m_metrics.m_maximum_elevation );
        map->set_shader_program( island_program );
        add_renderable( map );
      }
    };
}}

int main( int argc, char* argv[] )
{
  // This is typically the place where you will analyze command-line arguments
  // such as to set a resources root directory.

  // Initialize the GUI application.
  QGuiApplication qgui( argc, argv );

  // Register C++ types to the QML engine: we would then be able to use those types in qml scripts.
  qmlRegisterType<graphics_origin::application::simple_gl_window>( "GraphicsOrigin", 1, 0, "GLWindow" );
  qmlRegisterType<graphics_origin::application::simple_camera   >( "GraphicsOrigin", 1, 0, "GLCamera" );

  // Load the main QML describing the main window into the simple QML application.
  std::string input_qml = graphics_origin::tools::get_path_manager().get_resource_directory( "qml" ) + "4_island_map.qml";
  graphics_origin::application::simple_qml_application app;
  app.setSource(QUrl::fromLocalFile( input_qml.c_str()));

  // This ensure that the application is running and visible, you do not have to worry about those 3 lines.
  app.show();
  app.raise();
  return qgui.exec();
}
