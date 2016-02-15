/*  Created on: Feb 14, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */
# include <GL/glew.h>

# include "../../graphics-origin/application/camera.h"
# include "../../graphics-origin/application/gl_helper.h"
# include "../../graphics-origin/application/gl_window.h"
# include "3_simple_gl_application.h"

# include <QGuiApplication>
# include <QtGui/QSurfaceFormat>
# include <QtCore/QThread>

# include <list>

namespace graphics_origin {
namespace application {

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
        auto program = r->get_shader_program();
        glcheck(glUniformMatrix4fv(
            program->get_uniform_location( "view" ), 1,
            GL_FALSE, glm::value_ptr(m_camera->get_view_matrix())));
        glcheck(glUniformMatrix4fv(
            program->get_uniform_location( "projection" ), 1,
            GL_FALSE, glm::value_ptr(m_camera->get_projection_matrix())));
        program->bind();
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
      m_renderer = new simple_gl_renderer;
    }

  };

  test_application::test_application( QWindow* parent )
    : QQuickView( parent )
  {
    qmlRegisterType<simple_gl_renderer>( "GraphicsOrigin", 1, 0, "GLRenderer" );
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
  app.resize( 800, 800 );
  app.show();
  app.raise();
  return qgui.exec();
}
