/* Created on: Feb 15, 2016
 *     Author: T.Delame (tdelame@gmail.com)
 */
# include <graphics-origin/application/renderable.h>

BEGIN_GO_NAMESPACE
namespace application {

  renderable::renderable()
    : m_dirty{ true }, m_renderer{ nullptr }
  {}

  renderable::~renderable()
  {}

  void
  renderable::set_renderer( gl_window_renderer* renderer )
  {
    m_renderer = renderer;
  }

  void
  renderable::set_shader_program( shader_program_ptr program )
  {
    m_program = program;
    set_dirty();
  }

  void
  renderable::set_dirty()
  {
    m_dirty = true;
  }

  void
  renderable::render()
  {
    if( m_dirty )
      {
        update_gpu_data();
        m_dirty = false;
      }
    do_render();
  }

  const gpu_mat4& renderable::get_model_matrix() const
  {
    return m_model;
  }

  void renderable::set_model_matrix( const gpu_mat4& model )
  {
    m_model = model;
  }

  shader_program_ptr renderable::get_shader_program() const
  {
    return m_program;
  }
}
END_GO_NAMESPACE
