/*  Created on: Feb 20, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */
# include <graphics-origin/application/aaboxes_renderable.h>
# include <graphics-origin/application/gl_helper.h>
# include <GL/glew.h>

namespace graphics_origin { namespace application {

  aaboxes_renderable::aaboxes_renderable(
      shader_program_ptr program,
      size_t expected_number_of_boxes )
    : m_boxes{ expected_number_of_boxes },
      m_boxes_vbo{ 0 }
  {
    m_program = program;
  }

  aaboxes_renderable::boxes_buffer::handle
  aaboxes_renderable::add( geometry::aabox&& box )
  {
    m_dirty = true;
    auto pair = m_boxes.create();
    pair.second = std::move( box );
    return pair.first;
  }

  void
  aaboxes_renderable::remove( boxes_buffer::handle handle )
  {
    m_boxes.remove( handle );
  }

  geometry::aabox&
  aaboxes_renderable::get( boxes_buffer::handle handle )
  {
    return m_boxes.get( handle );
  }

  void
  aaboxes_renderable::update_gpu_data()
  {
    if( !m_boxes_vbo )
      {
        glcheck(glGenBuffers( 1, &m_boxes_vbo ) );
      }
    glcheck(glBindBuffer( GL_ARRAY_BUFFER, m_boxes_vbo ));
    glcheck(glBufferData( GL_ARRAY_BUFFER, 3 * sizeof(geometry::aabox), m_boxes.data(), GL_STATIC_DRAW ));
  }

  void
  aaboxes_renderable::do_render()
  {
    int model = m_program->get_uniform_location("model");
    glcheck(glUniformMatrix4fv( model, 1, GL_FALSE, glm::value_ptr(m_model)));

    int center = m_program->get_attribute_location("center");
    int half_sides = m_program->get_attribute_location("half_sides");

    glcheck(glEnableVertexAttribArray( center ));
    glcheck(glEnableVertexAttribArray( half_sides ));

    glcheck(glBindBuffer( GL_ARRAY_BUFFER, m_boxes_vbo ));
    glcheck(glVertexAttribPointer( center, 3, GL_DOUBLE, GL_FALSE, sizeof(geometry::aabox),0));
    glcheck(glVertexAttribPointer( half_sides, 3, GL_DOUBLE, GL_FALSE, sizeof(geometry::aabox),
       reinterpret_cast<void*>(sizeof(vec3))));

    glcheck(glDrawArrays(GL_POINTS, 0, m_boxes.get_size()));
  }

  void
  aaboxes_renderable::remove_gpu_data()
  {
    glcheck(glDeleteBuffers( 1, &m_boxes_vbo ));
    m_boxes_vbo = (unsigned int)0;
  }

  aaboxes_renderable::~aaboxes_renderable()
  {
    remove_gpu_data();
  }
}}
