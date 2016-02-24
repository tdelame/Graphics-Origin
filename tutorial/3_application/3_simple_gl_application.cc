/*  Created on: Feb 14, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */
# include <GL/glew.h>

# include "../../graphics-origin/application/camera.h"
# include "../../graphics-origin/application/gl_helper.h"
# include "../../graphics-origin/application/gl_window.h"
# include "../../graphics-origin/application/renderable.h"
# include "../../graphics-origin/application/shader_program.h"
# include "../../graphics-origin/application/aaboxes_renderable.h"
# include "../../graphics-origin/tools/log.h"

# include "../../graphics-origin/tools/tight_buffer_manager.h"
# include "3_simple_gl_application.h"

# include <QGuiApplication>
# include <QtGui/QSurfaceFormat>
# include <QtCore/QThread>

# include <list>

namespace graphics_origin {
namespace application {


  static gpu_vec3 positions[] = {
      gpu_vec3{ 0, -0.1, 0 },
      gpu_vec3{ 0,  0.1, 0 },
      gpu_vec3{ 0,  0, 1 }
  };

  static gpu_vec3 colors[] = {
      gpu_vec3{ 1, 0, 0 },
      gpu_vec3{ 0, 1, 0 },
      gpu_vec3{ 0, 0, 1 }
  };

  class triangle_renderable
    : public renderable {
  public:
    triangle_renderable( shader_program_ptr program )
      : m_buffer{ 0, 0 }
    {
      m_program = program;
      m_model = gpu_mat4(1.0);
    }

    ~triangle_renderable()
    {
      remove_gpu_data();
    }


  private:
    void update_gpu_data() override
    {
      if( !m_buffer[POSITIONS] )
        {
          glcheck(glGenBuffers( NB_BUFFERS, m_buffer ));
        }
      glcheck(glBindBuffer( GL_ARRAY_BUFFER, m_buffer[POSITIONS] ));
      glcheck(glBufferData( GL_ARRAY_BUFFER, 3 * sizeof(gpu_vec3), positions, GL_STATIC_DRAW ));

      glcheck(glBindBuffer( GL_ARRAY_BUFFER, m_buffer[COLORS] ));
      glcheck(glBufferData( GL_ARRAY_BUFFER, 3 * sizeof(gpu_vec3), colors, GL_STATIC_DRAW ));
    }

    void do_render() override
    {
      auto mvp = m_renderer->get_projection_matrix() * m_renderer->get_view_matrix() * m_model;
      int mvp_location = m_program->get_uniform_location("mvp");
      glcheck(glUniformMatrix4fv( mvp_location, 1, GL_FALSE, glm::value_ptr(mvp)));

      int position = m_program->get_attribute_location( "position");
      int color = m_program->get_attribute_location( "color");

      glcheck(glEnableVertexAttribArray( position ));
      glcheck(glEnableVertexAttribArray( color ));

      glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_buffer[POSITIONS]));
      glcheck(glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 0, 0 ));

      glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_buffer[COLORS]));
      glcheck(glVertexAttribPointer(color, 3, GL_FLOAT, GL_FALSE, 0, 0 ));

      glcheck(glDrawArrays(GL_TRIANGLES, 0, 3 ));

      glcheck(glDisableVertexAttribArray( position ));
      glcheck(glDisableVertexAttribArray( color ));
    }

    void remove_gpu_data() override
    {
      glcheck(glDeleteBuffers( NB_BUFFERS, m_buffer));
      m_buffer[ POSITIONS ] = (unsigned int)0;
    }

    enum{ POSITIONS, COLORS, NB_BUFFERS };
    unsigned int m_buffer[ NB_BUFFERS ];
  };


  simple_gl_renderer::~simple_gl_renderer()
  {}

  void simple_gl_renderer::do_add( renderable* r )
  {
    m_renderables.push_back( r );
  }

  void simple_gl_renderer::do_render()
  {
    for( auto& r : m_renderables )
      {
        r->get_shader_program()->bind();
//        program->bind();
//        int location = program->get_uniform_location( "view" );
//        if( location != shader_program::null_identifier )
//          glcheck(glUniformMatrix4fv( location, 1, GL_FALSE, glm::value_ptr(m_camera->get_view_matrix())));
//        location = program->get_uniform_location( "projection" );
//        if( location != shader_program::null_identifier )
//          glcheck(glUniformMatrix4fv( location, 1, GL_FALSE, glm::value_ptr(m_camera->get_projection_matrix())));
        r->render();
      }
  }

  void simple_gl_renderer::do_shut_down()
  {
    while( !m_renderables.empty() )
      {
        auto r = m_renderables.front();
        delete r;
        m_renderables.pop_front();
      }
  }

  class simple_gl_window
    : public gl_window {
  public:
    simple_gl_window( QQuickItem* parent = nullptr )
      : gl_window( parent )
    {
      initialize_renderer( new simple_gl_renderer );

      shader_program_ptr flat_program =
          std::make_shared<shader_program>( std::list<std::string>{
            "shaders/flat.vert",
            "shaders/flat.frag"});

      shader_program_ptr box_wireframe_program =
          std::make_shared<shader_program>( std::list<std::string>{
            "shaders/aabox.vert",
            "shaders/aabox.geom",
            "shaders/aabox.frag"});


      auto boxes = new aaboxes_renderable( box_wireframe_program, 4 );

      boxes->add( geometry::aabox( vec3{}, vec3{1, 1, 1 }));
      boxes->add( geometry::aabox( vec3{1,0,0}, vec3{2, 1, 1 }), gpu_vec3{1,0,0});
      boxes->add( geometry::aabox( vec3{0,1,0}, vec3{1, 2, 1 }), gpu_vec3{0,1,0});
      boxes->add( geometry::aabox( vec3{0,0,1}, vec3{1, 1, 2 }), gpu_vec3{0,0,1});

      add_renderable( new triangle_renderable( flat_program ) );
      add_renderable( boxes );
    }

  };

  test_application::test_application( QWindow* parent )
    : QQuickView( parent )
  {
    qmlRegisterType<simple_gl_window>( "GraphicsOrigin", 1, 0, "GLWindow" );
    qmlRegisterType<camera>( "GraphicsOrigin", 1, 0, "GLCamera" );

    QSurfaceFormat format;
    format.setMajorVersion( 4 );
    format.setMinorVersion( 4 );
    setFormat( format );
    setResizeMode(QQuickView::SizeRootObjectToView );
    // Rendering in a thread introduces a slightly more complicated cleanup
    // so we ensure that no cleanup of graphics resources happen until the
    // application is shutting down.
    setPersistentOpenGLContext(true);
    setPersistentSceneGraph(true);
  }


  test_application::~test_application()
  {
    // As the render threads make use of our QGuiApplication object
    // to clean up gracefully, wait for them to finish before
    // QGuiApp is taken off the heap.
    foreach(gl_window* w, gl_window::g_gl_windows)
      {
        w->pause();
        delete w;
      }
  }
}
}

int main( int argc, char* argv[] )
{
  QGuiApplication qgui( argc, argv );
  graphics_origin::application::test_application app;
  app.setSource(QUrl::fromLocalFile("tutorial/3_application/3_simple_gl_application.qml"));
  app.show();
  app.raise();
  return qgui.exec();
}
