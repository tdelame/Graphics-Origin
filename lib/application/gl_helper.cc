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
        LOG_WITH_LINE_FILE( error, "[GL] previous error discovered: "
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
        LOG_WITH_LINE_FILE( error, "[GL] Error discovered when calling "
          << call << ": " << gluErrorString(error), line, file );
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
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );
  }
}
END_GO_NAMESPACE
