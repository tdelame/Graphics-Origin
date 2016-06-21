/*  Created on: Jun 12, 2016
 *      Author: T. Delame (tdelame@gmail.com) */

#ifndef WINDOW_FRAMES_RENDERABLE2_H_
#define WINDOW_FRAMES_RENDERABLE2_H_

# include "../../graphics-origin/graphics_origin.h"
# include "../../graphics-origin/application/renderable.h"
# include "../../graphics-origin/tools/tight_buffer_manager.h"

namespace graphics_origin {
  namespace application {

    class window_frames_renderable2 :
        public graphics_origin::application::renderable {
      struct storage {
        gpu_vec3 center;
        gpu_vec3 v1;
        gpu_vec3 v2;
        gpu_real size;
        gpu_real depth;

        storage( const storage& other );
        storage& operator=( const storage& other );
        storage& operator=( storage&& other );
        storage();
      };

      typedef tools::tight_buffer_manager< storage, uint32_t, 22 > frames_buffer;
    public:
      typedef frames_buffer::handle handle;

      /**@brief Create a new collection of window frames.
       *
       * Build an instance of a window frames renderable.
       * @param program The shader program used to render a frame. This shader
       * should have the following attributes:
       * - center (vec3) for the center of a window
       * - v1 (vec3) to go from the center to a corner
       * - v2 (vec3) to go from the center to a consecutive corner
       * - size (float) dimension of the frame in the plane
       * - depth (float) dimension of the frame in the normal direction
       * The shader should also have a uniform named vp (mat4) to receive the
       * product between the projection matrix and the view matrix.
       * @param expected_number_of_frames A guess about the final number of frames
       * stored in the instance. A correct guess avoids resizing internal buffer.
       */
      window_frames_renderable2(
        shader_program_ptr program,
        size_t expected_number_of_frames = 0 );
      ~window_frames_renderable2();

      handle add( const gpu_vec3& center, const gpu_vec3& v1, const gpu_vec3& v2, gpu_real size, gpu_real depth );

    private:
      void update_gpu_data() override;
      void do_render() override;
      void remove_gpu_data() override;

      frames_buffer m_frames;
      enum { attributes_vbo_id, number_of_vbos };
      unsigned int m_vao;
      unsigned int m_vbos[ number_of_vbos ];
    };

  }
}

#endif /* WINDOW_FRAMES_RENDERABLE_H_ */
