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

namespace graphics_origin {
namespace application {

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

  static void print_node( uint32_t node_index,
                          const geometry::bvh<geometry::aabox>* bvh )
  {
    const auto& node = bvh->get_node( node_index );
    if( bvh->is_leaf( node_index ) )
      {
        std::cout <<"node #" << node_index << " is a leaf around primitive #" << node.element_index << "\n";
        std::cout <<"bounding box = {" << node.bounding.get_min() << " , " << node.bounding.get_max() << "} and father = " << node.parent_index << std::endl;
      }
    else
      {
        std::cout <<"node #" << node_index << " is internal with child node #" << node.left_index << " and node #" << node.right_index << " \n";
        std::cout <<"bounding box = {" << node.bounding.get_min() << " , " << node.bounding.get_max() << "} and father = " << node.parent_index << std::endl;
      }
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


      geometry::mesh mesh( "17674.ply");
      geometry::mesh_spatial_optimization mso( mesh, true, true );

//      LOG( error, "build mso and mesh");
      auto bvh = mso.get_bvh();
//
      for( size_t i = 0; i < bvh->get_number_of_internal_nodes() + bvh->get_number_of_leaf_nodes(); ++ i )
        {
          print_node( i, bvh );
        }
      std::cout << "\n\n\n" << std::endl;

      {
        std::cout << "BBOX = " << mso.get_bounding_box().get_min() << " , " << mso.get_bounding_box().get_max() << std::endl;
        print_node( 0, bvh );
        print_node( 131699, bvh );
        print_node( 131700, bvh );


        print_node( 1485, bvh );
        print_node( 11104, bvh );
        print_node( 11105, bvh );
      }

      auto boxes_renderable = new aaboxes_renderable( box_wireframe_program, bvh->get_number_of_leaf_nodes() );
      std::list< uint32_t > node_indices( 1, 0 );
      uint32_t size = 0;
      uint32_t seen = 0;
      while( ! node_indices.empty() )
        {
          auto idx = node_indices.front();
          node_indices.pop_front();
          ++seen;
//          LOG( error, "examining node #" << idx );
          if( !bvh->is_leaf( idx ) )
            {
              const auto& node = bvh->get_node( idx );
//              LOG( error, "not a leaf so pushing node #" << node.left_index << " and node #" << node.right_index << " ");
              node_indices.push_front( node.left_index );
              node_indices.push_front( node.right_index );
            }
          else
            {
              boxes_renderable->add( bvh->get_node( idx ).bounding, get_color( size, 0, bvh->get_number_of_internal_nodes() ) );
              ++size;
            }
          if( size >= bvh->get_number_of_leaf_nodes() + 1 || seen >= bvh->get_number_of_nodes() + 1 )
            {
              LOG( error, "ouin");
              break;

            }

        }
      add_renderable( boxes_renderable );



//      size_t nb_balls = 0;
//      balls_renderable* brenderable = nullptr;
//
//      {
//        size_t nline = 0;
//        std::ifstream input( "tutorial/3_application/bumpy_torus.balls");
//
//        {
//          std::istringstream tokenizer( get_next_line( input, nline ) );
//          tokenizer >> nb_balls;
//        }
//        brenderable = new balls_renderable( balls_program, nb_balls );
//        for( size_t i = 0; i < nb_balls; ++ i )
//          {
//            std::string line_string = get_next_line( input, nline );
//            std::istringstream tokenizer( line_string );
//            vec3 c;
//            real radius;
//            tokenizer >> c.x >> c.y >> c.z >> radius;
//            if( tokenizer.fail() )
//              {
//                LOG( error, "incorrect data at line " << nline << " [" << line_string << "]");
//              }
//            else
//              {
//                brenderable->add( geometry::ball( c, radius ) );
//              }
//          }
//        input.close();
//      }
//
//      add_renderable( brenderable );
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
