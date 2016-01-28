/*  Created on: Jan 26, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */

# ifndef GRAPHICS_ORIGIN_SHADER_PROGRAM_H_
# define GRAPHICS_ORIGIN_SHADER_PROGRAM_H_
# include "../graphics_origin.h"

# include <list>
# include <string>
# include <unordered_map>

BEGIN_GO_NAMESPACE
namespace application {

  /**
   * The application will have more than one OpenGL context. There will be at
   * least one context used and managed by Qt, and one context by GL Window. All
   * openGL command must be issued when the correct context is bound. We know
   * for sure the context of a GL Window is bound when the draw function is
   * called. If we want to change the current context at any time, we could
   * change it when it is needed by other part of the code and thus create a big
   * mess. Thus, we should take care than any openGL command is issued only *in*
   * the draw function of the GL window.
   *
   * This is not particularly constraining, as GL commands are meant to occur
   * during rendering, except for the shader program construction, source
   * loading and reloading. One solution might be to only set the shader source
   * filenames, then to perform the compiling and linking when the shader
   * program is bound during the rendering. This means to check a flag at every
   * bind(). It could be sub-optimal to check such flag every time. However,
   * the alternative is to construct, load and reload a shader only during
   * rendering. This might be limiting, e.g.: if a button of the GUI allows to
   * load dynamically sources into a shader program, the filenames has to be
   * stored somewhere and then passed to the constructor of a shader program at
   * rendering. Thus, I chose the first solution: a little sub-optimal but less
   * constraints.
   */

  class shader_program {
  public:
    typedef int identifier;
    static const identifier null_identifier;

    /** Construct a shader program from the specified file names.
     *
     * The shader types are identified by the extension of their source
     * filenames:
     *  - .cs for compute shader
     *  - .vert for vertex shader
     *  - .tcs for tesselation control shader
     *  - .tes for tesselation evaluation shader
     *  - .geom for geometry shader
     *  - .frag for fragment shader
     *
     * If the shaders are invalid or describe an invalid program, this is
     * initialized to the null shader program.
     */
    shader_program( const std::list< std::string >& shader_sources );
    ~shader_program();

    /** Load a shader from source filenames.
     *
     * The shader types are identified by the extension of their source
     * filenames (see shader_program() for more details).
     */
    void load( const std::list< std::string >& shader_sources );
    /** Reload the shader sources.
     *
     * This is useful is you decide to modify the shader sources while
     * executing your program. This way, you can check, improve, debug
     * shaders and see the results immediately on screen. If this shader
     * program was invalid, it will still be the null program after the
     * execution of this function.
     */
    void reload();

    /** Bind this program to the GPU.
     *
     * This is a necessary operation to render objects or to send
     * uniforms/attributes values.
     *
     * \note The shader sources compilation and linking are actually done in
     * this function. Indeed, this function is typically called during the
     * GL Window rendering (and should be called only during this operation),
     * thus when the correct GL context is bound. This is then a good time
     * to issue all GL commands that we couldn't do during the constructor,
     * the load() and reload() functions.
     */
    void bind();

    /** Unbind any shader program.
     */
    static void unbind();

    /** Get the location of an uniform in the program.
     *
     * This function is less expansive than an actual call to the GL API
     * to get the uniform location. Indeed, all existing uniform of the program
     * are analyzed at construction and their location are stored in a look-up
     * table. Thus, only CPU code is executed for this function.
     *
     * @param name The uniform name, as it appear in the shader sources (be
     * aware of the naming convention for blocks and arrays, see
     * https://www.opengl.org/wiki/Program_Introspection#Naming)
     * @return The uniform location or null_identifier is there is no uniform
     * with such name in this program.
     */
    identifier get_uniform_location( const std::string& name ) const;

    /** Get the location of an attribute in the program.
     *
     * This function is less expansive than an actual call to the GL API
     * to get the attribute location. Indeed, all existing attribute of the program
     * are analyzed at construction and their location are stored in a look-up
     * table. Thus, only CPU code is executed for this function.
     *
     * @param name The attribute location, as it appear in the shader sources.
     * @return The attribute location or null_identifier is there is no attribute
     * with such name in the program.
     */
    identifier get_attribute_location( const std::string& name ) const;

    operator unsigned int() const;

  private:
    void variables_introspection();

    std::list< std::string > m_source_filenames;
    std::unordered_map< std::string, identifier > m_uniforms;
    std::unordered_map< std::string, int > m_attributes;
    unsigned int m_program_id;
    bool m_loaded;
  };
}
END_GO_NAMESPACE
# endif
