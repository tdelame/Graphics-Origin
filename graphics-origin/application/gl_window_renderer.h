/*  Created on: Jan 29, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */
# ifndef GRAPHICS_ORIGIN_GL_WINDOW_RENDERER_H_
# define GRAPHICS_ORIGIN_GL_WINDOW_RENDERER_H_
# include "../graphics_origin.h"
# include "../geometry/matrix.h"
# include "../tools/fps_counter.h"

# include <atomic>
# include <list>
# include <condition_variable>

# include <QObject>
# include <QtCore/QThread>
# include <QSize>

class QOffscreenSurface;
class QOpenGLContext;
class QOpenGLFramebufferObject;

namespace graphics_origin {
namespace application {
  class camera;
  class gl_window;
  class renderable;

  class gl_window_renderer
    : public QThread {
    Q_OBJECT
  public:
    gl_window_renderer();
    virtual ~gl_window_renderer();

    void add( renderable* r );
    void set_size( const real& width, const real& height );
    void pause();
    void resume();

    const gpu_mat4&
    get_view_matrix() const;

    void
    set_view_matrix( const gpu_mat4& view );

    const gpu_mat4&
    get_projection_matrix() const;

    real get_fps() const;

    ///TODO: temp
    const camera* get_camera() const
    {
      return m_camera;
    }

    gpu_vec2 get_window_dimensions() const;

  public slots:
    void render_next();
  signals:
    void texture_ready( int id, const QSize& size );

  private slots:
    void shut_down();
  protected:
    friend gl_window;

    virtual void do_add( renderable* r ) = 0;
    virtual void do_render() = 0;
    virtual void do_shut_down() = 0;

    QOffscreenSurface* m_surface;
    QOpenGLContext* m_context;
    QOpenGLFramebufferObject* m_render_fbo;
    QOpenGLFramebufferObject* m_display_fbo;

    /**Note: the camera is not deleted in the destructor. */
    camera* m_camera;
    std::mutex m_lock;
    std::condition_variable m_cv;
    std::atomic_char m_size_changed;
    unsigned char m_is_running;
    unsigned int m_width;
    unsigned int m_height;
    tools::fps_counter m_fps_counter;
    real m_cached_fps;
  };

}
END_GO_NAMESPACE
# endif
