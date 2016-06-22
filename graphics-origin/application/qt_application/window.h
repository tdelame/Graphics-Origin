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
      class GO_API window :
          public QQuickItem {
        Q_OBJECT
        Q_PROPERTY( QObject* camera READ get_camera WRITE set_camera )
        Q_PROPERTY( int samples READ get_samples WRITE set_samples )
      public:
        window( QQuickItem* parent = nullptr );
        ~window();

        Q_INVOKABLE
        void initialize_renderer( renderer* new_renderer );

        Q_INVOKABLE void pause();
        Q_INVOKABLE void resume();

        void add_renderable( renderable* r );
        void set_camera( QObject* cam );
        QObject* get_camera() const;

        void set_samples( int samples );
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
