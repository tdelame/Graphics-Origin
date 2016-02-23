/*  Created on: Feb 20, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */
# include <graphics-origin/application/aaboxes_renderable.h>
# include <graphics-origin/application/gl_window_renderer.h>
# include <graphics-origin/application/gl_helper.h>

# include <graphics-origin/tools/log.h>
# include <GL/glew.h>

namespace graphics_origin { namespace application {

  static
  shader_program_ptr get_program()
  {
    static auto instance = std::make_shared<shader_program>(
        std::list<std::string>{
          "shaders/aabox.vert",
          "shaders/aabox.geom",
          "shaders/aabox.frag"
        }
    );
    return instance;
  }

  aaboxes_renderable::storage::storage( const gpu_vec3& center, const gpu_vec3& hsides, const gpu_vec3& color )
    : center{ center }, hsides{ hsides }, color{ color }
  {}

  aaboxes_renderable::storage::storage()
    : center{}, hsides{}, color{}
  {}

  aaboxes_renderable::storage&
  aaboxes_renderable::storage::operator=( storage&& other )
  {
    center = other.center;
    hsides = other.hsides;
    color  = other.color ;
    return *this;
  }


  aaboxes_renderable::aaboxes_renderable(
      size_t expected_number_of_boxes )
    : m_boxes{ expected_number_of_boxes },
      m_vao{ 0 }, m_boxes_vbo{ 0 }
  {
    m_model = gpu_mat4(1.0);
    m_program = get_program();
  }

  aaboxes_renderable::boxes_buffer::handle
  aaboxes_renderable::add( geometry::aabox&& box, const gpu_vec3& color )
  {
    m_dirty = true;
    auto pair = m_boxes.create();
    pair.second.center = box.get_center();
    pair.second.hsides = box.get_half_sides();
    pair.second.color  = color;
    return pair.first;
  }

  void
  aaboxes_renderable::remove( boxes_buffer::handle handle )
  {
    m_boxes.remove( handle );
  }

  void
  aaboxes_renderable::update_gpu_data()
  {
    if( !m_vao )
      {
        glcheck(glGenVertexArrays( 1, &m_vao ));
        glcheck(glGenBuffers( 1, &m_boxes_vbo ) );
      }

    int center_location = m_program->get_attribute_location( "center" );
    int hsides_location = m_program->get_attribute_location( "hsides" );
    int  color_location = m_program->get_attribute_location( "color"  );

    glcheck(glBindVertexArray( m_vao ));
      glcheck(glBindBuffer( GL_ARRAY_BUFFER, m_boxes_vbo ));
      glcheck(glBufferData( GL_ARRAY_BUFFER, sizeof(storage) * m_boxes.get_size(), m_boxes.data(), GL_STATIC_DRAW ));
      glcheck(glEnableVertexAttribArray( center_location ));
      glcheck(glVertexAttribPointer( center_location,        // format of center:
        3, GL_FLOAT, GL_FALSE,                               // 3 unnormalized floats
        sizeof(storage),                                     // each attribute has the size of storage
        reinterpret_cast<void*>(offsetof(storage,center)))); // offset of the center inside an attribute

      glcheck(glEnableVertexAttribArray( hsides_location ));
      glcheck(glVertexAttribPointer( hsides_location,        // format of hsides:
        3, GL_FLOAT, GL_FALSE,                               // 3 unnormalized floats
        sizeof(storage),                                     // each attribute has the size of storage
        reinterpret_cast<void*>(offsetof(storage,hsides)))); // offset of the hsides inside an attribute

      glcheck(glEnableVertexAttribArray( color_location ));
      glcheck(glVertexAttribPointer( color_location,         // format of hsides:
        3, GL_FLOAT, GL_FALSE,                               // 3 unnormalized floats
        sizeof(storage),                                     // each attribute has the size of storage
        reinterpret_cast<void*>(offsetof(storage,color)))); // offset of the hsides inside an attribute
    glcheck(glBindVertexArray( 0 ));
  }

  void
  aaboxes_renderable::do_render()
  {
    m_program->bind();

    gpu_mat4 mvp = m_renderer->get_projection_matrix() * m_renderer->get_view_matrix() * m_model;
    glcheck(glUniformMatrix4fv( m_program->get_uniform_location( "mvp"), 1, GL_FALSE, glm::value_ptr(mvp)));

    glcheck(glLineWidth( 2.0 ));
    glcheck(glBindVertexArray( m_vao ));
    glcheck(glDrawArrays( GL_POINTS, 0, m_boxes.get_size()));
    glcheck(glBindVertexArray( 0 ) );
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
