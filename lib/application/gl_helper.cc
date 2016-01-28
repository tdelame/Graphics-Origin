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
}
END_GO_NAMESPACE
