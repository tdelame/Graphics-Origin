/*  Created on: Feb 14, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */
# include "../../graphics-origin/application/gl_helper.h"
# include "../../graphics-origin/application/gl_window.h"
# include "../../graphics-origin/application/renderable.h"
# include "../../graphics-origin/application/shader_program.h"
# include "../../graphics-origin/application/aaboxes_renderable.h"
# include "../../graphics-origin/application/mesh_renderable.h"
# include "../../graphics-origin/tools/random.h"
# include "../../graphics-origin/tools/resources.h"
# include "../../graphics-origin/geometry/bvh.h"

# include "simple_camera.h"
# include "simple_gl_renderer.h"
# include "simple_qml_application.h"

# include <QGuiApplication>

namespace graphics_origin {
namespace application {

  class simple_gl_window
    : public gl_window {
  public:
    simple_gl_window( QQuickItem* parent = nullptr )
      : gl_window( parent )
    {
      initialize_renderer( new simple_gl_renderer );

      std::string shader_directory = tools::get_path_manager().get_resource_directory( "shaders" );
      std::string mesh_directory = tools::get_path_manager().get_resource_directory( "meshes" );

      shader_program_ptr box_wireframe_program =
          std::make_shared<shader_program>( std::list<std::string>{
            shader_directory + "aabox.vert",
            shader_directory + "aabox.geom",
            shader_directory + "aabox.frag"});

      shader_program_ptr mesh_program =
          std::make_shared<shader_program>( std::list<std::string>{
            shader_directory + "mesh.vert",
            shader_directory + "mesh.geom",
            shader_directory + "mesh.frag"});

      auto mesh = new mesh_renderable( mesh_program );
      mesh->load( mesh_directory + "armadillo.off" );
      geometry::mesh_spatial_optimization mso( mesh->get_geometry(), true, true );
      add_renderable( mesh );
      auto boxes_renderable = aaboxes_renderable_from_box_bvh( box_wireframe_program, *mso.get_bvh() );
      add_renderable( boxes_renderable );
    }
  };

}
}

int main( int argc, char* argv[] )
{
  // This is typically the place where you will analyze command-line arguments
  // such as to set a resources root directory.

  QGuiApplication qgui( argc, argv );
  qmlRegisterType<graphics_origin::application::simple_gl_window>( "GraphicsOrigin", 1, 0, "GLWindow" );
  qmlRegisterType<graphics_origin::application::simple_camera   >( "GraphicsOrigin", 1, 0, "GLCamera" );

  std::string input_qml = graphics_origin::tools::get_path_manager().get_resource_directory( "qml" ) + "3_simple_gl_application.qml";
  graphics_origin::application::simple_qml_application app;
  app.setSource(QUrl::fromLocalFile( input_qml.c_str()));

  app.show();
  app.raise();
  return qgui.exec();
}
