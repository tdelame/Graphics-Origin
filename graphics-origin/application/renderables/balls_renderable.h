# ifndef GRAPHICS_ORIGIN_BALLS_RENDERABLE_H_
# define GRAPHICS_ORIGIN_BALLS_RENDERABLE_H_
# include "../renderable.h"
# include "../../tools/tight_buffer_manager.h"
# include "../../geometry/ball.h"

namespace graphics_origin {
  namespace application {
    class GO_API balls_renderable
      : public renderable {
    public:
      struct storage {
        gl_vec4 ball;
        gl_vec4 color;
        storage( const gl_vec4& ball, const gl_vec4& color );
        storage( const storage& other );

        storage& operator=( storage && other );
        storage& operator=( const storage& other );

        storage();
      };
    private:
      typedef tools::tight_buffer_manager<
          storage,
          uint32_t,
          22 > balls_buffer;
    public:
      balls_renderable(
          shader_program_ptr program,
          size_t expected_number_of_balls = 0 );
      ~balls_renderable();
      balls_buffer::handle add( const geometry::ball& ball, const gl_vec4& color = gl_vec4{0.1, 0.9, 0.1, 1.0 } );
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
