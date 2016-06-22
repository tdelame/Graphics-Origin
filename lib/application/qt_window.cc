# include "../../graphics-origin/application/qt_application/window.h"
# include "../../graphics-origin/application/qt_application/renderer.h"
# include "../../graphics-origin/application/qt_application/texture_node.h"
# include "../../graphics-origin/application/camera.h"
# include "../../graphics-origin/application/gl_helper.h" // to initialize gl context
# include "../../graphics-origin/tools/log.h"

# include <QtQuick/QQuickWindow>
# include <QtGui/QOpenGLContext>
# include <QtGui/QOffscreenSurface>

namespace graphics_origin {
  namespace application {
    namespace qt {

      QList< window* > window::window_instances;

      window::window( QQuickItem* parent ) :
          QQuickItem( parent ), threaded_renderer( nullptr )
      {
        // make sure updatePaintNode() is called during the update of the scene graph
        setFlag( ItemHasContents, true );
        // catch resize events, to transmit them to the renderer
        connect( this, SIGNAL(heightChanged()), this, SLOT(handle_size_changed()));
        connect( this, SIGNAL(widthChanged()), this, SLOT(handle_size_changed()));
        // add this instance to the list of existing instances. TODO: check if this is still necessary
        window_instances << this;
      }

      window::~window()
      {
        cleanup();
        window_instances.removeOne( this );
        render_thread.quit();
      }

      void window::initialize_renderer( renderer* new_renderer)
      {
        cleanup();
        threaded_renderer = new_renderer;
        LOG( info, "QT Window " << this << " is now using QT renderer " << threaded_renderer );
      }

      void window::pause()
      {
        if( threaded_renderer )
          threaded_renderer->pause();
      }

      void window::resume()
      {
        if( threaded_renderer )
          threaded_renderer->resume();
      }

      void window::add_renderable( renderable* r )
      {
        if( threaded_renderer )
          threaded_renderer->add( r );
      }

      void window::set_camera( QObject* cam )
      {
        auto res = dynamic_cast< camera* >( cam );
        if( res && threaded_renderer )
          threaded_renderer->gl_camera = res;
      }

      QObject* window::get_camera() const
      {
        if( threaded_renderer )
          return dynamic_cast< QObject* >( threaded_renderer->gl_camera );
        return nullptr;
      }

      void window::set_samples( int samples )
      {
        if( threaded_renderer )
          threaded_renderer->set_samples( samples );
      }

      int window::get_samples() const
      {
        if( threaded_renderer )
          return threaded_renderer->get_samples();
        return 0;
      }

      void window::ready()
      {
        // we assume that when this function is called, we have a renderer
        threaded_renderer->surface = new QOffscreenSurface();
        threaded_renderer->surface->setFormat( threaded_renderer->context->format() );
        threaded_renderer->surface->create();

        threaded_renderer->moveToThread( &render_thread );
        render_thread.start();
        update();
      }

      void window::cleanup()
      {
        if( threaded_renderer )
          {
            resume();
            QMetaObject::invokeMethod( threaded_renderer, "shut_down", Qt::QueuedConnection );
            while( threaded_renderer->is_running )
              {

              }
            delete threaded_renderer;
            threaded_renderer = nullptr;
          }
      }

      QSGNode* window::updatePaintNode( QSGNode* old_node, UpdatePaintNodeData* )
      {
        if( !threaded_renderer ) return nullptr;

        if( !threaded_renderer->context )
          {
            QOpenGLContext* current = QQuickItem::window()->openglContext();
            current->doneCurrent();
              threaded_renderer->context = new QOpenGLContext();
              threaded_renderer->context->setFormat( current->format() );
              threaded_renderer->context->setShareContext( current ); //why?
              threaded_renderer->context->create();
              threaded_renderer->context->moveToThread( &render_thread );
            current->makeCurrent( QQuickItem::window() );

            initialize_glew_for_current_context();
            initialize_current_context();

            QMetaObject::invokeMethod( this, "ready" );
            return nullptr;
          }

        texture_node* node = static_cast< texture_node* >( old_node );
        if( !node )
          {
            node = new texture_node( QQuickItem::window() );

            connect( threaded_renderer, SIGNAL(texture_ready(int,QSize)), node, SLOT(new_texture(int,QSize)), Qt::DirectConnection);
            connect( node, SIGNAL(pending_new_texture()), QQuickItem::window(), SLOT(update()), Qt::QueuedConnection);
            connect( QQuickItem::window(), SIGNAL(beforeRendering()), node, SLOT(prepare_node()), Qt::DirectConnection);
            connect( node, SIGNAL(texture_in_use()), threaded_renderer, SLOT(render_next()), Qt::QueuedConnection);

            // Get the production of FBO textures started..
            QMetaObject::invokeMethod( threaded_renderer, "render_next", Qt::QueuedConnection);
          }
        node->setRect( boundingRect() );
        return node;
      }

      void window::handle_size_changed()
      {
        if( threaded_renderer )
          threaded_renderer->set_size(
              std::max( qreal{5}, width() ),
              std::max( qreal{5}, height() ));
      }

    }
  }
}
