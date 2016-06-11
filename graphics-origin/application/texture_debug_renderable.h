/*  Created on: Jun 11, 2016
 *      Author: T. Delame (tdelame@gmail.com) */

#ifndef TEXTURE_DEBUG_RENDERABLE_H_
#define TEXTURE_DEBUG_RENDERABLE_H_

# include "../graphics_origin.h"
# include "renderable.h"
# include <mutex>

class FIBITMAP;

namespace graphics_origin {
  namespace application {

    /**@brief Debug the content of a texture as well as its transfer to the GPU.
     *
     * In most case, textures are loaded from image files and then transfered to
     * the GPU to be used by a sampler. Thus, apart from the coordinate textures,
     * there are many reasons why an object can be incorrectly textured:
     * - wrong image filename or file cannot be opened
     * - invalid image file type
     * - no loader for a specific image file type
     * - incorrect internal representation of an image (how pixels are stored,
     * including their colors and the stride between each row)
     * - incorrect transfer of the internal representation to OpenGL
     * - wrong texture unit / sampler bindings
     * - ...
     *
     * This renderable does the debug for you: we give it an OpenGL texture ID
     * or a image file name, and it try to display it on a full screen billboard.
     * If it fails to do so, log entries will be sent to the debug channel, to
     * help you to identify the failure.
     *
     * On the billboard, the lower left corner corresponds to the texture
     * coordinates (x,y)=(0,0). When we go to the right, the x coordinate
     * increases. When we go to the top, the y coordinate increases.
     *
     * Since the texture is rendered on a full screen billboard, we cannot
     * debug at the same time more than one texture. This is why it is possible
     * to turn on/off the debug as well as to change the texture to debug.
     */
    class GO_API texture_debug_renderable :
      public renderable {
    public:
      /**@brief Instantiate a texture debug renderable.
       *
       * Build a new texture debug renderable.
       * @program The shader program used to render the texture on a full
       * screen billboard. Such program should be valid and must have only one
       * uniform: a \c sampler2D named \c sampler. There is no vertex attribute,
       * since the billboard can be generated in the geometry shader. In
       * this shader, be sure to generate the right texture coordinates. A
       * correct example can be created from the shaders \c texture_debug.vert,
       * \c texture_debug.geom and \c texture_debug.frag. */
      texture_debug_renderable(
        shader_program_ptr program );
      /**@brief Delete this instance.
       *
       * Clean all data both on the GPU and the CPU related to this instance. */
      ~texture_debug_renderable();

      /**@brief Set the visibility of the billboard.
       *
       * Turn on/off the visibility of the billboard. By default, the billboard
       * is visible. If a texture has been loaded.
       * @param visible The new visibility of the billboard. */
      void set_visible( bool visible );

      /**@brief Set a texture by file name.
       *
       * Load a texture from a file name to debug it.
       * @param texture_file_name The file name of the texture to load. */
      void set_texture( const std::string& texture_file_name );

      /**@brief Set a texture by OpenGL texture ID.
       *
       * Reuse a texture already loaded by OpenGL. Please note that in
       * this case, this renderable will not be the owner of the texture.
       * It will be up to you to delete the texture from the GPU.
       * @param gl_texture_id Identifier of an already loaded texture.
       */
      void set_texture( unsigned int gl_texture_id );

    private:
      void update_gpu_data() override;
      void do_render() override;
      void remove_gpu_data() override;
      void unload_fib();

      std::mutex lock;
      FIBITMAP* m_fib;
      unsigned int m_texture_id;
      unsigned int m_next_texture_id;
      bool m_visible;
      /**@brief Is this renderable the owner of the OpenGL texture?
       *
       * True if this renderable is the owner of the OpenGL texture (and thus
       * should remove the texture from the GPU when needed).
       */
      bool m_owner;
    };
  }
}

#endif /* TEXTURE_DEBUG_RENDERABLE_H_ */
