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

      /**
       *
       *
       *
       *
       * @note The renderer is not vsync'ed. I do not understand why.
       * The texture used by the geometry node is not filled by the renderer
       * and vsync is *normally* activated for the scene graph.
       *
       * If we cannot have a vsync'ed render to two textures, there is no
       * use of a threaded renderer.
       *
       *
       */
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

        void set_samples( int samples );
        int get_samples() const;

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

        void render_gl();
        void transfer_to_normal_fbo();

        void build_textures();
        void build_render_buffers();
        void build_frame_buffer();
        void complete_frame_buffer();
        void setup_opengl_objects();

        void destroy_textures();
        void destroy_render_buffers();
        void destroy_frame_buffer();
        void destroy_opengl_objects();

        QOffscreenSurface* surface;
        QOpenGLContext* context;

        enum { multisampled, normal, number_of_fbos };
        uint frame_buffer_objects[number_of_fbos];
        uint color_textures[number_of_fbos];
        uint depth_render_buffer;

        /**Note: the camera is not deleted in the destructor. */
        camera* gl_camera;
        std::mutex lock;
        std::condition_variable cv;
        std::atomic_char size_changed;
        unsigned char is_running;
        unsigned int width;
        unsigned int height;
        int samples;
      };
    }
  }
}

# endif 
