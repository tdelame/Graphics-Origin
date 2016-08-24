# include "../../graphics-origin/application/renderables/lines_renderable.h"
# include "../../graphics-origin/application/renderer.h"
# include "../../graphics-origin/application/gl_helper.h"

# include <GL/glew.h>

namespace graphics_origin { namespace application {

  lines_renderable::storage::storage()
  {}

  lines_renderable::storage&
  lines_renderable::storage::operator=( storage&& other )
  {
    p1 = other.p1;
    color1 = other.color1;
    p2 = other.p2;
    color2 = other.color2;
    return *this;
  }

  lines_renderable::storage&
  lines_renderable::storage::operator=( const storage& other )
  {
    p1 = other.p1;
    color1 = other.color1;
    p2 = other.p2;
    color2 = other.color2;
    return *this;
  }

  lines_renderable::storage::storage( const storage& other )
    : p1{ other.p1 }, color1{ other.color1 }, p2{ other.p2 }, color2{ other.color2}
  {}

  lines_renderable::lines_renderable(
      shader_program_ptr program,
      size_t expected_number_of_lines )
    : m_lines{ expected_number_of_lines },
      m_vao{ 0 }, m_lines_vbo{ 0 }
  {
    model = gl_mat4(1.0);
    this->program = program;
  }

  lines_renderable::lines_buffer::handle
  lines_renderable::add(
      const gl_vec3& p1, const gl_vec3& color1,
      const gl_vec3& p2, const gl_vec3& color2 )
  {
    m_dirty = true;
    auto pair = m_lines.create();
    pair.second.p1 = p1;
    pair.second.color1 = color1;
    pair.second.p2 = p2;
    pair.second.color2  = color2;
    return pair.first;
  }

  void
  lines_renderable::remove( lines_buffer::handle handle )
  {
    m_lines.remove( handle );
    m_dirty = true;
  }

  void
  lines_renderable::update_gpu_data()
  {
    if( !m_vao )
      {
        glcheck(glGenVertexArrays( 1, &m_vao ));
        glcheck(glGenBuffers( 1, &m_lines_vbo ) );
      }

    int position_location = program->get_attribute_location( "position" );
    int color_location = program->get_attribute_location( "color" );

    glcheck(glBindVertexArray( m_vao ));
      glcheck(glBindBuffer( GL_ARRAY_BUFFER, m_lines_vbo ));
      glcheck(glBufferData( GL_ARRAY_BUFFER, sizeof(storage) * m_lines.get_size(), m_lines.data(), GL_STATIC_DRAW ));
      glcheck(glEnableVertexAttribArray( position_location ));
      glcheck(glVertexAttribPointer( position_location,    // format of center:
        3, GL_FLOAT, GL_FALSE,                             // 3 unnormalized floats
        sizeof(storage)>>1,                                   // each attribute has the size of storage
        reinterpret_cast<void*>(offsetof(storage,p1)))); // offset of the center inside an attribute

      glcheck(glEnableVertexAttribArray( color_location ));
      glcheck(glVertexAttribPointer( color_location,       // format of color:
        4, GL_FLOAT, GL_FALSE,                             // 4 unnormalized floats
        sizeof(storage)>>1,                                   // each attribute has the size of storage
        reinterpret_cast<void*>(offsetof(storage,color1))));// offset of the color inside an attribute
    glcheck(glBindVertexArray( 0 ));
  }

  void
  lines_renderable::do_render()
  {
    gl_mat4 temp = renderer_ptr->get_projection_matrix() * renderer_ptr->get_view_matrix() * model;
    glcheck(glUniformMatrix4fv( program->get_uniform_location( "mvp"), 1, GL_FALSE, glm::value_ptr(temp)));
    glcheck(glLineWidth(2.5));
    glcheck(glBindVertexArray( m_vao ));
    glcheck(glDrawArrays( GL_LINES, 0, m_lines.get_size() * 2));
    glcheck(glBindVertexArray( 0 ) );
  }

  void
  lines_renderable::remove_gpu_data()
  {
    if( m_vao )
      {
        glcheck(glDeleteBuffers( 1, &m_lines_vbo ));
        glcheck(glDeleteVertexArrays( 1, &m_vao ));
        m_lines_vbo = (unsigned int)0;
        m_vao = (unsigned int)0;
      }
  }

  lines_renderable::~lines_renderable()
  {
    remove_gpu_data();
  }
}}
