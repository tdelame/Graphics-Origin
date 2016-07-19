/*  Created on: Mar 29, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */
# ifndef TUTORIAL_SIMPLE_GL_RENDERER_H_
# define TURORIAL_SIMPLE_GL_RENDERER_H_
# include "../../../graphics-origin/application/camera.h"
# include "../../../graphics-origin/application/renderer.h"
# include "../../../graphics-origin/application/renderable.h"
# include <list>

namespace graphics_origin { namespace application {

  class simple_gl_renderer
    : public graphics_origin::application::renderer {
  public:
    ~simple_gl_renderer(){}

  private:
    void do_add( graphics_origin::application::renderable* r ) override
    {
      m_renderables.push_back( r );
    }

    void do_render() override
    {
      gl_camera->update();
      for( auto& r : m_renderables )
        {
          r->get_shader_program()->bind();
          r->render();
        }
    }

    void do_shut_down() override
    {
      while( !m_renderables.empty() )
        {
          auto r = m_renderables.front();
          delete r;
          m_renderables.pop_front();
        }
    }

    std::list< graphics_origin::application::renderable* > m_renderables;
  };
}}

# endif
