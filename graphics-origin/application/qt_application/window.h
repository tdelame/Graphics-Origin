# ifndef GRAPHICS_ORIGIN_QT_APPLICATION_WINDOW_H_
# define GRAPHICS_ORIGIN_QT_APPLICATION_WINDOW_H_
# include "../../graphics_origin.h"
# include <QQuickItem>
# include <QtCore/QThread>

namespace graphics_origin {
  namespace application {
    class renderable;
    namespace qt {
      class renderer;
      /**@brief Display a 3D scene into Qt Quick.
       *
       * This class displays a 3D scene, rendered by OpenGL 4, into a surface
       * put into the Qt Quick's scene graph. It is advised to subclass window
       * in order to initialize your scene in the constructor, as well as to
       * initialize the renderer implementation you want to use. Then, this
       * derived class can be exported to QML.
       *
       * This class has, for now, few features:
       * - adding a renderable to the renderer (no access to those renderable
       * or removal functionalities),
       * - pausing/resuming the rendering (usable in QML)
       * - get/change the camera used by the renderer (\c camera property in QML)
       * - get/change the number of samples for MultiSampling Anti-Aliasing (MSAA)
       * (\c samples property in QML)
       */
      class GO_API window :
          public QQuickItem {
        Q_OBJECT
        Q_PROPERTY( QObject* camera READ get_camera WRITE set_camera )
        Q_PROPERTY( int samples READ get_samples WRITE set_samples )
      public:
        window( QQuickItem* parent = nullptr );
        ~window();

        /**@brief Set the new renderer to render the 3D scene.
         * Set a renderer to render the scene. If a renderer was already
         * used, it will be deleted. This function is likely to be called
         * in the constructor of a subclass of window.
         * @param new_renderer The new renderer implementation to use. */
        Q_INVOKABLE void initialize_renderer( renderer* new_renderer );

        /**@brief Stop the rendering.
         * After this call, no more frames will be produced until the next
         * call to resume(). */
        Q_INVOKABLE void pause();
        /**@brief Restart the rendering.
         * If pause() has been called, this function will restart the production
         * of frames. Otherwise, this function has no effect. */
        Q_INVOKABLE void resume();

        /**@brief Add a renderable to the scene.
         *
         * Add a renderable to the scene. This call is forwarded to the renderer,
         * if it exist, to store the renderable into the structures of the
         * renderer subclass. */
        void add_renderable( renderable* r );
        /**@brief Set a new camera.
         * Set a new camera to be used by the renderer. If a camera already exist,
         * it is not destroyed. Thus it is the caller responsibility to destroy
         * an existing camera. */
        void set_camera( QObject* cam );
        /**@brief Get the current camera.
         * Get the camera currently used by the renderer.
         * @return The current camera. */
        QObject* get_camera() const;

        /**@brief Set the number of samples for MSAA.
         * Set the number of samples to use for MSAA.
         * @param samples The new number of samples.
         */
        void set_samples( int samples );
        /**@brief Get the current number of samples used for MSAA.
         * Get the number of samples used for MSAA.
         * @return The current number of samples. */
        int get_samples() const;

        static QList< window* > window_instances;

      public slots:
        void ready();
        void cleanup();

      protected:
        QSGNode* updatePaintNode( QSGNode*, UpdatePaintNodeData* ) override;
        renderer* threaded_renderer;
        QThread render_thread;
      private slots:
        void handle_size_changed();
      };
    }
  }
}
# endif 
