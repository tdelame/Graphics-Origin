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
# include "../../graphics-origin/application/balls_renderable.h"
# include "../../graphics-origin/tools/log.h"

# include "../../graphics-origin/geometry/ball.h"
# include "../../graphics-origin/geometry/bvh.h"

# include "../../graphics-origin/tools/tight_buffer_manager.h"
# include "3_simple_gl_application.h"

# include <omp.h>

# include <QGuiApplication>
# include <QtGui/QSurfaceFormat>
# include <QtCore/QThread>

# include <list>
# include <fstream>
# include <sstream>

namespace graphics_origin {
namespace application {

    simple_camera::simple_camera( QObject* parent )
      : camera{ parent }, m_direction{}, m_update_time{ omp_get_wtime() },
        m_forward{false}, m_left{false}, m_right{false},
        m_backward{false}, m_up{false}, m_down{false}
    {}

    void simple_camera::set_go_left( bool left )
    {
      m_left = left;
      if( m_left )
        m_direction.x = 1.0;
      else if( m_right )
        m_direction.x = -1.0;
      else m_direction.x = 0;
    }
    void simple_camera::set_go_right( bool right )
    {
      m_right = right;
      if( m_right )
        m_direction.x = -1.0;
      else if( m_left )
        m_direction.x = 1.0;
      else m_direction.x = 0;
    }
     void simple_camera::set_go_forward( bool forward )
    {
      m_forward = forward;
      if( m_forward )
        m_direction.z = 1.0;
      else if( m_backward )
        m_direction.z = -1.0;
      else m_direction.z = 0;
    }
    void simple_camera::set_go_backward( bool backward )
    {
      m_backward = backward;
      if( m_backward )
        m_direction.z = -1.0;
      else if( m_forward )
        m_direction.z = 1.0;
      else m_direction.z = 0;
    }

    void simple_camera::do_update()
    {
      if( m_forward || m_left || m_right || m_backward )
        {
          real elapsed_time = omp_get_wtime() - m_update_time;

          gpu_vec3 shift = glm::normalize(m_direction) * gpu_real(elapsed_time) * gpu_real(0.5);
          m_view[3][0] += shift.x;
          m_view[3][1] += shift.y;
          m_view[3][2] += shift.z;

          emit position_changed();
        }
      m_update_time = omp_get_wtime();
    }


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
    m_camera->update();
    for( auto& r : m_renderables )
      {
        r->get_shader_program()->bind();
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

  std::string get_next_line( std::ifstream& file, size_t& cline )
  {
    std::string result = "";
    do
      {
        if (file.eof() || file.fail())
          return "";
        getline(file, result);
        ++cline;

        std::string::size_type pos = 0;
        for( auto c : result )
          {
            if( c == ' ' || c == '\t' )
              ++pos;
            else break;
          }
        result = result.substr( pos );

      }
    while (result == "" || result[0] == '#');
    size_t found = result.find('#');
    if (found != std::string::npos)
      result = result.substr(0, found);
    return result;
  }

  class simple_gl_window
    : public gl_window {
  public:
    simple_gl_window( QQuickItem* parent = nullptr )
      : gl_window( parent )
    {
      initialize_renderer( new simple_gl_renderer );

      shader_program_ptr box_wireframe_program =
          std::make_shared<shader_program>( std::list<std::string>{
            "shaders/aabox.vert",
            "shaders/aabox.geom",
            "shaders/aabox.frag"});

      shader_program_ptr balls_program =
          std::make_shared<shader_program>( std::list<std::string>{
            "shaders/balls.vert",
            "shaders/balls.geom",
            "shaders/balls.frag"});

      shader_program_ptr flat_program =
          std::make_shared<shader_program>( std::list<std::string>{
            "shaders/flat.vert",
            "shaders/flat.frag"});


      size_t nb_balls = 0;
      size_t nline = 0;
      std::ifstream input( "tutorial/3_application/bumpy_torus.balls");

      {
        std::istringstream tokenizer( get_next_line( input, nline ) );
        tokenizer >> nb_balls;
      }
      auto brenderable = new balls_renderable( balls_program, nb_balls );
      for( size_t i = 0; i < nb_balls; ++ i )
        {
          std::string line_string = get_next_line( input, nline );
          std::istringstream tokenizer( line_string );
          vec3 c;
          real radius;
          tokenizer >> c.x >> c.y >> c.z >> radius;
          if( tokenizer.fail() )
            {
              LOG( error, "incorrect data at line " << nline << " [" << line_string << "]");
            }
          else
            {
              brenderable->add( geometry::ball( c, radius ));
            }
        }
      input.close();
//      geometry::box_bvh bvh( balls, nb_balls );

//      add_renderable( aaboxes_renderable_from_box_bvh( box_wireframe_program, bvh ) );
      add_renderable( brenderable );
    }




  };

  test_application::test_application( QWindow* parent )
    : QQuickView( parent )
  {
    qmlRegisterType<simple_gl_window>( "GraphicsOrigin", 1, 0, "GLWindow" );
    qmlRegisterType<simple_camera   >( "GraphicsOrigin", 1, 0, "GLCamera" );

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
