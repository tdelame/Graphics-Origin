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

# include "simple_terrain_generation/map.h"

# include <chrono>

# include <QGuiApplication>

# include <noise/noiseutils.h>
# include <noise/noise.h>

/**
 * This tutorial is about the simple terrain generation. I took a great pleasure
 * while building this tutorial, playing with parameters, building more and more
 * complex noises to obtain what I had in mind. I played so much that the
 * resulting code was a little too obscure for a tutorial. So, to build this
 * version, I removed every fancy thing, to make it really simple. You can improve
 * it the way you want, to suit your design intent. */

namespace graphics_origin {
  namespace application {

    struct map_metrics {
      double m_radius;
      double m_maximum_elevation;
      double m_horizontal_resolution;

      map_metrics( double radius )
        : m_radius{ radius },
          m_maximum_elevation{ 1000 },
          m_horizontal_resolution{ 1.0 }
      {}
    };

    class simple_gl_window
      : public window {
    public:
      simple_gl_window( QQuickItem* parent = nullptr )
        : window{ parent }
      {
        initialize_renderer( new simple_gl_renderer );
        std::string shader_directory = tools::get_path_manager().get_resource_directory( "shaders" );
        shader_program_ptr map_program =
            std::make_shared<shader_program>( std::list<std::string>{
              shader_directory + "simple_terrain.vert",
              shader_directory + "simple_terrain.tcs",
              shader_directory + "simple_terrain.tes",
              shader_directory + "simple_terrain.frag"});

        // we create a new set of parameters, for a map of 5km, with a resolution of 50 cm
        // the mountains should be lower than 500 m
        map_metrics params(5000);
        params.m_horizontal_resolution = 0.5;
        params.m_maximum_elevation = 500;

        // definition of the land module: keep in mind that you SHOULD improve it yourself :-}
        // to do so, have a look at the documentation of the libnoise library, it is really fascinating!
        // The parameters are chosen here to give something like an archipelago.
        //////
        // a base perlin noise for the land
        noise::module::Perlin base_land_definition;
        base_land_definition.SetSeed( std::chrono::system_clock::now().time_since_epoch().count() );
        base_land_definition.SetFrequency( 2.0 / params.m_radius );
        base_land_definition.SetPersistence( 0.5 );
        base_land_definition.SetLacunarity( 2.0 );
        base_land_definition.SetOctaveCount( 10 );
        base_land_definition.SetNoiseQuality( noise::QUALITY_STD );
        // scale the noise to match the expected maximum elevation
        noise::module::ScaleBias module;
        module.SetSourceModule( 0, base_land_definition );
        module.SetScale( params.m_maximum_elevation );

        simple_terrain_map* map = new simple_terrain_map{};
        map->set_radius( params.m_radius );
        map->set_heightmap( module, 4096 );
        map->set_resolution( params.m_horizontal_resolution );
        map->set_maximum_elevation( params.m_maximum_elevation );
        map->set_shader_program( map_program );
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
  std::string input_qml = graphics_origin::tools::get_path_manager().get_resource_directory( "qml" ) + "4_simple_terrain_generation.qml";
  graphics_origin::application::simple_qml_application app;
  app.setSource(QUrl::fromLocalFile( input_qml.c_str()));

  // This ensure that the application is running and visible, you do not have to worry about those 3 lines.
  app.show();
  app.raise();
  return qgui.exec();
}
