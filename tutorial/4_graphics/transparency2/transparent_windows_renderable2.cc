/* Created on: Jun 13, 2016
 *     Author: T.Delame (tdelame@gmail.com)
 */
# include "transparent_windows_renderable2.h"
# include "../../../graphics-origin/application/camera.h"
# include "../../../graphics-origin/application/qt_application/renderer.h"
# include "../../../graphics-origin/application/gl_helper.h"
# include <GL/glew.h>

namespace graphics_origin {
  namespace application {
    transparent_windows_renderable2::storage::storage( const storage& other )
       : center{ other.center }, v1{ other.v1 }, v2{ other.v2 },
         color{ other.color }, depth{ other.depth }
     {}

   transparent_windows_renderable2::storage&
   transparent_windows_renderable2::storage::operator=( const storage& other )
    {
      center = other.center;
      v1 = other.v1;
      v2 = other.v2;
      color = other.color;
      depth = other.depth;
      return *this;
    }

   transparent_windows_renderable2::storage&
   transparent_windows_renderable2::storage::operator=( storage&& other )
  {
    center = other.center;
    v1 = other.v1;
    v2 = other.v2;
    color = other.color;
    depth = other.depth;
    return *this;
  }

   transparent_windows_renderable2::storage::storage()
    : depth{0}
  {}

  transparent_windows_renderable2::storage_depth_computation::storage_depth_computation( const gpu_mat4& view )
    : eye{ -gpu_vec3( view[3] ) * gpu_mat3( view ) }, forward{ gpu_vec3{ -view[0][2], -view[1][2], -view[2][2] } }
  {}

  void transparent_windows_renderable2::storage_depth_computation::operator()( storage& s ) const
  {
    s.depth = dot( forward, s.center - eye );
  }

  bool
  transparent_windows_renderable2::storage_depth_ordering::operator()( const storage& a, const storage& b ) const
  {
    return a.depth > b.depth;
  }
  transparent_windows_renderable2::transparent_windows_renderable2(
      shader_program_ptr program,
      size_t expected_number_of_windows )
    : m_windows{ expected_number_of_windows },
      m_vao{0}, m_vbos{ 0 }
  {
    m_model = gpu_mat4(1.0);
    m_program = program;
  }
  transparent_windows_renderable2::~transparent_windows_renderable2()
  {
    remove_gpu_data();
  }

  transparent_windows_renderable2::handle
  transparent_windows_renderable2::add(
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

  transparent_windows_renderable2::storage&
  transparent_windows_renderable2::get( handle h )
  {
    return m_windows.get( h );
  }

  void transparent_windows_renderable2::sort()
  {
    // first compute the depth for all windows (done in parallel by the tight buffer manager)
    m_windows.process( storage_depth_computation{renderer->get_view_matrix()} );
    // then sort windows according to this depth (done in parallel by the tight buffer manager)
    m_windows.sort( storage_depth_ordering{} );
    // now update the data on the GPU.
    glcheck(glBindBuffer( GL_ARRAY_BUFFER, m_vbos[ windows_vbo_id] ));
    glcheck(glBufferData( GL_ARRAY_BUFFER, sizeof(storage) * m_windows.get_size(), m_windows.data(), GL_DYNAMIC_DRAW));
  }

    void transparent_windows_renderable2::update_gpu_data()
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

    void transparent_windows_renderable2::do_render()
    {
      // Sort the windows according to the depth of their centers. This way, we can
      // render windows from the back of the scene to the front and be able to combine
      // their color with opaque objects as well as with already drawn windows. Note that
      // this function update the data on the gpu so we do not have to care about it here.
      sort();
      gpu_mat4 vp = renderer->get_projection_matrix() * renderer->get_view_matrix();
      glcheck(glUniformMatrix4fv( m_program->get_uniform_location( "vp"), 1, GL_FALSE, glm::value_ptr(vp)));
      glcheck(glBindVertexArray( m_vao ));
      glcheck(glDrawArrays( GL_POINTS, 0, m_windows.get_size()));
      glcheck(glBindVertexArray( 0 ) );
    }

    void transparent_windows_renderable2::remove_gpu_data()
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
  }
}
