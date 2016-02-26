/* Created on: Feb 24, 2016
 *     Author: T.Delame (tdelame@gmail.com)
 */
# include <graphics-origin/application/balls_renderable.h>
# include <graphics-origin/application/gl_window_renderer.h>
# include <graphics-origin/application/gl_helper.h>

# include <graphics-origin/tools/log.h>
# include <GL/glew.h>

# include <thrust/sort.h>

namespace graphics_origin { namespace application {

  balls_renderable::storage::storage( const gpu_vec4& ball, const gpu_vec3& color )
    : ball{ ball }, color{ color }
  {}

  balls_renderable::storage::storage()
    : ball{}, color{}
  {}

  balls_renderable::storage&
  balls_renderable::storage::operator=( storage&& other )
  {
    ball  = other.ball ;
    color = other.color;
    return *this;
  }

  balls_renderable::storage&
  balls_renderable::storage::operator=( const storage& other )
  {
    ball  = std::move(other.ball) ;
    color = std::move(other.color);
    return *this;
  }

  balls_renderable::storage::storage( const storage& other )
    : ball{ other.ball }, color{ other.color }
  {}

  balls_renderable::balls_renderable(
      shader_program_ptr program,
      size_t expected_number_of_balls )
    : m_balls{ expected_number_of_balls },
      m_vao{ 0 }, m_balls_vbo{ 0 }
  {
    m_model = gpu_mat4(1.0);
    m_program = program;
  }

  balls_renderable::balls_buffer::handle
  balls_renderable::add( const geometry::ball& ball, const gpu_vec3& color )
  {
    m_dirty = true;
    auto pair = m_balls.create();
    pair.second.ball = gpu_vec4{ ball.get_center(), ball.get_radius() };
    pair.second.color  = color;
    return pair.first;
  }

  void
  balls_renderable::remove( balls_buffer::handle handle )
  {
    m_balls.remove( handle );
    m_dirty = true;
  }


  struct ball_storage_sorter {

    ball_storage_sorter( const gpu_vec4& indicator )
      : m_indicator{ indicator }
    {}

    bool operator()( const balls_renderable::storage& a, const balls_renderable::storage& b ) const
     {
       return dot( m_indicator, gpu_vec4(a.ball.x, a.ball.y, a.ball.z, 1 ) ) + a.ball.w
           < dot( m_indicator, gpu_vec4(b.ball.x, b.ball.y, b.ball.z, 1 ) ) + b.ball.w;
     }

    gpu_vec4 m_indicator;
  };


  void
  balls_renderable::update_gpu_data()
  {
    if( !m_vao )
      {
        glcheck(glGenVertexArrays( 1, &m_vao ));
        glcheck(glGenBuffers( 1, &m_balls_vbo ) );
      }

//    auto mv = m_renderer->get_view_matrix() * m_model;
//    thrust::sort( m_balls.begin(), m_balls.end(), ball_storage_sorter{gpu_vec4{ mv[0][2], mv[1][2], mv[2][2], mv[3][2] }} );


    int ball_location  = m_program->get_attribute_location(  "ball_attribute" );
    int color_location = m_program->get_attribute_location( "color_attribute" );

    glcheck(glBindVertexArray( m_vao ));
      glcheck(glBindBuffer( GL_ARRAY_BUFFER, m_balls_vbo ));
      glcheck(glBufferData( GL_ARRAY_BUFFER, sizeof(storage) * m_balls.get_size(), m_balls.data(), GL_STATIC_DRAW ));
      glcheck(glEnableVertexAttribArray( ball_location ));
      glcheck(glVertexAttribPointer( ball_location,        // format of ball:
        4, GL_FLOAT, GL_FALSE,                             // 4 unnormalized floats
        sizeof(storage),                                   // each attribute has the size of storage
        reinterpret_cast<void*>(offsetof(storage,ball)))); // offset of the center inside an attribute

      glcheck(glEnableVertexAttribArray( color_location ));
      glcheck(glVertexAttribPointer( color_location,         // format of color:
        3, GL_FLOAT, GL_FALSE,                               // 3 unnormalized floats
        sizeof(storage),                                     // each attribute has the size of storage
        reinterpret_cast<void*>(offsetof(storage,color))));  // offset of the color inside an attribute
    glcheck(glBindVertexArray( 0 ));
  }

  void
  balls_renderable::do_render()
  {
    gpu_mat4 temp = m_renderer->get_projection_matrix();
    glcheck(glUniformMatrix4fv( m_program->get_uniform_location("projection"), 1, GL_FALSE, glm::value_ptr(temp)));
    temp = m_renderer->get_view_matrix() * m_model;
    glcheck(glUniformMatrix4fv( m_program->get_uniform_location( "mv"), 1, GL_FALSE, glm::value_ptr(temp)));

    glcheck(glBindVertexArray( m_vao ));
    glcheck(glDrawArrays( GL_POINTS, 0, m_balls.get_size()));
    glcheck(glBindVertexArray( 0 ) );
  }

  void
  balls_renderable::remove_gpu_data()
  {
    if( m_vao )
      {
        glcheck(glDeleteBuffers( 1, &m_balls_vbo ));
        glcheck(glDeleteVertexArrays( 1, &m_vao ));
        m_balls_vbo = (unsigned int)0;
        m_vao = (unsigned int)0;
      }
  }

  balls_renderable::~balls_renderable()
  {
    remove_gpu_data();
  }
}}
