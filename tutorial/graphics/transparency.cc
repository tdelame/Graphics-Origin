/*  Created on: May 22, 2016
 *      Author: T. Delame (tdelame@gmail.com) */

/**
 * The main differences between the files of this application and the one of
 * the tutorial 3_simple_gl_applications are:
 * - the definition of a transparent windows renderable, that we will use to
 * have some transparency in the scene
 * - the definition of a transparency_gl_renderer, which is basically the
 * simple_gl_renderer with another list for transparent windows renderables.
 * Those renderables are rendered last, after all opaque objects had been
 * rendered on the GPU.
 * - the definition of a transparency_gl_window, which only difference with
 * simple_gl_window is the initialization of a default scene in the
 * constructor.
 */

# include "../../graphics-origin/application/gl_helper.h"
# include "../../graphics-origin/application/gl_window.h"
# include "../../graphics-origin/application/renderable.h"
# include "../../graphics-origin/application/shader_program.h"
# include "../../graphics-origin/application/mesh_renderable.h"
# include "../../graphics-origin/tools/resources.h"
# include "../../graphics-origin/tools/tight_buffer_manager.h"
# include <GL/glew.h>

# include "simple_camera.h"
# include "simple_qml_application.h"

# include <QGuiApplication>




namespace graphics_origin {
namespace application {

  class transparent_windows_renderable
    : public graphics_origin::application::renderable {

    struct storage {
      gpu_vec3 center;
      gpu_vec3 v1;
      gpu_vec3 v2;
      gpu_vec4 color;

      storage& operator=( storage&& other )
      {
        center = other.center;
        v1 = other.v1;
        v2 = other.v2;
        color = other.color;
        return *this;
      }
      storage()
      {}
    };

    typedef tools::tight_buffer_manager<
        storage,
        uint32_t,
        22 > windows_buffer;

  public:
    transparent_windows_renderable(
        shader_program_ptr program,
        size_t expected_number_of_windows = 0 )
      : m_windows{ expected_number_of_windows },
        m_vao{0}, m_vbos{ 0 }
    {
      m_model = gpu_mat4(1.0);
      m_program = program;
    }
    ~transparent_windows_renderable()
    {
      remove_gpu_data();
    }

    windows_buffer::handle add(
        const gpu_vec3& center,
        const gpu_vec3& v1,
        const gpu_vec3& v2,
        const gpu_vec4& color )
    {
      m_dirty = true;
      auto pair = m_windows.create();
      pair.second.center = center;
      pair.second.v1 = v1;
      pair.second.v2 = v2;
      pair.second.color = color;
      return pair.first;
    }

  private:
    void update_gpu_data() override
    {
      if( !m_vao )
        {
          glcheck(glGenVertexArrays( 1, &m_vao ));
          glcheck(glGenBuffers( number_of_vbos, m_vbos ));
        }

      int center_location = m_program->get_attribute_location( "center" );
      int v1_location = m_program->get_attribute_location( "v1" );
      int v2_location = m_program->get_attribute_location( "v2" );
      int color_location = m_program->get_attribute_location( "color" );

      glcheck(glBindVertexArray( m_vao ));
        glcheck(glBindBuffer( GL_ARRAY_BUFFER, m_vbos[ windows_vbo_id] ));
        glcheck(glBufferData( GL_ARRAY_BUFFER, sizeof(storage) * m_windows.get_size(), m_windows.data(), GL_STATIC_DRAW));

        glcheck(glEnableVertexAttribArray( center_location ));
        glcheck(glVertexAttribPointer( center_location,        // format of center:
          3, GL_FLOAT, GL_FALSE,                               // 3 unnormalized floats
          sizeof(storage),                                     // each attribute has the size of storage
          reinterpret_cast<void*>(offsetof(storage,center)))); // offset of the center inside an attribute

        glcheck(glEnableVertexAttribArray( v1_location ));
        glcheck(glVertexAttribPointer( v1_location,            // format of v1:
          3, GL_FLOAT, GL_FALSE,                               // 3 unnormalized floats
          sizeof(storage),                                     // each attribute has the size of storage
          reinterpret_cast<void*>(offsetof(storage,v1))));     // offset of the v1 inside an attribute

        glcheck(glEnableVertexAttribArray( v2_location ));
        glcheck(glVertexAttribPointer( v2_location,            // format of v1:
          3, GL_FLOAT, GL_FALSE,                               // 3 unnormalized floats
          sizeof(storage),                                     // each attribute has the size of storage
          reinterpret_cast<void*>(offsetof(storage,v2))));     // offset of the v1 inside an attribute

        glcheck(glEnableVertexAttribArray( color_location ));
        glcheck(glVertexAttribPointer( color_location,        // format of color:
          4, GL_FLOAT, GL_FALSE,                              // 4 unnormalized floats
          sizeof(storage),                                    // each attribute has the size of storage
          reinterpret_cast<void*>(offsetof(storage,color)))); // offset of the color inside an attribute

      glcheck(glBindVertexArray( 0 ));
    }

    void do_render() override
    {
      gpu_mat4 vp = m_renderer->get_projection_matrix() * m_renderer->get_view_matrix();
      glcheck(glUniformMatrix4fv( m_program->get_uniform_location( "vp"), 1, GL_FALSE, glm::value_ptr(vp)));
      glcheck(glBindVertexArray( m_vao ));
      glcheck(glDrawArrays( GL_POINTS, 0, m_windows.get_size()));
      glcheck(glBindVertexArray( 0 ) );
    }

    void remove_gpu_data() override
    {
      if( m_vao )
        {
          glcheck(glDeleteBuffers( number_of_vbos, m_vbos ));
          glcheck(glDeleteVertexArrays( 1, &m_vao ));
          m_vao = (unsigned int) 0;
          for( unsigned int i = 0; i < number_of_vbos; ++ i )
            m_vbos[ i ] = (unsigned int)0;
        }
    }

    windows_buffer m_windows;
    enum{ windows_vbo_id, number_of_vbos };
    unsigned int m_vao;
    unsigned int m_vbos[ number_of_vbos];
  };


