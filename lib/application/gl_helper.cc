/*  Created on: Jan 26, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */
# include <graphics-origin/application/gl_helper.h>
# include <graphics-origin/tools/log.h>
# include <GL/glew.h>

BEGIN_GO_NAMESPACE 
namespace application {
  void
  check_previous_gl_errors()
  {
    GLenum error;
    while( ( error=glGetError() ) != GL_NO_ERROR )
      {
        LOG_WITH_LINE_FILE( error, "[OpenGL] previous error discovered: "
          << gluErrorString(error), __LINE__, __FILE__ );
        error = glGetError();
      }
  }

  void
  check_gl_error( const char* call, const char* file, const int line )
  {
    GLenum error = glGetError();
    if( error != GL_NO_ERROR )
      {
        LOG_WITH_LINE_FILE( error, "[OpenGL] Error discovered when calling "
          << call << ": " << gluErrorString(error), line, file );
      }
  }


  bool check_gl_framebuffer( unsigned int fbo_id, const char* fbo_name, const char* file, const int line )
  {
    switch( glCheckFramebufferStatus( GL_FRAMEBUFFER ))
    {
      case GL_FRAMEBUFFER_COMPLETE:
        LOG( info, "[OpenGL] FBO " << fbo_name << " (" << fbo_id << ") is complete.");
        return true;

      case GL_FRAMEBUFFER_UNDEFINED:
        LOG_WITH_LINE_FILE( error, "[OpenGL] FBO " << fbo_name << " is the default read or draw framebuffer, but the default framebuffer does not exist", line, file );
        return false;

      case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        LOG_WITH_LINE_FILE( error, "[OpenGL] some of FBO " << fbo_name <<"'s attachment points are framebuffer incomplete", line, file );
        return false;

      case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        LOG_WITH_LINE_FILE( error, "[OpenGL] FBO " << fbo_name << " does not have at least one image attachment", line, file );
        return false;

      case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        LOG_WITH_LINE_FILE( error, "[OpenGL] one color attachment of FBO " << fbo_name << " named by GL_DRAW_BUFFERi has GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE=GL_NONE", line, file );
        return false;

      case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
        LOG_WITH_LINE_FILE( error, "[OpenGL] one color attachment of FBO " << fbo_name << "named by GL_READ_BUFFER has GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE=GL_NONE but GL_READ_BUFFER is not GL_NONE", line, file );
        return false;

      case GL_FRAMEBUFFER_UNSUPPORTED:
        LOG_WITH_LINE_FILE( error, "[OpenGL] the combination of internal formats of the image attached to FBO " << fbo_name << " violates an implementation-dependent set of restrictions.", line, file );
        return false;

      case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
        LOG_WITH_LINE_FILE( error, "[OpenGL] for FBO " << fbo_name << " the value of GL_RENDERBUFFER_SAMPLES is not the same for all attached renderbuffers; "
             << "the value of GL_TEXTURE_SAMPLES is the not same for all attached textures; or, the attached images are a mix of renderbuffers and textures, the value of GL_RENDERBUFFER_SAMPLES does not match the value of GL_TEXTURE_SAMPLES. "
             << "It is also possible that the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not the same for all attached textures; or, the attached images are a mix of renderbuffers and textures, the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not GL_TRUE for all attached textures.", line, file );
        return false;

      case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
        LOG_WITH_LINE_FILE( error, "[OpenGL] for FBO " << fbo_name << " one framebuffer attachment is layered, and any populated attachment is not layered, or all populated color attachments are not from textures of the same target.", line, file );
        return false;

      default:
        LOG_WITH_LINE_FILE( error,  "[OpenGL] unknown error for incomplete FBO " << fbo_name, line, file );
        return false;
      }
  }

  void initialize_glew_for_current_context()
  {
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    glGetError();//glewInit generate an invalid gl enum...
    if (err != GLEW_OK )
     {
       LOG( fatal,"[graphics] Failed to initialize GLEW (" << glewGetErrorString(err) <<")")
       exit ( EXIT_FAILURE );
     }
     LOG(info, "GL INFO for current context:")
     LOG(info, "  + GL version = " << glGetString( GL_VERSION))
     LOG(info, "  + GLSL version = " << glGetString(GL_SHADING_LANGUAGE_VERSION) )
     LOG(info, "  + Vendor = " << glGetString(GL_VENDOR) )
     LOG(info, "  + Renderer = " << glGetString(GL_RENDERER) )
  }

  void initialize_current_context()
  {
    glcheck(glEnable( GL_CULL_FACE ));
    glcheck(glCullFace( GL_BACK ));
    glcheck(glEnable(GL_MULTISAMPLE));
    glcheck(glClearColor( 1.0, 1.0, 1.0, 1.0 ));
  }
}
END_GO_NAMESPACE
