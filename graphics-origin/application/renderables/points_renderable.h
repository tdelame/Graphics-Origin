# ifndef GRAPHICS_ORIGIN_POINTS_RENDERABLE_H_
# define GRAPHICS_ORIGIN_POINTS_RENDERABLE_H_
# include "../renderable.h"
# include "../../tools/tight_buffer_manager.h"

namespace graphics_origin {
  namespace application {
    class GO_API points_renderable
      : public renderable {
    public:
      struct storage {
        gl_vec3 center;
        gl_vec3 color;
        storage( const gl_vec3& center, const gl_vec3& color);
        storage( const storage& other );
        storage& operator=( storage && other );
        storage& operator=( const storage& other );
        storage();
      };
    private:
      typedef tools::tight_buffer_manager<
          storage,
          uint32_t,
          22 > points_buffer;
    public:
      points_renderable(
          shader_program_ptr program,
          size_t expected_number_of_points = 0);
      ~points_renderable();
      points_buffer::handle add( const gl_vec3& center, const gl_vec3& color = gl_vec3{0.6, 0.2, 0.5} );
      void remove( points_buffer::handle  handle );
    private:
      void update_gpu_data() override;
      void do_render() override;
      void remove_gpu_data() override;

      points_buffer m_points;
      unsigned int m_vao;
      unsigned int m_points_vbo;
    };
  }
}
# endif 
