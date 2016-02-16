/* Created on: Feb 15, 2016
 *     Author: T.Delame (tdelame@gmail.com)
 */
# include <graphics-origin/application/renderable.h>

BEGIN_GO_NAMESPACE
namespace application {

  renderable::renderable()
    : m_dirty{ true }
  {}

  renderable::~renderable()
  {}

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

  shader_program_ptr renderable::get_shader_program() const
  {
    return m_program;
  }
}
END_GO_NAMESPACE
