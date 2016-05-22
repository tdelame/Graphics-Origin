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

/**
 * Those 3 headers define a simple setup to create a 3D application:
 * - a camera, that can be controlled by a qml script
 * - a renderer, that simply iterate over a list of renderables
 * - a simple application that initialize QtQuick for us, and make sure that all windows are destroyed at exit
 *
 * If you want more features, you will have to rewrite those classes.
 */
# include "simple_camera.h"
# include "simple_gl_renderer.h"
# include "simple_qml_application.h"

# include <QGuiApplication>

namespace graphics_origin {
namespace application {

  /**
   * Subclass a gl_window to initialize a scene in the constructor. This way,
   * the initialization is pretty simple from the QML side: you just have to
   * declare an object of type GLWindow in the qml script. Also, this allows
   * a simple way to bind an implementation of a gl_window_renderer to the
   * window.
   */
  class simple_gl_window
    : public gl_window {
  public:
    simple_gl_window( QQuickItem* parent = nullptr )
      : gl_window( parent )
    {
      /**
       * Bind an implementation of a gl_window_renderer to the window. This
       * renderer will be executed in the GL thread to render whenever possible
       * the scene into a texture. QtQuick will then display this texture in a
       * window.
       */
      initialize_renderer( new simple_gl_renderer );

      /**
       * Get the resource paths from the path manager.
       */
      std::string shader_directory = tools::get_path_manager().get_resource_directory( "shaders" );
      std::string mesh_directory = tools::get_path_manager().get_resource_directory( "meshes" );

      /**
       * Initialize two shader program to render the scene composed of a mesh and its BVH.
       */
      shader_program_ptr box_wireframe_program =
          std::make_shared<shader_program>( std::list<std::string>{
            shader_directory + "aabox.vert",
            shader_directory + "aabox.geom",
            shader_directory + "aabox.frag"});

      shader_program_ptr mesh_program =
          std::make_shared<shader_program>( std::list<std::string>{
            shader_directory + "mesh.vert",
            shader_directory + "mesh_wireframe.geom",
            shader_directory + "mesh_wireframe.frag"});

      /**
       * Load a mesh from the mesh directory, the armadillo in this case.
       */
      auto mesh = new mesh_renderable( mesh_program );
      mesh->load( mesh_directory + "armadillo.off" );
      add_renderable( mesh ); // add the mesh to the scene.

      /**
       * Build a BVH of that mesh, with axis aligned bounding box for the bounding volume.
       * There is already a function that convert the produced BVH to a set of boxes to render.
       */
      geometry::mesh_spatial_optimization mso( mesh->get_geometry(), true, true );
      auto boxes_renderable = aaboxes_renderable_from_box_bvh( box_wireframe_program, *mso.get_bvh() );
      add_renderable( boxes_renderable ); // add the boxes to the scene.
    }
  };

}
}

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
  std::string input_qml = graphics_origin::tools::get_path_manager().get_resource_directory( "qml" ) + "3_simple_gl_application.qml";
  graphics_origin::application::simple_qml_application app;
  app.setSource(QUrl::fromLocalFile( input_qml.c_str()));

  // This ensure that the application is running and visible, you do not have to worry about those 3 lines.
  app.show();
  app.raise();
  return qgui.exec();
}
