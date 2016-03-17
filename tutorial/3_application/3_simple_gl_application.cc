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
# include "../../graphics-origin/application/points_renderable.h"
# include "../../graphics-origin/application/mesh_renderable.h"
# include "../../graphics-origin/application/lines_renderable.h"
# include "../../graphics-origin/application/balls_renderable.h"
# include "../../graphics-origin/tools/log.h"

# include "../../graphics-origin/geometry/ball.h"
# include "../../graphics-origin/geometry/bvh.h"
# include "../../graphics-origin/geometry/ray.h"

# include "../../graphics-origin/tools/tight_buffer_manager.h"
# include "3_simple_gl_application.h"

# include <omp.h>

# include <QGuiApplication>
# include <QtGui/QSurfaceFormat>
# include <QtCore/QThread>

# include <list>
# include <fstream>
# include <sstream>


# include <chrono>
# include <random>


namespace graphics_origin {
namespace application {

real unit_random()
{
  //fixme:
//  static std::default_random_engine generator( 7 );
  static std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count() );
  static std::uniform_real_distribution<real> distribution( real(0), real(1));
  return distribution(generator);
}



    simple_camera::simple_camera( QObject* parent )
      : camera{ parent }, m_direction{},
        m_update_time{ omp_get_wtime() },
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
          gpu_real factor = glm::length( m_direction );
          if( factor > 0.01 )
            {
              factor = gpu_real((omp_get_wtime() - m_update_time)* 0.5 ) / factor;
              gpu_vec3 shift = factor * m_direction;
              if( std::isnan( shift.x ) || std::isnan( shift.y ) || std::isnan( shift.z ) )
                {
                  LOG( error, "SHIFT IS NAN = " << shift );
                }
              m_view[3][0] += shift.x;
              m_view[3][1] += shift.y;
              m_view[3][2] += shift.z;

              emit position_changed();
            }
        }
      m_update_time = omp_get_wtime();
    }

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

      shader_program_ptr mesh_program =
          std::make_shared<shader_program>( std::list<std::string>{
            "shaders/mesh.vert",
            "shaders/mesh.geom",
            "shaders/mesh.frag"});


//      size_t nb_balls = 0;
//      size_t nline = 0;
//      std::ifstream input( "tutorial/3_application/bumpy_torus.balls");
//
//      {
//        std::istringstream tokenizer( get_next_line( input, nline ) );
//        tokenizer >> nb_balls;
//      }
//      auto brenderable = new balls_renderable( balls_program, nb_balls );
//      std::vector< geometry::ball> balls( nb_balls );
//      for( size_t i = 0; i < nb_balls; ++ i )
//        {
//          std::string line_string = get_next_line( input, nline );
//          std::istringstream tokenizer( line_string );
//          vec3 c;
//          real radius;
//          tokenizer >> c.x >> c.y >> c.z >> radius;
//          if( tokenizer.fail() )
//            {
//              LOG( error, "incorrect data at line " << nline << " [" << line_string << "]");
//            }
//          else
//            {
//              balls[ i ] = geometry::ball( c, radius );
//              brenderable->add( geometry::ball( c, radius ));
//            }
//        }
//      input.close();
//      geometry::bvh<geometry::aabox> bvh( balls.data(), nb_balls );
//
//      add_renderable( aaboxes_renderable_from_box_bvh( box_wireframe_program, bvh ) );
//      add_renderable( brenderable );


      auto mesh = new mesh_renderable( mesh_program );
      mesh->load( "tutorial/3_application/dinopet.off");
      add_renderable( mesh );
      geometry::mesh_spatial_optimization msp( mesh->get_geometry() );

//      auto bbox = new aaboxes_renderable( box_wireframe_program, 1 );
      geometry::aabox box;
      mesh->get_geometry().compute_bounding_box( box );
//      bbox->add( box, gpu_vec3{1,0,0});
//      add_renderable( aaboxes_renderable_from_box_bvh( box_wireframe_program, *msp.get_bvh()));



      const size_t npoints = 1;
      std::vector< std::pair< vec3, bool> > points_data( npoints, std::make_pair( vec3{}, false ) );

//      # pragma omp parallel for
      for( size_t i = 0; i < npoints; ++ i )
        {
          vec3 p = vec3{ unit_random() - 0.5, unit_random() - 0.5, unit_random() - 0.5} * 2.0 * box.m_hsides + box.m_center;
          points_data[ i ] = std::make_pair( p, msp.contain( p ) );
        }

      auto points = new points_renderable( flat_program, npoints );
      auto lines = new lines_renderable( flat_program, npoints );
      for( auto& pair : points_data )
        {
          points->add( pair.first, pair.second ? gpu_vec3{0,1,0} : gpu_vec3{0,0,1} );

          size_t vi = 0;
          real distance = 0;
          msp.get_closest_vertex( pair.first, vi, distance );

          geometry::mesh::FaceVertexIter fviter = mesh->get_geometry().fv_begin( *mesh->get_geometry().vf_begin( geometry::mesh::VertexHandle( vi ) ) );

          auto target = mesh->get_geometry().point( *fviter ); ++ fviter;
          target += mesh->get_geometry().point( *fviter ); ++ fviter;
          target += mesh->get_geometry().point( *fviter );
          target *= real( 1.0 / 3.0 );


          vec3 direction = vec3{ target[0] - pair.first.x,
                    target[1] - pair.first.y,
                    target[2] - pair.first.z };
          distance = glm::length( direction );
          direction *= real(1.0) / distance;
          distance *= 1.1;

          if( msp.intersect( geometry::ray( pair.first, direction ), distance ) )
            {
              LOG( info, "youhou");
              lines->add( pair.first, gpu_vec3{1,1,0}, pair.first + distance * direction,
                          gpu_vec3{1,1,0});
            }
          else
            lines->add( pair.first, gpu_vec3{1,0,0},
                        vec3{target[0], target[1], target[2] }, gpu_vec3{1,0,0});
        }
      add_renderable( points );
      add_renderable( lines );
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
