/*  Created on: Jan 29, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */

# ifndef GRAPHICS_ORIGIN_GL_WINDOW_RENDERER_H_
# define GRAPHICS_ORIGIN_GL_WINDOW_RENDERER_H_
# include <graphics-origin/graphics_origin.h>

# include <atomic>
# include <list>
# include <condition_variable>

# include <QObject>
# include <QtCore/QThread>
# include <QSize>

class QOffscreenSurface;
class QOpenGLContext;
class QOpenGLFramebufferObject;

BEGIN_GO_NAMESPACE
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

  public slots:
    void render_next();
  signals:
    void texture_ready( int id, const QSize& size );

  private slots:
    void shut_down();
  private:
    friend gl_window;

    QOffscreenSurface* m_surface;
    QOpenGLContext* m_context;
    QOpenGLFramebufferObject* m_render_fbo;
    QOpenGLFramebufferObject* m_display_fbo;

    camera* m_camera;
    std::mutex m_lock;
    std::condition_variable m_cv;
    std::atomic_char m_size_changed;
    unsigned char m_is_running;
    unsigned int m_width;
    unsigned int m_height;
  };

}
END_GO_NAMESPACE
# endif
