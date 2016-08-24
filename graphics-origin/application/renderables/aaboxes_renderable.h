# ifndef GRAPHICS_ORIGIN_AABOX_RENDERABLES_H_
# define GRAPHICS_ORIGIN_AABOX_RENDERABLES_H_
# include "../renderable.h"
# include "../../tools/tight_buffer_manager.h"
# include "../../geometry/box.h"

namespace graphics_origin {
  namespace geometry {
    template< typename bounding_object >
    class bvh;
  }
  namespace application {
    class GO_API aaboxes_renderable
      : public renderable {

      struct storage {
        gl_vec3 center;
        gl_vec3 hsides;
        gl_vec3 color;
        storage( const gl_vec3& center, const gl_vec3& hsides, const gl_vec3& color );

        storage& operator=( storage && other );
        storage();
      };

      typedef tools::tight_buffer_manager<
          storage,
          uint32_t,
          22 > boxes_buffer;
    public:
      aaboxes_renderable(
          shader_program_ptr program,
          size_t expected_number_of_boxes = 0 );
      ~aaboxes_renderable();
      boxes_buffer::handle add( const geometry::aabox& box, const gl_vec3& color = gl_vec3{0.1, 0.1, 0.1 } );
      void remove( boxes_buffer::handle handle );
    private:
      void update_gpu_data() override;
      void do_render() override;
      void remove_gpu_data() override;

      boxes_buffer m_boxes;
      unsigned int m_vao;
      unsigned int m_boxes_vbo;
    };

    GO_API
    aaboxes_renderable*
    aaboxes_renderable_from_box_bvh( shader_program_ptr program, geometry::bvh<geometry::aabox>& bvh );
  }
}
# endif
