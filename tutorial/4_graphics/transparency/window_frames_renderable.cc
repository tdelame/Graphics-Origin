/*  Created on: Jun 12, 2016
 *      Author: T. Delame (tdelame@gmail.com) */
# include "window_frames_renderable.h"
# include "../../graphics-origin/application/gl_helper.h"
# include "../../graphics-origin/application/renderer.h"
# include "../../graphics-origin/tools/log.h"

# include <GL/glew.h>

namespace graphics_origin {
  namespace application {

    window_frames_renderable::storage::storage( const storage& other ) :
        center( other.center ), v1( other.v1 ), v2( other.v2 ),
        size( other.size ), depth( other.depth )
    {}

    window_frames_renderable::storage&
    window_frames_renderable::storage::operator=( const storage& other )
    {
      center = other.center;
      v1 = other.v1;
      v2 = other.v2;
      size = other.size;
      depth = other.depth;
      return *this;
    }

    window_frames_renderable::storage&
    window_frames_renderable::storage::operator=( storage&& other )
    {
      center = other.center;
      v1 = other.v1;
      v2 = other.v2;
      size = other.size;
      depth = other.depth;
      return *this;
    }

    window_frames_renderable::storage::storage() :
        size( 0 ), depth( 0 )
    {}

    window_frames_renderable::window_frames_renderable(
      shader_program_ptr program,
      size_t expected_number_of_frames ) :
          m_frames( expected_number_of_frames ), m_vao{ 0 }, m_vbos{ 0 }
    {
      model = gpu_mat4(1.0);
      this->program = program;
    }

    window_frames_renderable::~window_frames_renderable()
    {
      remove_gpu_data();
    }

    window_frames_renderable::handle
    window_frames_renderable::add(
      const gpu_vec3& center,
      const gpu_vec3& v1,
      const gpu_vec3& v2,
      gl_real size,
      gl_real depth )
    {
      set_dirty();
      auto pair = m_frames.create();
      pair.second.center = center;
      pair.second.v1 = v1;
      pair.second.v2 = v2;
      pair.second.size = size;
      pair.second.depth = depth;
      return pair.first;
    }

    void window_frames_renderable::update_gpu_data()
    {
      if( !m_vao )
        {
          glcheck(glGenVertexArrays( 1, &m_vao ));
          glcheck(glGenBuffers( number_of_vbos, m_vbos ));
        }
      int center_location = program->get_attribute_location( "center" );
      int v1_location = program->get_attribute_location( "v1" );
      int v2_location = program->get_attribute_location( "v2" );
      int size_location = program->get_attribute_location( "size" );
      int depth_location = program->get_attribute_location( "depth" );

      glcheck(glBindVertexArray( m_vao ));
        glcheck(glBindBuffer( GL_ARRAY_BUFFER, m_vbos[ attributes_vbo_id ] ));
        glcheck(glBufferData( GL_ARRAY_BUFFER, sizeof(storage) * m_frames.get_size(), m_frames.data(), GL_STATIC_DRAW ));

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

        glcheck(glEnableVertexAttribArray( size_location ));
        glcheck(glVertexAttribPointer( size_location,          // format of size:
          1, GL_FLOAT, GL_FALSE,                               // 1 unnormalized floats
          sizeof(storage),                                     // each attribute has the size of storage
          reinterpret_cast<void*>(offsetof(storage,size))));   // offset of the size inside an attribute

        glcheck(glEnableVertexAttribArray( depth_location ));
        glcheck(glVertexAttribPointer( depth_location,         // format of depth:
          1, GL_FLOAT, GL_FALSE,                               // 1 unnormalized floats
          sizeof(storage),                                     // each attribute has the depth of storage
          reinterpret_cast<void*>(offsetof(storage,depth))));  // offset of the depth inside an attribute

      glcheck(glBindVertexArray( 0 ));
    }

    void window_frames_renderable::do_render()
    {
      gpu_mat4 vp = renderer_ptr->get_projection_matrix() * renderer_ptr->get_view_matrix();
      glcheck(glUniformMatrix4fv( program->get_uniform_location( "vp"), 1, GL_FALSE, glm::value_ptr(vp)));
      glcheck(glBindVertexArray( m_vao ));
      glcheck(glDrawArrays( GL_POINTS, 0, m_frames.get_size()));
      glcheck(glBindVertexArray( 0 ) );
    }

    void window_frames_renderable::remove_gpu_data()
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
