/*  Created on: Feb 20, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */
# include "../../graphics-origin/application/renderables/aaboxes_renderable.h"
# include "../../graphics-origin/application/renderer.h"
# include "../../graphics-origin/application/gl_helper.h"
# include "../../graphics-origin/geometry/bvh.h"
# include "../../graphics-origin/tools/log.h"

# include <GL/glew.h>

namespace graphics_origin { namespace application {

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
      shader_program_ptr program,
      size_t expected_number_of_boxes )
    : m_boxes{ expected_number_of_boxes },
      m_vao{ 0 }, m_boxes_vbo{ 0 }
  {
    model = gpu_mat4(1.0);
    this->program = program;
  }

  aaboxes_renderable::boxes_buffer::handle
  aaboxes_renderable::add( const geometry::aabox& box, const gpu_vec3& color )
  {
    m_dirty = true;
    auto pair = m_boxes.create();
    pair.second.center = box.m_center;
    pair.second.hsides = box.m_hsides;
    pair.second.color  = color;
    return pair.first;
  }

  void
  aaboxes_renderable::remove( boxes_buffer::handle handle )
  {
    m_boxes.remove( handle );
    m_dirty = true;
  }

  void
  aaboxes_renderable::update_gpu_data()
  {
    if( !m_vao )
      {
        glcheck(glGenVertexArrays( 1, &m_vao ));
        glcheck(glGenBuffers( 1, &m_boxes_vbo ) );
      }

    int center_location = program->get_attribute_location( "center" );
    int hsides_location = program->get_attribute_location( "hsides" );
    int  color_location = program->get_attribute_location( "color"  );

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
      glcheck(glVertexAttribPointer( color_location,         // format of colors:
        3, GL_FLOAT, GL_FALSE,                               // 3 unnormalized floats
        sizeof(storage),                                     // each attribute has the size of storage
        reinterpret_cast<void*>(offsetof(storage,color))));  // offset of the color inside an attribute
    glcheck(glBindVertexArray( 0 ));
  }

  void
  aaboxes_renderable::do_render()
  {
    gpu_mat4 mvp = renderer_ptr->get_projection_matrix() * renderer_ptr->get_view_matrix() * model;
    glcheck(glUniformMatrix4fv( program->get_uniform_location( "mvp"), 1, GL_FALSE, glm::value_ptr(mvp)));
    glcheck(glBindVertexArray( m_vao ));
    glcheck(glDrawArrays( GL_POINTS, 0, m_boxes.get_size()));
    glcheck(glBindVertexArray( 0 ) );
  }

  void
  aaboxes_renderable::remove_gpu_data()
  {
    if( m_vao )
      {
        glcheck(glDeleteBuffers( 1, &m_boxes_vbo ));
        glcheck(glDeleteVertexArrays( 1, &m_vao ));
        m_boxes_vbo = (unsigned int)0;
        m_vao = (unsigned int)0;
      }
  }

  aaboxes_renderable::~aaboxes_renderable()
  {
    remove_gpu_data();
  }

  aaboxes_renderable*
  aaboxes_renderable_from_box_bvh( shader_program_ptr program, geometry::bvh<geometry::aabox>& bvh )
  {
    const auto nb_boxes = bvh.get_number_of_nodes();
    auto result = new aaboxes_renderable( program, nb_boxes );

    uint32_t max_level = 0;
    {
      std::list< std::pair<uint32_t,uint32_t> > indexes( 1, std::make_pair( uint32_t{0}, uint32_t{0}) );
      while( !indexes.empty() )
        {
          auto current_index = indexes.front();
          indexes.pop_front();
          max_level = std::max( max_level, current_index.second );
          if( !bvh.is_leaf( current_index.first ) )
            {
              const auto& node = bvh.get_node( current_index.first );
              indexes.push_back( std::make_pair( node.left_index, current_index.second + 1));
              indexes.push_back( std::make_pair( node.right_index, current_index.second + 1));
            }
        }
      LOG(debug, "max level = " << max_level);
    }


    std::list< std::pair<uint32_t,uint32_t> > indexes( 1, std::make_pair( uint32_t{0}, uint32_t{0}) );
    while( !indexes.empty() )
      {
        auto current_index = indexes.front();
        indexes.pop_front();
        if( bvh.is_leaf( current_index.first ) )
          {
            result->add( bvh.get_node( current_index.first ).bounding, get_color( current_index.second, 0, max_level ) );
          }
        else
          {
            const auto& node = bvh.get_node( current_index.first );
            result->add( node.bounding, get_color( current_index.second, 0, max_level ) );
            indexes.push_back( std::make_pair( node.left_index, current_index.second + 1));
            indexes.push_back( std::make_pair( node.right_index, current_index.second + 1));
          }
      }
    return result;
  }

}}
