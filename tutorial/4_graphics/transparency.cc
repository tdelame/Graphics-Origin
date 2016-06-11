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
# include "../../graphics-origin/application/textured_mesh_renderable.h"
# include "../../graphics-origin/tools/resources.h"
# include "../../graphics-origin/tools/log.h"
# include "../../graphics-origin/tools/tight_buffer_manager.h"
# include <GL/glew.h>

# include "common/simple_camera.h"
# include "common/simple_qml_application.h"

# include <QGuiApplication>


namespace graphics_origin {
namespace application {

  /**@brief A collection of transparent windows to render.
   *
   * A transparent window is a quad that let the light go through it. As such,
   * we can see any opaque object that lies behind a transparent window. We show
   * in this tutorial how to do it.
   *
   * First, a window is defined by its center, and two vectors that goes from
   * the center to two consecutive corners of that window. Those two vectors
   * are named v1 and v2, and should not be colinear (otherwise, you have a
   * zero-sized window by definition). Then, a window has a RGBA color.
   *
   * It should be noted that the current code can handle any orientation of the
   * corners. But if face culling is activated, some holes can appear depending
   * on the view point.
   *
   * A window has no thickness, but you can either add another parallel
   * window or modify the code to render a box instead of a quad.
   *
   * This code has a limitation related to the difficulty of performing transparency:
   * put the camera between the mesh and the windows, and look at the windows.
   * You will see that depending on the viewing angle, the transparency is not
   * ok. This is due to the simple depth sorting made here: we assume that all
   * quad fragments have the same depth as the center. There exist more evolved
   * methods to address this issue.
   * TODO: make a simple implementation of a complex method.
   */
  class transparent_windows_renderable
    : public graphics_origin::application::renderable {

    struct storage {
      gpu_vec3 center;
      gpu_vec3 v1;
      gpu_vec3 v2;
      gpu_vec4 color;
      gpu_real depth;

      storage( const storage& other )
         : center{ other.center }, v1{ other.v1 }, v2{ other.v2 },
           color{ other.color }, depth{ other.depth }
       {}

      storage& operator=( const storage& other )
      {
        center = other.center;
        v1 = other.v1;
        v2 = other.v2;
        color = other.color;
        depth = other.depth;
        return *this;
      }

      storage& operator=( storage&& other )
      {
        center = other.center;
        v1 = other.v1;
        v2 = other.v2;
        color = other.color;
        depth = other.depth;
        return *this;
      }
      storage()
        : depth{0}
      {}
    };

    typedef tools::tight_buffer_manager<
        storage,
        uint32_t,
        22 > windows_buffer;

    struct storage_depth_computation {
      storage_depth_computation( const camera* camera )
        : eye{ camera->get_position() }, forward{ camera->get_forward() }
      {}
      void operator()( storage& s ) const
      {
        s.depth = dot( forward, s.center - eye );
      }
      const gpu_vec3 eye;
      const gpu_vec3 forward;
    };

    struct storage_depth_ordering {
      bool operator()( const storage& a, const storage& b ) const
      {
        return a.depth > b.depth;
      }
    };

  public:
    typedef windows_buffer::handle handle;

    /**@brief Create a new collection of transparent windows.
     *
     * Build an instance of a transparent windows renderable.
     * @param program The shader program used to render a window. This shader
     * should have the following attributes:
     * - center (vec3) for the center of a window
     * - v1 (vec3) to go from the center to a corner
     * - v2 (vec3) to go from the center to a consecutive corner
     * - color (vec4) for the color of the window.
     * The shader should also have a uniform named vp (mat4) to receive the
     * product between the projection matrix and the view matrix.
     * @param expected_number_of_windows A guess about the final number of windows
     * stored in the instance. A correct guess avoids resizing internal buffer.
     */
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

    /**@brief Add another transparent window to render.
     *
     * Add one more transparent window to this instance.
     * @param center Center of the window.
     * @param v1 Vector that goes from the center to one corner of the window.
     * @param v2 Vector that goes from the center to the next corner of the window.
     * @param color Color of the window.
     * @return Handle to the newly created window.
     */
    handle add(
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

    /**@brief Get an existing window.
     *
     * Access to a window that was previously created by add(). If you modify
     * the data of a window, be sure to notify
     * @param h Handle of the existing window.
     * @return The window pointed by the handle h.
     */
    storage& get( handle h )
    {
      return m_windows.get( h );
    }

    void sort()
    {
      m_windows.process( storage_depth_computation{m_renderer->get_camera()} );
      m_windows.sort( storage_depth_ordering{} );
      glcheck(glBindBuffer( GL_ARRAY_BUFFER, m_vbos[ windows_vbo_id] ));
      glcheck(glBufferData( GL_ARRAY_BUFFER, sizeof(storage) * m_windows.get_size(), m_windows.data(), GL_DYNAMIC_DRAW));
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
        glcheck(glBufferData( GL_ARRAY_BUFFER, sizeof(storage) * m_windows.get_size(), m_windows.data(), GL_DYNAMIC_DRAW));

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
      sort();
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
          if( m_windows )
            {
              LOG( info, "cannot have more than one set of windows. Deleting previous one.");
              delete m_windows;
              m_windows = nullptr;
            }
          m_windows = windows;
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

      if( m_windows )
        {
          glcheck(glEnable(GL_BLEND));
          glcheck(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
            m_windows->get_shader_program()->bind();
            m_windows->render();
          glcheck(glDisable(GL_BLEND));
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

      auto mesh = new textured_mesh_renderable( mesh_program );
      mesh->load_mesh( mesh_directory + "spot_triangulated.obj" );
      mesh->load_texture( texture_directory + "spot_texture.png" );
      mesh->set_model_matrix( glm::rotate( -gpu_real{M_PI_2}, gpu_vec3{0,0,1}) * glm::rotate( gpu_real{M_PI_2}, gpu_vec3{1,0,0}));
      add_renderable( mesh );

      auto windows = new transparent_windows_renderable( transparent_program );
      windows->add( gpu_vec3{2,0,0}, gpu_vec3{0,0.3,-0.3}, gpu_vec3{0,-0.3,-0.3}, gpu_vec4( 0.2, 0.8, 0.4, 0.9 ) );

      windows->add( gpu_vec3{1.9,1,0}, gpu_vec3{0,1,-1}, gpu_vec3{0,-1,-1}, gpu_vec4( 0.2, 0.4, 0.8, 0.68 ) );
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
  std::string input_qml = graphics_origin::tools::get_path_manager().get_resource_directory( "qml" ) + "4_transparency.qml";
  graphics_origin::application::simple_qml_application app;
  app.setSource(QUrl::fromLocalFile( input_qml.c_str()));

  // This ensure that the application is running and visible, you do not have to worry about those 3 lines.
  app.show();
  app.raise();
  return qgui.exec();
}
