/*  Created on: May 22, 2016
 *      Author: T. Delame (tdelame@gmail.com) */

/**
 * The main differences between the files of this application and the one of
 * the tutorial 3_simple_gl_applications are:
 * - the definition of a transparent windows renderable, that we will use to
 * have some transparency in the scene (see transparency/transparent_windows_renderable.h)
 * - the definition of a window frames renderable to add a frame around the
 * transparent windows (see transparency/window_frames_renderable.h)
 * - the definition of a transparency_gl_renderer, which is basically the
 * simple_gl_renderer with another list for transparent windows renderables.
 * Those renderables are rendered last, after all opaque objects had been
 * rendered on the GPU.
 * - the definition of a transparency_gl_window, which only difference with
 * simple_gl_window is the initialization of a default scene in the
 * constructor.
 * - the definition of a camera that cannot be controlled by the user but
 * that will rotate around the scene (see transparency/rotating_camera.h)
 */
// to write window/draw code
# include "../../graphics-origin/application/camera.h"
# include "../../graphics-origin/application/gl_helper.h"
# include "../../graphics-origin/application/gl_window.h"
# include "../../graphics-origin/application/gl_window_renderer.h"
# include "../../graphics-origin/application/shader_program.h"
# include <GL/glew.h>
// to use renderables in the scene
# include "../../graphics-origin/application/textured_mesh_renderable.h"
# include "transparency/window_frames_renderable.h"
# include "transparency/transparent_windows_renderable.h"
// utilities
# include "../../graphics-origin/tools/resources.h"
# include "../../graphics-origin/tools/log.h"

namespace graphics_origin {
namespace application {

  class transparency_gl_renderer
    : public graphics_origin::application::gl_window_renderer {
  public:
    ~transparency_gl_renderer(){}

  private:
    void do_add( graphics_origin::application::renderable* r ) override
    {
      // check if the renderable is a transparent window
      transparent_windows_renderable* windows = dynamic_cast< transparent_windows_renderable* >( r );
      if( windows )
        {
          if( m_windows )
            {
              LOG( info, "cannot have more than one set of windows. Deleting previous one.");
              delete m_windows;
              m_windows = nullptr;
            }
          m_windows = windows;
        }
      // otherwise, add it to the list of opaque objects
      else m_renderables.push_back( r );
    }

    void do_render() override
    {
      m_camera->update();

      // render opaque objects, i.e. the central mesh and the window frames.
      for( auto& r : m_renderables )
        {
          r->get_shader_program()->bind();
          r->render();
        }

      // render transparent windows (if any)
      if( m_windows )
        {
          // Activate blending with the right interpolation function. [1]
          glcheck(glEnable(GL_BLEND));
          glcheck(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
          // bind the program and draw our windows
            m_windows->get_shader_program()->bind();
            m_windows->render();
          // remember to disable blending to not mess with other parts of the code [1]
          glcheck(glDisable(GL_BLEND));

          //[1] Typically, those lines should be included inside the do_render()
          // function of transparent_windows_renderable. I let them here to insist
          // on their importance in the main source file of the demo.
        }
    }

    void do_shut_down() override
    {
      while( !m_renderables.empty() )
        {
          auto r = m_renderables.front();
          delete r;
          m_renderables.pop_front();
        }
      if( m_windows )
        {
          delete m_windows;
          m_windows = nullptr;
        }
    }
    std::list< graphics_origin::application::renderable* > m_renderables;
    graphics_origin::application::transparent_windows_renderable* m_windows;
  };

  class simple_gl_window
    : public gl_window {
  public:
    simple_gl_window( QQuickItem* parent = nullptr )
      : gl_window( parent )
    {
      initialize_renderer( new transparency_gl_renderer );
      std::string texture_directory = tools::get_path_manager().get_resource_directory( "textures" );
      std::string shader_directory = tools::get_path_manager().get_resource_directory( "shaders" );
      std::string mesh_directory = tools::get_path_manager().get_resource_directory( "meshes" );

      shader_program_ptr mesh_program =
          std::make_shared<shader_program>( std::list<std::string>{
            shader_directory + "textured_mesh.vert",
            shader_directory + "textured_mesh.frag"});

      shader_program_ptr transparent_program =
          std::make_shared<shader_program>( std::list<std::string>{
            shader_directory + "transparent_window.vert",
            shader_directory + "transparent_window.geom",
            shader_directory + "transparent_window.frag"
      });

      shader_program_ptr frame_program =
          std::make_shared<shader_program>( std::list<std::string>{
            shader_directory + "window_frame.vert",
            shader_directory + "window_frame.geom",
            shader_directory + "window_frame.frag"
      });


      auto mesh = new textured_mesh_renderable( mesh_program );
      mesh->load_mesh( mesh_directory + "Bunny.obj" );
      mesh->load_texture( texture_directory + "TexturedBunny.png" );
      mesh->set_model_matrix( glm::rotate( gpu_real{M_PI_2}, gpu_vec3{0,0,1}) * glm::rotate( gpu_real{M_PI_2}, gpu_vec3{1,0,0}));
      add_renderable( mesh );


      const unsigned int angle_divisions = 16;
      const gpu_real angle_step = 2.0 * M_PI / gpu_real{16};
      gpu_real angle = 0;

      auto windows = new transparent_windows_renderable( transparent_program, angle_divisions );
      auto frames = new window_frames_renderable( frame_program, angle_divisions );

      for( unsigned int i = 0; i < angle_divisions; ++ i, angle += angle_step )
        {
          auto rotation = glm::rotate( angle, gpu_vec3{0,0,1} );
          auto position = gpu_vec3{rotation * gpu_vec4{2,0,0,1.0}};
          auto v1 = gpu_vec3{rotation * gpu_vec4{0,0.4,-0.3,0}};
          auto v2 = gpu_vec3{rotation * gpu_vec4{0,-0.4,-0.3,0}};
          // add a transparent window with a color based on its position
          windows->add(
            position, v1, v2,
            gpu_vec4{0.5 + position.x / 4.0, 0.5 + position.y / 4.0, 0.5 + position.z / 4.0, 0.2 + gpu_real(i) / gpu_real(angle_divisions)*0.4} );
          // add a frame around that window with a dimension that depends on its position
          frames->add(
            position, v1, v2, 0.01 + 0.02 * gpu_real(i)/gpu_real(angle_divisions), 0.02 );
        }

      add_renderable( windows );
      add_renderable( frames );
    }
  };

}}

// launching our Qt/QtQuick + OpenGL application
# include "common/simple_qml_application.h"
# include "transparency/rotating_camera.h"
# include <QGuiApplication>

int main( int argc, char* argv[] )
{
  // This is typically the place where you will analyze command-line arguments
  // such as to set a resources root directory.

  // Initialize the GUI application.
  QGuiApplication qgui( argc, argv );

  // Register C++ types to the QML engine: we would then be able to use those types in qml scripts.
  qmlRegisterType<graphics_origin::application::simple_gl_window>( "GraphicsOrigin", 1, 0, "GLWindow" );
  qmlRegisterType<graphics_origin::application::rotating_camera   >( "GraphicsOrigin", 1, 0, "GLCamera" );

  // Load the main QML describing the main window into the simple QML application.
  std::string input_qml = graphics_origin::tools::get_path_manager().get_resource_directory( "qml" ) + "4_transparency.qml";
  graphics_origin::application::simple_qml_application app;
  app.setSource(QUrl::fromLocalFile( input_qml.c_str()));

  // This ensure that the application is running and visible, you do not have to worry about those 3 lines.
  app.show();
  app.raise();
  return qgui.exec();
}
