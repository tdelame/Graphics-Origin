# ifndef GRAPHICS_ORIGIN_QT_APPLICATION_RENDERER_H_
# define GRAPHICS_ORIGIN_QT_APPLICATION_RENDERER_H_
# include "../../graphics_origin.h"
# include "../../geometry/matrix.h"

# include <atomic>
# include <condition_variable>

# include <QObject>

class QOffscreenSurface;
class QOpenGLContext;
class QOpenGLFramebufferObject;
class QSize;

namespace graphics_origin {
  namespace application {
    class camera;
    class renderable;
    namespace qt {
      class window;
      class renderer :
          public QObject {
        Q_OBJECT
      public:
        renderer();
        virtual ~renderer();

        void add( renderable* r );
        void set_size( const real& width, const real& height );
        void pause();
        void resume();

        const gpu_mat4& get_view_matrix() const;
        void set_view_matrix( const gpu_mat4& new_view_matrix );

        const gpu_mat4& get_projection_matrix() const;

        gpu_vec2 get_window_dimensions() const;

      public slots:
        void render_next();
      signals:
        void texture_ready( int id, const QSize& size );

      private slots:
        void shut_down();
      protected:
        friend window;

        virtual void do_add( renderable* r ) = 0;
        virtual void do_render() = 0;
        virtual void do_shut_down() = 0;

        QOffscreenSurface* surface;
        QOpenGLContext* context;
        QOpenGLFramebufferObject* render_fbo;
        QOpenGLFramebufferObject* display_fbo;
        QOpenGLFramebufferObject* downsampled_fbo;

        /**Note: the camera is not deleted in the destructor. */
        camera* gl_camera;
        std::mutex lock;
        std::condition_variable cv;
        std::atomic_char size_changed;
        unsigned char is_running;
        unsigned int width;
        unsigned int height;
      };
    }
  }
}

# endif 
