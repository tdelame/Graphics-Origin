/* Created on: Feb 15, 2016
 *     Author: T.Delame (tdelame@gmail.com)
 */
# ifndef GRAPHICS_ORIGIN_3_SIMPLE_GL_APPLICATION_H_
# define GRAPHICS_ORIGIN_3_SIMPLE_GL_APPLICATION_H_

# include "../../graphics-origin/application/gl_window_renderer.h"
# include "../../graphics-origin/application/renderable.h"

# include <QtQuick/QQuickView>

namespace graphics_origin {
namespace application {

  class simple_gl_renderer
    : public gl_window_renderer {
  public:
    ~simple_gl_renderer();

  private:
    void do_add( renderable* r );
    void do_render();
    void do_shut_down();

    std::list< renderable* > m_renderables;
  };

  class test_application :
      public QQuickView {
    Q_OBJECT
  public:
    explicit test_application( QWindow* parent = nullptr );
    virtual ~test_application();
  };
}}
# endif 
