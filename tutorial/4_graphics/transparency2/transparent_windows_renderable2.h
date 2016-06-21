/* Created on: Jun 13, 2016
 *     Author: T.Delame (tdelame@gmail.com)
 */
# ifndef PROJECT_TRANSPARENT_WINDOWS_RENDERABLE2_H_
# define PROJECT_TRANSPARENT_WINDOWS_RENDERABLE2_H_
# include "../../../graphics-origin/application/renderable.h"
# include "../../../graphics-origin/tools/tight_buffer_manager.h"

namespace graphics_origin {
  namespace application {
    class camera;
    /**@brief A collection of transparent windows to render.
     *
     * A transparent window is a quad that let the light go through it. As such,
     * we can see any opaque object that lies behind a transparent window. We show
     * in this tutorial how to do it.
     *
     * First, a window is defined by its center, and two vectors that goes from
     * the center to two consecutive corners of that window. Those two vectors
     * are named v1 and v2, and should not be colinear (otherwise, you have a
     * zero-sized window by definition). Then, a window has a RGBA color.
     *
     * It should be noted that the current code can handle any orientation of the
     * corners. But if face culling is activated, some holes can appear depending
     * on the view point.
     *
     * A window has no thickness, but you can either add another parallel
     * window or modify the code to render a box instead of a quad.
     *
     * This code has a limitation related to the difficulty of performing
     * transparency with OpenGL: quads are not always in front of other
     * quads, thus, only parts of a quad need to be rendered before another
     * one. Partial primitive rendering is not possible with the rasterization
     * pipeline. There are more evolved rendering methods to address this issue,
     * like ray-tracing.
     * TODO: make a simple implementation of a ray-tracing (on the cpu).
     */
    class transparent_windows_renderable2
      : public graphics_origin::application::renderable {

      struct storage {
        gpu_vec3 center;
        gpu_vec3 v1;
        gpu_vec3 v2;
        gpu_vec4 color;
        gpu_real depth;

        storage( const storage& other );
        storage& operator=( const storage& other );
        storage& operator=( storage&& other );
        storage();
      };

      typedef tools::tight_buffer_manager<
          storage,
          uint32_t,
          22 > windows_buffer;

      struct storage_depth_computation {
        storage_depth_computation( const gpu_mat4& view );
        void operator()( storage& s ) const;
        const gpu_vec3 eye;
        const gpu_vec3 forward;
      };

      struct storage_depth_ordering {
        bool operator()( const storage& a, const storage& b ) const;
      };

    public:
      typedef windows_buffer::handle handle;

      /**@brief Create a new collection of transparent windows.
       *
       * Build an instance of a transparent windows renderable.
       * @param program The shader program used to render a window. This shader
       * should have the following attributes:
       * - center (vec3) for the center of a window
       * - v1 (vec3) to go from the center to a corner
       * - v2 (vec3) to go from the center to a consecutive corner
       * - color (vec4) for the color of the window.
       * The shader should also have a uniform named vp (mat4) to receive the
       * product between the projection matrix and the view matrix.
       * @param expected_number_of_windows A guess about the final number of windows
       * stored in the instance. A correct guess avoids resizing internal buffer.
       */
      transparent_windows_renderable2(
          shader_program_ptr program,
          size_t expected_number_of_windows = 0 );
      ~transparent_windows_renderable2();

      /**@brief Add another transparent window to render.
       *
       * Add one more transparent window to this instance.
       * @param center Center of the window.
       * @param v1 Vector that goes from the center to one corner of the window.
       * @param v2 Vector that goes from the center to the next corner of the window.
       * @param color Color of the window.
       * @return Handle to the newly created window.
       */
      handle add(
          const gpu_vec3& center,
          const gpu_vec3& v1,
          const gpu_vec3& v2,
          const gpu_vec4& color );

      /**@brief Get an existing window.
       *
       * Access to a window that was previously created by add(). If you modify
       * the data of a window, be sure to notify
       * @param h Handle of the existing window.
       * @return The window pointed by the handle h.
       */
      storage& get( handle h );

    private:
      void update_gpu_data() override;
      void do_render() override;
      void remove_gpu_data() override;
      void sort();

      windows_buffer m_windows;
      enum{ windows_vbo_id, number_of_vbos };
      unsigned int m_vao;
      unsigned int m_vbos[ number_of_vbos];
    };
  }
}
# endif 
