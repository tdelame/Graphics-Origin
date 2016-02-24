/* Created on: Feb 24, 2016
 *     Author: T.Delame (tdelame@gmail.com)
 */
# ifndef GRAPHICS_ORIGIN_BALLS_RENDERABLE_H_
# define GRAPHICS_ORIGIN_BALLS_RENDERABLE_H_
# include "../graphics_origin.h"
# include "renderable.h"
# include "../tools/tight_buffer_manager.h"
# include "../geometry/ball.h"

namespace graphics_origin {
  namespace application {
    class balls_renderable
      : public renderable {

      struct storage {
        gpu_vec4 ball;
        gpu_vec3 color;
        storage( const gpu_vec4& ball, const gpu_vec3& color );

        storage& operator=( storage && other );
        storage();
      };

      typedef tools::tight_buffer_manager<
          storage,
          uint32_t,
          22 > balls_buffer;
    public:
      balls_renderable(
          shader_program_ptr program,
          size_t expected_number_of_balls = 0 );
      ~balls_renderable();
      balls_buffer::handle add( const geometry::ball& ball, const gpu_vec3& color = gpu_vec3{0.1, 0.9, 0.1 } );
      void remove( balls_buffer::handle handle );
    private:
      void update_gpu_data() override;
      void do_render() override;
      void remove_gpu_data() override;

      balls_buffer m_balls;
      unsigned int m_vao;
      unsigned int m_balls_vbo;
    };
  }
}
# endif 
