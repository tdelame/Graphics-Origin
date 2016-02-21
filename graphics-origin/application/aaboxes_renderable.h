/*  Created on: Feb 20, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */

# ifndef GRAPHICS_ORIGIN_AABOX_RENDERABLES_H_
# define GRAPHICS_ORIGIN_AABOX_RENDERABLES_H_
# include "../graphics_origin.h"
# include "renderable.h"
# include "../tools/tight_buffer_manager.h"
# include "../geometry/box.h"

namespace graphics_origin {
  namespace application {
    class aaboxes_renderable
      : public renderable {
      typedef tools::tight_buffer_manager<
          geometry::aabox,
          uint32_t,
          22 > boxes_buffer;
    public:
      aaboxes_renderable(
          shader_program_ptr program,
          size_t expected_number_of_boxes = 0 );
      ~aaboxes_renderable();
      boxes_buffer::handle add( geometry::aabox&& box );
      void remove( boxes_buffer::handle handle );
      geometry::aabox& get( boxes_buffer::handle handle );
    private:
      void update_gpu_data() override;
      void do_render() override;
      void remove_gpu_data() override;

      tools::tight_buffer_manager<
        geometry::aabox,
        uint32_t,
        22 > m_boxes;

      unsigned int m_boxes_vbo;
    };
  }
}
# endif
