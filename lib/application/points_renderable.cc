/* Created on: Mar 17, 2016
 *     Author: T.Delame (tdelame@gmail.com)
 */
# include "../../graphics-origin/application/points_renderable.h"
# include "../../graphics-origin/application/gl_window_renderer.h"
# include "../../graphics-origin/application/gl_helper.h"

# include <GL/glew.h>

namespace graphics_origin { namespace application {

  points_renderable::storage::storage( const gpu_vec3& center, const gpu_vec3& color )
    : center{ center }, color{ color }
  {}

  points_renderable::storage::storage()
    : center{}, color{}
  {}

  points_renderable::storage&
  points_renderable::storage::operator=( storage&& other )
  {
    center = other.center;
    color = other.color;
    return *this;
  }

  points_renderable::storage&
  points_renderable::storage::operator=( const storage& other )
  {
    center = std::move(other.center) ;
    color = std::move(other.color);
    return *this;
  }

  points_renderable::storage::storage( const storage& other )
    : center{ other.center }, color{ other.color }
  {}

  points_renderable::points_renderable(
      shader_program_ptr program,
      size_t expected_number_of_points )
    : m_points{ expected_number_of_points },
      m_vao{ 0 }, m_points_vbo{ 0 }
  {
    m_model = gpu_mat4(1.0);
    m_program = program;
  }

  points_renderable::points_buffer::handle
  points_renderable::add( const gpu_vec3& center, const gpu_vec3& color )
  {
    m_dirty = true;
    auto pair = m_points.create();
    pair.second.center = center;
    pair.second.color  = color;
    return pair.first;
  }

  void
  points_renderable::remove( points_buffer::handle handle )
  {
    m_points.remove( handle );
    m_dirty = true;
  }

  void
  points_renderable::update_gpu_data()
  {
    if( !m_vao )
      {
        glcheck(glGenVertexArrays( 1, &m_vao ));
        glcheck(glGenBuffers( 1, &m_points_vbo ) );
      }

    int position_location = m_program->get_attribute_location( "position" );
    int color_location = m_program->get_attribute_location( "color" );

    glcheck(glBindVertexArray( m_vao ));
      glcheck(glBindBuffer( GL_ARRAY_BUFFER, m_points_vbo ));
      glcheck(glBufferData( GL_ARRAY_BUFFER, sizeof(storage) * m_points.get_size(), m_points.data(), GL_STATIC_DRAW ));
      glcheck(glEnableVertexAttribArray( position_location ));
      glcheck(glVertexAttribPointer( position_location,    // format of center:
        3, GL_FLOAT, GL_FALSE,                             // 3 unnormalized floats
        sizeof(storage),                                   // each attribute has the size of storage
        reinterpret_cast<void*>(offsetof(storage,center)))); // offset of the center inside an attribute

      glcheck(glEnableVertexAttribArray( color_location ));
      glcheck(glVertexAttribPointer( color_location,       // format of color:
        4, GL_FLOAT, GL_FALSE,                             // 4 unnormalized floats
        sizeof(storage),                                   // each attribute has the size of storage
        reinterpret_cast<void*>(offsetof(storage,color))));// offset of the color inside an attribute
    glcheck(glBindVertexArray( 0 ));
  }

  void
  points_renderable::do_render()
  {
    gpu_mat4 temp = m_renderer->get_projection_matrix() * m_renderer->get_view_matrix() * m_model;
    glcheck(glUniformMatrix4fv( m_program->get_uniform_location( "mvp"), 1, GL_FALSE, glm::value_ptr(temp)));
    glcheck(glPointSize(4.0));
    glcheck(glBindVertexArray( m_vao ));
    glcheck(glDrawArrays( GL_POINTS, 0, m_points.get_size()));
    glcheck(glBindVertexArray( 0 ) );
  }

  void
  points_renderable::remove_gpu_data()
  {
    if( m_vao )
      {
        glcheck(glDeleteBuffers( 1, &m_points_vbo ));
        glcheck(glDeleteVertexArrays( 1, &m_vao ));
        m_points_vbo = (unsigned int)0;
        m_vao = (unsigned int)0;
      }
  }

  points_renderable::~points_renderable()
  {
    remove_gpu_data();
  }
}}
