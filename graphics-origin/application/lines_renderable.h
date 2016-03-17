/* Created on: Mar 17, 2016
 *     Author: T.Delame (tdelame@gmail.com)
 */
# ifndef GRAPHICS_ORIGIN_LINES_RENDERABLE_H_
# define GRAPHICS_ORIGIN_LINES_RENDERABLE_H_

# include "../graphics_origin.h"
# include "renderable.h"
# include "../tools/tight_buffer_manager.h"

namespace graphics_origin {

  namespace application {

    class lines_renderable
      : public renderable {
    public:

      struct storage {
        gpu_vec3 p1;
        gpu_vec3 color1;
        gpu_vec3 p2;
        gpu_vec3 color2;
        storage( const storage& other );
        storage& operator=( storage && other );
        storage& operator=( const storage& other );
        storage();
      };
    private:
      typedef tools::tight_buffer_manager<
          storage,
          uint32_t,
          22 > lines_buffer;
    public:

      lines_renderable(
          shader_program_ptr program,
          size_t expected_number_of_lines = 0);
      ~lines_renderable();
      lines_buffer::handle add(
          const gpu_vec3& p1, const gpu_vec3& color1,
          const gpu_vec3& p2, const gpu_vec3& color2 );
      void remove( lines_buffer::handle  handle );
    private:
      void update_gpu_data() override;
      void do_render() override;
      void remove_gpu_data() override;

      lines_buffer m_lines;
      unsigned int m_vao;
      unsigned int m_lines_vbo;
    };
  }
}
# endif 
