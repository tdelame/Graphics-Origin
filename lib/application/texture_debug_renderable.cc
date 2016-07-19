/*  Created on: Jun 11, 2016
 *      Author: T. Delame (tdelame@gmail.com) */
# include "../../graphics-origin/application/renderables/texture_debug_renderable.h"
# include "../../graphics-origin/application/gl_helper.h"
# include "../../graphics-origin/tools/filesystem.h"
# include "../../graphics-origin/tools/log.h"

# include <GL/glew.h>

# include <FreeImage.h>

namespace graphics_origin {
  namespace application {

    texture_debug_renderable::texture_debug_renderable(
      shader_program_ptr program )
      : m_fib{ nullptr }, m_texture_id{ 0 }, m_next_texture_id{ 0 },
        m_visible{ true }, m_owner{ false }
    {
      this->program = program;
    }

    texture_debug_renderable::~texture_debug_renderable()
    {
      unload_fib();
      remove_gpu_data();
    }

    void texture_debug_renderable::unload_fib()
    {
      if( m_fib )
        {
          FreeImage_Unload( m_fib );
          m_fib = nullptr;
        }
    }

    void texture_debug_renderable::set_visible( bool visible )
    {
      m_visible = visible;
    }

    void texture_debug_renderable::set_texture( const std::string& texture_file_name )
    {
      if( tools::file_exist( texture_file_name ) )
        {
          FREE_IMAGE_FORMAT fif = FreeImage_GetFileType( texture_file_name.c_str(), 0 );
          if( fif == FIF_UNKNOWN )
            fif = FreeImage_GetFIFFromFilename( texture_file_name.c_str() );
          if( fif != FIF_UNKNOWN )
            {
              if( FreeImage_FIFSupportsReading( fif ) )
                {
                  lock.lock();
                    unload_fib();
                    m_fib = FreeImage_Load( fif, texture_file_name.c_str() );
                    if( FreeImage_GetBPP( m_fib ) != 32 )
                      {
                        FIBITMAP* fib32 = FreeImage_ConvertTo32Bits( m_fib );
                        FreeImage_Unload( m_fib );
                        m_fib = fib32;
                      }

                    // will upload the texture to the GPU at the next render
                    set_dirty();
                  lock.unlock();
                }
              else
                {
                  LOG( error, "no loader to read image file [" << texture_file_name << "]");
                }
            }
          else
            {
              LOG( error, "file [" << texture_file_name << "] is an unknown image file type" );
            }
        }
      else
        {
          LOG( debug, "file [" << texture_file_name << "] does not exist");
        }
    }

    void texture_debug_renderable::set_texture( unsigned int gl_texture_id )
    {
      lock.lock();
        m_next_texture_id = gl_texture_id;
        set_dirty();
      lock.unlock();
    }

    void texture_debug_renderable::update_gpu_data()
    {
      remove_gpu_data();

      lock.lock();
        // the new texture will be created from an image file
        if( m_fib )
          {
            // generate a new texture id, managed by this
            glcheck(glGenTextures( 1, &m_texture_id ));
            m_owner = true;

            // load the texture and its options
            glcheck(glActiveTexture(GL_TEXTURE0));
            glcheck(glBindTexture(GL_TEXTURE_2D, m_texture_id));
            glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
            glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
            glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
            glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
            glcheck(glTexImage2D( GL_TEXTURE_2D,
              0, GL_RGBA32F, FreeImage_GetWidth(m_fib), FreeImage_GetHeight(m_fib), 0,
              GL_BGRA, GL_UNSIGNED_BYTE, FreeImage_GetBits(m_fib)));
            glcheck(glBindTexture(GL_TEXTURE_2D, 0));

            // clean the image internal representation
            unload_fib();
          }
        // the new texture is already on the gpu
        else
          {
            m_texture_id = m_next_texture_id;
            m_next_texture_id = 0;
            m_owner = false;
          }
      lock.unlock();
      ///fixme: not 100% thread-safe. If at this point, set_texture() is called and set_dirty()
      /// return before the "m_dirty = false;" line executed just after update_gpu_data() in
      /// renderable::render(), we will miss a texture update. This case occurs only when two
      /// texture changes are requested between two frames.
    }

    void texture_debug_renderable::remove_gpu_data()
    {
      // the current texture id was created by this, we should delete it
      if( m_texture_id && m_owner )
        glcheck(glDeleteTextures( 1, &m_texture_id));
    }

    void texture_debug_renderable::do_render()
    {
      glcheck(glBindTexture( GL_TEXTURE_2D, m_texture_id ));
      glcheck(glUniform1i( program->get_uniform_location( "sampler" ), 0 ));
      glcheck(glDrawArrays( GL_POINTS, 0, 1 ));
    }
  }
}

