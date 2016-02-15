/* Created on: Jan 28, 2016
 *     Author: T.Delame (tdelame@gmail.com)
 */
# ifndef GRAPHICS_ORIGIN_GL_WINDOW_H_
# define GRAPHICS_ORIGIN_GL_WINDOW_H_
# include "../graphics_origin.h"

# include <QQuickItem>

namespace graphics_origin {
namespace application {
  class gl_window_renderer;
  class renderable;
  /**
   *
   *
   * The design for this class is adapted from this article:
   * http://doc.qt.io/qt-5/qtquick-scenegraph-openglunderqml-example.html
   */
  class gl_window :
      public QQuickItem {
    Q_OBJECT
    Q_PROPERTY( QObject* camera READ get_camera WRITE set_camera )
  public:
    gl_window( QQuickItem* parent = nullptr );
    ~gl_window();

    Q_INVOKABLE
    void initialize_renderer( gl_window_renderer* renderer );

    static QList< gl_window* > g_gl_windows;

  public Q_SLOTS:
    void ready();
    void sync();
    void cleanup();

    Q_INVOKABLE void pause();
    Q_INVOKABLE void resume();

    QObject* get_camera() const;
    void set_camera(QObject* cam);
    void add_renderable( renderable* r );

  protected:
    QSGNode* updatePaintNode( QSGNode*, UpdatePaintNodeData* ) override;
  private slots:
    void handle_size_changed();
    void handle_window_changed( QQuickWindow* win );

  protected:
    gl_window_renderer* m_renderer;
  };


}
END_GO_NAMESPACE
# endif 
