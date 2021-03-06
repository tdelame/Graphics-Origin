/* Created on: Feb 15, 2016
 *     Author: T.Delame (tdelame@gmail.com)
 */
# ifndef GRAPHICS_ORIGIN_RENDERABLE_H_
# define GRAPHICS_ORIGIN_RENDERABLE_H_
# include "../graphics_origin.h"
# include "../application/shader_program.h"
# include "../geometry/matrix.h"

BEGIN_GO_NAMESPACE
namespace application {

  class renderer;

  /** Interface class to render an object with OpenGL.
   *
   * OpenGL related operations should be done when the right context is bound
   * and when the right shader program is applied, and only in such situation.
   * To enforce this behavior, any OpenGL command should be issued in one of
   * the following function:
   *
   * - update_gpu_data(), which is called when the flag \a m_dirty at the beginning
   * of the function render(). This function is used to transmit / update any
   * data on the GPU.
   *
   * - do_render(), which implements the actual rendering of an instance
   *
   * - remove_gpu_data(), called at the destruction of an instance. Note that this
   * function cannot be called in renderable::~renderable. Indeed, the destructor
   * is itself virtual such that an instance of a final class can be deleted by a
   * pointer to a renderable. Thus, the behavior of calling a pure virtual function
   * in the virtual destructor is unspecified by the standard. You *SHOULD* call this
   * function yourself in the final class destructor.
   */
  class GO_API renderable {
  public:
    renderable();
    virtual ~renderable();
    void render();

    /**@brief Set a new shader program for this renderable.
     *
     * Set a new shader program for this renderable.
     * @param new_program The new shader program. */
    void set_shader_program( shader_program_ptr new_program );

    /**@brief Get the shader program currently used by this renderable.
     *
     * Read-only access to the shader program used by this renderable.
     * @return The shader program. */
    shader_program_ptr get_shader_program() const;

    /**@brief Get the model matrix of this renderable.
     *
     * Read-only access to the model matrix of this renderable. This matrix
     * position the renderable relatively to the world, i.e. it transforms
     * local coordinates to world coordinates.
     * @return The model matrix.
     */
    const gl_mat4& get_model_matrix() const;

    /**@brief Set the model matrix of this renderable.
     *
     * Write the model matrix of this renderable. If you write an incorrect
     * model matrix, you might be unable to see the renderable on the screen.
     * @param new_model The new model matrix.
     */
    void set_model_matrix( const gl_mat4& new_model );

    /**@brief Notify a modification in data used for the rendering.
     *
     * Notify the renderable that the data used for its rendering has been
     * changed and thus require an update on the gpu.
     */
    void set_dirty();
  private:
    /**@brief Update the data on the gpu.
     *
     * This function is called when \a m_dirty is true during a rendering.
     * Its purpose is to set and transmit data to the GPU to be used for
     * the rendering. */
    virtual void update_gpu_data() = 0;

    /**@brief Rendering implementation.
     *
     * Implement the rendering of a Renderable sub-class. The context is
     * guaranteed to be bound, the shader program to be applied and the
     * gpu data to be up-to-date. */
    virtual void do_render() = 0;

    /**@brief Remove data from the GPU.
     *
     * This function clean the data from the GPU when an instance is
     * destroyed. This function should be called in the destructor of the
     * final class. */
    virtual void remove_gpu_data() = 0;
  protected:
    /** This friend declaration is to set the renderer that will manage this
     * renderable. It could have been done thanks to a public method, but I
     * do not want other classes than renderer to be able to change the
     * renderer.*/
    friend class renderer;

    /**@brief Model matrix.
     *
     * This matrix store the transformation to go from the local coordinates
     * to the world coordinates.
     */
    gl_mat4 model;
    /**@brief Shader program to render the instance.
     *
     * This shader program specify the graphics operations that will take place
     * on the GPU to render an instance from its data.
     */
    shader_program_ptr program;
    /**@brief Tell if the GPU data is up-to-date.
     *
     * When set to false, the function update_gpu_data() is called at the next
     * rendering. */
    bool m_dirty;

    /** Renderer.
     *
     * If non null, this points to the renderer that will render this renderable.
     * It could be used to access to the camera of the renderer and to other
     * functionalities.
     */
    renderer* renderer_ptr;
  };

}
END_GO_NAMESPACE
# endif 
