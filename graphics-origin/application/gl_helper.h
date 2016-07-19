/*  Created on: Dec 13, 2015
 *      Author: T. Delame (tdelame@gmail.com)
 */

# ifndef GRAPHICS_ORIGIN_GL_HELPER_H_
# define GRAPHICS_ORIGIN_GL_HELPER_H_
# include "../graphics_origin.h"

BEGIN_GO_NAMESPACE
namespace application {

/** @file
 * @brief Helper functions to use OpenGL.
 *
 * This file groups helper functions to use OpenGL. Apart from the glcheck()
 * macro, you are not supposed to directly call any other function of this
 * file.
 */

/** @brief Check if previous GL API calls produced errors.
 *
 * Look if there are previous openGL error on the error stack.
 * If you use the macro glcheck() for all your GL API calls,
 * this function would be useless :-}.
 */
GO_API
void check_previous_gl_errors();

/**
 * @brief Check if a GL API call produce errors and place them in the log.
 */
GO_API
void check_gl_error( const char* call, const char* file, const int line );

/**@brief Check if a GL framebuffer is complete*/
GO_API
bool check_gl_framebuffer( unsigned int fbo_id, const char* fbo_name, const char* file, const int line );

/**
 * @brief This macro should be used for ALL GL API calls!
 *
 * OpenGL command can fail either because the state is not coherent with what
 * you are trying to do or because you sent wrong parameters to a command. In
 * both case, the failure will be silent: no exception is thrown, no log entry
 * is created, you would just have unexpected results on screen. In order to
 * ease the debugging process, we provide a macro that should be used for
 * EVERY openGL command:
 *
 * \code{.cpp}
 * glcheck(glBindBuffer(GL_ARRAY_BUFFER, m_vBuffer));
 * glcheck(glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
 * glcheck(glDrawArrays(GL_TRIANGLES,0, m_vertices.size()));
 * // or
 * glcheck(uint i = glGetProgramResourceIndex( program, GL_SHADER_STORAGE_BLOCK, "data" ));
 * \endcode
 *
 * If a GL command should be issued in a block, always use the brackets to delimit
 * the block:
 * \code{.cpp}
 * // do
 * if( something )
 * {
 *   glcheck( ...command... );
 * }
 * // don't (you will have weird errors)
 * if( something )
 *   glcheck( ...command...);
 * \endcode
 *
 * If an error occurred before the command wrapped in the glcheck() macro, it
 * is reported in the log. In this case, the log entry is not really informative
 * as we do not know after which command any previous error occurred. The macro
 * then executes the command and check for any error. If the command leads to an
 * error, it would be seen in the log, with the command that failed, the error
 * returned, the place in the source code that called this command.
 *
 * \sa LOG()
 *
 * The use of this macro creates some runtime overhead. This is why it does
 * nothing when the source code is compiled in release mode.
 */
# ifdef DEBUG
#   define glcheck( call )                                                 \
  graphics_origin::application::check_previous_gl_errors();                \
  call;                                                                    \
  graphics_origin::application::check_gl_error( #call, __FILE__,__LINE__ );
# else
#   define glcheck( call ) call;
# endif


/**@brief Macro to check the status of a GL frame buffer.
 *
 * OpenGL frame buffer objects (FBO) are quite hard to setup correctly. This
 * macro will check the status of a FBO to tell you (with the return value)
 * if this FBO can be used. If the FBO cannot be used, you will see why in
 * the logs. If your FBO's identifier is stored in the variable fbo_name,
 * simply call:
 * \code{.cpp}
 * bool can_be_used = glcheckfbo( fbo_name );
 * \endcode.
 *
 *
 *
 * \sa LOG()
 */
# define glcheckfbo( name ) \
  graphics_origin::application::check_gl_framebuffer( name, #name, __FILE__, __LINE__ )

/**
 *
 */
void initialize_glew_for_current_context();
void initialize_current_context();

}
END_GO_NAMESPACE

# endif
