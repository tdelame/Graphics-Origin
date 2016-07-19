# include <graphics-origin/application/renderable.h>
BEGIN_GO_NAMESPACE
namespace application {

  renderable::renderable()
    : m_dirty{ true }, renderer_ptr{ nullptr }
  {}

  renderable::~renderable()
  {}

  void
  renderable::set_shader_program( shader_program_ptr new_program )
  {
    program = new_program;
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
    return model;
  }

  void renderable::set_model_matrix( const gpu_mat4& new_model )
  {
    model = new_model;
  }

  shader_program_ptr renderable::get_shader_program() const
  {
    return program;
  }
}
END_GO_NAMESPACE
