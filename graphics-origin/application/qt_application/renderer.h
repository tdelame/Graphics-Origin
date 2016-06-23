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

      /**@brief Render a 3D scene in its own thread.
       *
       * The design of this renderer kept me busy many days. I really wanted
       * to have multisampling and vsync, as well as the total control over
       * my OpenGL 4 context. However, I was never able to achieve vsync.
       * Also, vsync may not be something we really want to, or can, achieve.
       * So, instead of continuing to fight with the design of this class,
       * Qt, the driver and the OS, I let the code like it is now. If you find
       * an explanation, at the code level, for the vertical tearing that can
       * happen, please let me know.
       *
       * This renderer will render a surface to a multisampled FBO in its own
       * thread. Then, this FBO is blitted to a normal FBO and the resulting
       * texture is sent to a texture node to be rendered by the Qt Quick
       * Scene Graph. In case you worry that the normal texture can be read
       * by the scene graph and written to by the renderer at the same time
       * (meaning that the renderer takes less time to render a 3D scene than
       * the scene graph to render the interface), you can compile with the
       * flag GO_QT_RENDERER_USE_DOUBLE_NORMAL_TEXTURES. In that case, the
       * renderer will blit alternatively to one of the two normal FBOS.
       */
      class renderer :
          public QObject {
        Q_OBJECT
      public:
        renderer();
        virtual ~renderer();

        /**@name Useful values for shader program uniforms.
         * Those methods can be used to fill the values of shader program uniforms.
         * They are accessible thanks to the field \c renderer inside the renderable
         * class.
         * @{*/
        gpu_vec2 get_window_dimensions() const;
        const gpu_mat4& get_view_matrix() const;
        const gpu_mat4& get_projection_matrix() const;
        ///@}

      public slots:
        void render_next();
      signals:
        void texture_ready( int id, const QSize& size );

      private slots:
        void shut_down();
      protected:
        friend window;

        /**@name Interface for the window class
         * Only the window lass will call those methods. You do not have
         * (should) call them yourself.
         * @{*/
        void add( renderable* r );
        void set_surface_dimensions( const real& width, const real& height );
        void pause();
        void resume();
        void set_samples( int samples );
        int get_samples() const;
        ///@}

        /**@name Renderer's interface
         * You have to implement those virtual functions to do the actual
         * work of a renderer.
         * @{*/
        virtual void do_add( renderable* r ) = 0;
        virtual void do_render() = 0;
        virtual void do_shut_down() = 0;
        ///@}

        void render_gl();
        void transfer_to_normal_fbo();

        /**@name OpenGL setup/build
         * Create and setup OpenGL objects. This is done by different methods
         * because it is impossible to have Qt and OpenGL raw calls in the same
         * source code (due to the includes).
         * @{
         */
        void build_textures();
        void build_render_buffers();
        void build_frame_buffer();
        void complete_frame_buffer();
        void setup_opengl_objects();
        ///@}

        /**@name OpenGL destruction
         * Destroy OpenGL objects. This is done by different methods
         * because it is impossible to have Qt and OpenGL raw calls in the same
         * source code (due to the includes).
         * @{
         */
        void destroy_textures();
        void destroy_render_buffers();
        void destroy_frame_buffer();
        void destroy_opengl_objects();
        ///@}

        QOffscreenSurface* surface;
        QOpenGLContext* context;

# ifdef GO_QT_RENDERER_USE_DOUBLE_NORMAL_TEXTURES
        enum { multisampled, normal, normalbis, number_of_fbos };
        uint current_render_texture;
# else
        enum { multisampled, normal, number_of_fbos };
# endif
        uint frame_buffer_objects[number_of_fbos];
        uint color_textures[number_of_fbos];
        uint depth_render_buffer;

        /**Note: the camera is not deleted in the destructor. */
        camera* gl_camera;
        std::mutex lock;
        std::condition_variable cv;
        std::atomic_char size_changed;
        unsigned char is_running;
        uint width;
        uint height;
        int samples;
      };
    }
  }
}

# endif 
