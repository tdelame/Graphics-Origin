/*  Created on: Feb 14, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */
# include "../../graphics-origin/application/gl_window_renderer.h"
# include "../../graphics-origin/application/gl_window.h"
# include "../../graphics-origin/application/renderable.h"
BEGIN_GO_NAMESPACE 
namespace application {

  class simple_gl_renderer
    : public gl_window_renderer {
  public:


  private:
    void do_add( renderable* r )
    {
      m_renderables.push_back( r );
    }

    void do_render()
    {
      for( auto& r : m_renderables )
        r->

    }

    std::list< renderable* > m_renderables;
  };

}
END_GO_NAMESPACE


int main( int argc, char* argv[] )
{

  return 0;
}