  class transparency_gl_renderer
    : public graphics_origin::application::gl_window_renderer {
  public:
    ~transparency_gl_renderer(){}

  private:
    void do_add( graphics_origin::application::renderable* r ) override
    {
      transparent_windows_renderable* windows = dynamic_cast< transparent_windows_renderable* >( r );
      if( windows )
        {
          m_windows.push_back( windows );
        }
      else m_renderables.push_back( r );
    }

    void do_render() override
    {
      m_camera->update();
      for( auto& r : m_renderables )
        {
          r->get_shader_program()->bind();
          r->render();
        }

      glcheck(glEnable(GL_BLEND));
      glcheck(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

      for( auto& r : m_windows )
        {
          r->get_shader_program()->bind();
          r->render();
        }
      glcheck(glDisable(GL_BLEND));
    }

    void do_shut_down() override
    {
      while( !m_renderables.empty() )
        {
          auto r = m_renderables.front();
          delete r;
          m_renderables.pop_front();
        }
      while( !m_windows.empty() )
        {
          auto r = m_windows.front();
          delete r;
          m_windows.pop_front();
        }
    }

    std::list< graphics_origin::application::renderable* > m_renderables;
    std::list< graphics_origin::application::transparent_windows_renderable* > m_windows;
  };

  class simple_gl_window
    : public gl_window {
  public:
    simple_gl_window( QQuickItem* parent = nullptr )
      : gl_window( parent )
    {
      initialize_renderer( new transparency_gl_renderer );
      std::string shader_directory = tools::get_path_manager().get_resource_directory( "shaders" );
      std::string mesh_directory = tools::get_path_manager().get_resource_directory( "meshes" );

      shader_program_ptr mesh_program =
          std::make_shared<shader_program>( std::list<std::string>{
            shader_directory + "mesh.vert",
            shader_directory + "mesh.frag"});

      shader_program_ptr transparent_program =
          std::make_shared<shader_program>( std::list<std::string>{
            shader_directory + "transparent_window.vert",
            shader_directory + "transparent_window.geom",
            shader_directory + "transparent_window.frag"
      });

      auto mesh = new mesh_renderable( mesh_program );
      mesh->load( mesh_directory + "armadillo.off" );
      mesh->set_model_matrix( glm::rotate( -gpu_real{M_PI_2}, gpu_vec3{0,0,1}) * glm::rotate( gpu_real{M_PI_2}, gpu_vec3{1,0,0}));
      add_renderable( mesh );

      auto windows = new transparent_windows_renderable( transparent_program );
      windows->add( gpu_vec3{2,0,0}, gpu_vec3{0,0.3,-0.3}, gpu_vec3{0,-0.3,-0.3}, gpu_vec4( 0.2, 0.8, 0.4, 0.9 ) );
      add_renderable( windows );
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
  std::string input_qml = graphics_origin::tools::get_path_manager().get_resource_directory( "qml" ) + "graphics_transparency.qml";
  graphics_origin::application::simple_qml_application app;
  app.setSource(QUrl::fromLocalFile( input_qml.c_str()));

  // This ensure that the application is running and visible, you do not have to worry about those 3 lines.
  app.show();
  app.raise();
  return qgui.exec();
}
