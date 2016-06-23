# ifndef GRAPHICS_ORIGIN_QT_TEXTURE_NODE_H_
# define GRAPHICS_ORIGIN_QT_TEXTURE_NODE_H_
# include "../../graphics_origin.h"

# include <QtCore/QMutex>
# include <qsgsimpletexturenode.h>

class QQuickWindow;
class QSGTexture;

namespace graphics_origin {
  namespace application {
    namespace qt {
      /**@brief Display our 3D scene thanks to a texture.
       *
       * This class is used to display a texture, produced by an offscreen
       * rendering, for a window into the Qt application. It is very unlikely
       * you will have to touch this class. */
      class texture_node
        : public QObject,
          public QSGSimpleTextureNode {
        Q_OBJECT
      public:
        texture_node( QQuickWindow* window );
        ~texture_node();

      signals:
        void texture_in_use();
        void pending_new_texture();

      public slots:
        void new_texture( int id, const QSize& s );
        void prepare_node();

      private:
        unsigned int m_identifier;
        QSize m_size;
        QMutex m_mutex;
        QSGTexture* m_texture;
        QQuickWindow* m_window;
      };
    }
  }
}
# endif 
