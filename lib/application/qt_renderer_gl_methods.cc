# include "../../graphics-origin/application/qt_application/renderer.h"
# include "../../graphics-origin/application/gl_helper.h"
# include "../../graphics-origin/tools/log.h"
# include <GL/glew.h>
# include <FreeImage.h>

namespace graphics_origin {
  namespace application {
    namespace qt {

      void renderer::build_textures()
      {
         glcheck(glGenTextures(number_of_fbos, color_textures));
         glcheck(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, color_textures[multisampled] ));
         glcheck(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA, width, height, GL_TRUE ));
         glcheck(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0 ));

         glcheck(glBindTexture(GL_TEXTURE_2D, color_textures[normal] ));
         glcheck(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, 0));
         glcheck(glBindTexture(GL_TEXTURE_2D, 0));
      }

      void renderer::build_render_buffers()
      {
        glcheck(glGenRenderbuffers( 1, &depth_render_buffer ));
        glcheck(glBindRenderbuffer( GL_RENDERBUFFER, depth_render_buffer ));
        glcheck(glRenderbufferStorageMultisample( GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, width, height ));
        glcheck(glBindRenderbuffer( GL_RENDERBUFFER, 0 ));
      }

      void renderer::build_frame_buffer()
      {
        glcheck(glGenFramebuffers( number_of_fbos, frame_buffer_objects ));
      }

      void renderer::complete_frame_buffer()
      {
        glcheck(glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_objects[multisampled] ));

        glcheck(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, color_textures[multisampled], 0 ));
        glcheck(glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_render_buffer ));

        glcheck(glViewport( 0, 0, width, height ));
        glcheckfbo(frame_buffer_objects[multisampled]);
        glcheck(glBindFramebuffer(GL_FRAMEBUFFER, 0 ));

        glcheck(glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_objects[normal] ));

        glcheck(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_textures[normal], 0 ));
        glcheckfbo(frame_buffer_objects[normal]);
        glcheck(glBindFramebuffer(GL_FRAMEBUFFER, 0 ));
      }

      void renderer::setup_opengl_objects()
      {
        build_textures();
        build_render_buffers();
        build_frame_buffer();
        complete_frame_buffer();
      }

      void renderer::destroy_textures()
      {
       glcheck(glDeleteTextures( number_of_fbos, color_textures ));
      }

      void renderer::destroy_render_buffers()
      {
       glcheck(glDeleteRenderbuffers( 1, &depth_render_buffer ));
      }

      void renderer::destroy_frame_buffer()
      {
       glcheck(glBindFramebuffer(GL_FRAMEBUFFER, 0 ));
       glcheck(glDeleteFramebuffers( number_of_fbos, frame_buffer_objects ));
      }

      void renderer::destroy_opengl_objects()
      {
       destroy_textures();
       destroy_render_buffers();
       destroy_frame_buffer();
      }

      void renderer::render_gl()
      {
        glcheck(glBindFramebuffer( GL_FRAMEBUFFER, frame_buffer_objects[multisampled] ));
        glcheck(glClearColor(1.0, 1.0, 1.0, 1.0 ));
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glcheck(glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ));
          do_render();
          // with glFlush or glFinish, nothing change: we still have tearing and thus no vsync.
//          glcheck(glFlush());
//          glFinish();
      }

      void renderer::transfer_to_normal_fbo()
      {
        glcheck(glBlitNamedFramebuffer(
            frame_buffer_objects[multisampled], frame_buffer_objects[normal],
            0,0,width,height,0,0,width,height, GL_COLOR_BUFFER_BIT, GL_NEAREST));
        glcheck(glBindFramebuffer( GL_FRAMEBUFFER, 0 ));
      }
    }
  }
}



