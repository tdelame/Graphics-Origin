/* Created on: Feb 15, 2016
 *     Author: T.Delame (tdelame@gmail.com)
 */
# ifndef GRAPHICS_ORIGIN_3_SIMPLE_GL_APPLICATION_H_
# define GRAPHICS_ORIGIN_3_SIMPLE_GL_APPLICATION_H_

# include "../../graphics-origin/application/gl_window_renderer.h"
# include "../../graphics-origin/application/renderable.h"
# include "../../graphics-origin/application/camera.h"

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


  class simple_camera
    : public camera {
    Q_OBJECT
  public:
    explicit simple_camera( QObject* parent = nullptr );

    Q_INVOKABLE void set_go_left( bool left );
    Q_INVOKABLE void set_go_right( bool right );
    Q_INVOKABLE void set_go_forward( bool forward );
    Q_INVOKABLE void set_go_backward( bool backward );

  private:
    void do_update();

    gpu_vec3 m_direction;

    real m_update_time;
    bool m_forward;
    bool m_left;
    bool m_right;
    bool m_backward;
    bool m_up;
    bool m_down;
  };

}}
# endif 
