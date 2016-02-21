/*  Created on: Feb 20, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */
# include <graphics-origin/application/aaboxes_renderable.h>
# include <graphics-origin/application/gl_helper.h>

# include <graphics-origin/tools/log.h>
# include <GL/glew.h>

namespace graphics_origin { namespace application {

  aaboxes_renderable::storage::storage( const gpu_vec3& c, const gpu_vec3& h )
    : center{ c }, hsides{ h }
  {}

  aaboxes_renderable::storage::storage()
    : center{}, hsides{}
  {}

  aaboxes_renderable::storage&
  aaboxes_renderable::storage::operator=( storage&& other )
  {
    center = other.center;
    hsides = other.hsides;
    return *this;
  }


  aaboxes_renderable::aaboxes_renderable(
      shader_program_ptr program,
      size_t expected_number_of_boxes )
    : m_boxes{ expected_number_of_boxes },
      m_boxes_vbo{ 0 }
  {
    m_program = program;
    m_model = gpu_mat4(1.0);
  }

  aaboxes_renderable::boxes_buffer::handle
  aaboxes_renderable::add( geometry::aabox&& box )
  {
    m_dirty = true;
    auto pair = m_boxes.create();
    pair.second.center = box.get_center();
    pair.second.hsides = box.get_half_sides();


    centers.push_back( box.get_center() );
    hsides.push_back( box.get_half_sides() );

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
    if( !m_boxes_vbo )
      {
        glcheck(glGenBuffers( 1, &m_boxes_vbo ) );

        glcheck(glGenBuffers( 1, &center_vbo ) );
        glcheck(glGenBuffers( 1, &hsides_vbo ) );
      }

    glcheck(glBindBuffer( GL_ARRAY_BUFFER, center_vbo ));
    glcheck(glBufferData( GL_ARRAY_BUFFER, sizeof(gpu_vec3) * centers.size(), centers.data(), GL_STATIC_DRAW ));

    glcheck(glBindBuffer( GL_ARRAY_BUFFER, hsides_vbo ));
    glcheck(glBufferData( GL_ARRAY_BUFFER, sizeof(gpu_vec3) * hsides.size(), hsides.data(), GL_STATIC_DRAW ));

    glcheck(glBindBuffer( GL_ARRAY_BUFFER, m_boxes_vbo ));
    glcheck(glBufferData( GL_ARRAY_BUFFER, sizeof(storage) * m_boxes.get_size(), m_boxes.data(), GL_STATIC_DRAW ));

    {
      const auto size = m_boxes.get_size();
      for( size_t i = 0; i < size; ++ i )
        {
          auto& b = m_boxes.get_by_index( i );
          LOG( debug, "box #" << i << ": " << b.center << " " << b.hsides );
        }
      LOG( debug, "sizeof a box  = " << sizeof(storage));
      LOG( debug, "sizeof a vec3 = " << sizeof(gpu_vec3));

    }
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


    glcheck(glBindBuffer( GL_ARRAY_BUFFER, center_vbo ));
    glcheck(glVertexAttribPointer( center, 3, GL_FLOAT, GL_FALSE, sizeof(gpu_vec3), 0 ));

    glcheck(glBindBuffer( GL_ARRAY_BUFFER, hsides_vbo ));
    glcheck(glVertexAttribPointer( half_sides, 3, GL_FLOAT, GL_FALSE, sizeof(gpu_vec3), 0 ));
//
//    glcheck(glBindBuffer( GL_ARRAY_BUFFER, m_boxes_vbo ));
//    glcheck(glVertexAttribPointer( center, 3, GL_FLOAT, GL_FALSE, sizeof(storage),reinterpret_cast<void*>(offsetof(storage,center))));
//    glcheck(glVertexAttribPointer( half_sides, 3, GL_FLOAT, GL_FALSE, sizeof(storage),
//       reinterpret_cast<void*>(offsetof(storage,hsides))));

    glcheck(glLineWidth( 4.0 ));
    glcheck(glPointSize( 4.0 ));
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
