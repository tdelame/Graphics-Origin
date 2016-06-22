# include "../../graphics-origin/application/qt_application/renderer.h"
# include "../../graphics-origin/application/gl_helper.h"
# include "../../graphics-origin/tools/log.h"
# include <GL/glew.h>

namespace graphics_origin {
  namespace application {
    namespace qt {

      bool checkFramebufferStatus()
      {
          // check FBO status
          GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
          switch(status)
          {
          case GL_FRAMEBUFFER_COMPLETE:
              LOG( debug, "Framebuffer complete.");
              return true;

          case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            LOG( debug,  "[ERROR] Framebuffer incomplete: Attachment is NOT complete." );
              return false;

          case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            LOG( debug,  "[ERROR] Framebuffer incomplete: No image is attached to FBO." );
              return false;
      /*
          case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
              std::cout << "[ERROR] Framebuffer incomplete: Attached images have different dimensions." << std::endl;
              return false;

          case GL_FRAMEBUFFER_INCOMPLETE_FORMATS:
              std::cout << "[ERROR] Framebuffer incomplete: Color attached images have different internal formats." << std::endl;
              return false;
      */
          case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            LOG( debug,  "[ERROR] Framebuffer incomplete: Draw buffer." );
              return false;

          case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            LOG( debug,  "[ERROR] Framebuffer incomplete: Read buffer." );
              return false;

          case GL_FRAMEBUFFER_UNSUPPORTED:
            LOG( debug,  "[ERROR] Framebuffer incomplete: Unsupported by FBO implementation.");
              return false;

          default:
            LOG( debug,  "[ERROR] Framebuffer incomplete: Unknown error.");
              return false;
          }
      }


      void renderer::build_textures()
      {
         glcheck(glGenTextures(2, color_textures));

         glcheck(glBindTexture(GL_TEXTURE_2D, color_textures[0] ));
         glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
         glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
         glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
         glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
         glcheck(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, 0));


         glcheck(glBindTexture(GL_TEXTURE_2D, color_textures[1] ));
         glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
         glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
         glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
         glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
         glcheck(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, 0));

         glcheck(glBindTexture(GL_TEXTURE_2D, 0));
      }

      void renderer::build_render_buffers()
      {
        glcheck(glGenRenderbuffers( 1, &depth_render_buffer ));
        glcheck(glBindRenderbuffer( GL_RENDERBUFFER, depth_render_buffer ));
        glcheck(glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height ));
        glcheck(glBindRenderbuffer( GL_RENDERBUFFER, 0 ));
      }

      void renderer::build_frame_buffer()
      {
        glcheck(glGenFramebuffers( 1, &frame_buffer_object ));

      }

      void renderer::complete_frame_buffer()
      {
        glcheck(glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_object ));
        glcheck(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_textures[0], 0 ));
        glcheck(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, color_textures[1], 0 ));
        glcheck(glBindRenderbuffer( GL_RENDERBUFFER, depth_render_buffer ));
        glcheck(glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_render_buffer ));

        glcheck(glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height ));

        glcheck(glBindRenderbuffer( GL_RENDERBUFFER, 0 ));

        glcheck(glViewport( 0, 0, width, height ));
        glcheck(glClearColor(1.0, 1.0, 1.0, 1.0 ));
      }

      void renderer::setup_opengl_objects()
      {
        build_textures();
        build_render_buffers();
        build_frame_buffer();
        complete_frame_buffer();
        checkFramebufferStatus();

      }

      void renderer::destroy_textures()
      {
       glcheck(glDeleteTextures( 2, color_textures ));
      }

      void renderer::destroy_render_buffers()
      {
       glcheck(glDeleteRenderbuffers( 1, &depth_render_buffer ));
      }

      void renderer::destroy_frame_buffer()
      {
       glcheck(glBindFramebuffer(GL_FRAMEBUFFER, 0 ));
       glcheck(glDeleteFramebuffers( 1, &frame_buffer_object ));
      }

      void renderer::destroy_opengl_objects()
      {
       destroy_textures();
       destroy_render_buffers();
       destroy_frame_buffer();
      }

      void renderer::render_gl()
      {
        glcheck(glBindFramebuffer( GL_FRAMEBUFFER, frame_buffer_object ));
        glcheck(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + current_color_texture,
                                       GL_TEXTURE_2D, color_textures[current_color_texture],0));
        glcheck(glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ));
          do_render();
          glcheck(glFlush());
        glcheck(glBindFramebuffer( GL_FRAMEBUFFER, 0 ));
      }
    }
  }
}



