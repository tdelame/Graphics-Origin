# include <graphics-origin/application/gl_window.h>
# include <graphics-origin/application/gl_window_renderer.h>
# include <graphics-origin/application/gl_texture_node.h>
# include <graphics-origin/application/gl_helper.h>
# include <graphics-origin/application/camera.h>
# include <graphics-origin/tools/log.h>

# include <QtQuick/QQuickWindow>
# include <QtGui/QOpenGLContext>
# include <QtGui/QOffscreenSurface>

namespace graphics_origin {
namespace application {

  QList< gl_window* > gl_window::g_gl_windows;

  gl_window::gl_window( QQuickItem* parent )
  : QQuickItem{ parent }, m_renderer{ nullptr }
  {
    setFlag( ItemHasContents, true );
    connect( this, SIGNAL(windowChanged(QQuickWindow*)),
             this, SLOT(handle_window_changed(QQuickWindow*)));
    connect( this, SIGNAL(heightChanged()),
             this, SLOT(handle_size_changed()));
    connect( this, SIGNAL(widthChanged()),
             this, SLOT(handle_size_changed()));
    g_gl_windows << this;
  }

  void
  gl_window::initialize_renderer( gl_window_renderer* renderer )
  {
    cleanup();
    m_renderer = renderer;
    LOG( info, "GL Window " << this << " initialized its renderer to " << m_renderer )
  }

  gl_window::~gl_window()
  {
    cleanup();
    g_gl_windows.removeOne( this );
  }

  void
  gl_window::ready()
  {
    m_renderer->m_surface = new QOffscreenSurface();
    m_renderer->m_surface->setFormat( m_renderer->m_context->format() );
    m_renderer->m_surface->create();
    m_renderer->moveToThread( m_renderer );
    m_renderer->start();
    update();
  }

  void
  gl_window::sync()
  {
    m_renderer->set_size( width(), height() );
  }

  void
  gl_window::pause()
  {
    m_renderer->pause();
  }

  void
  gl_window::resume()
  {
    m_renderer->resume();
  }

  void
  gl_window::cleanup()
  {
    if( m_renderer )
      {
        resume();
        QMetaObject::invokeMethod( m_renderer, "shut_down", Qt::QueuedConnection );
        while( m_renderer->isRunning() )
          {
            ///TODO: can we do that in a more nice manner? Like wait( !isRunning() )
          }
        delete m_renderer;
        m_renderer = nullptr;
      }
  }

  QObject*
  gl_window::get_camera() const
  {
    return dynamic_cast<QObject*>(m_renderer->m_camera);
  }

  void
  gl_window::set_camera(QObject* cam )
  {
    m_renderer->m_camera = dynamic_cast<camera*>( cam );
  }

  void
  gl_window::add_renderable( renderable* r )
  {
    m_renderer->add( r );
  }

  /* Called on the render thread when it is time to sync the state of the item
   * with the scene graph.
   *
   * The function is called as a result of QQuickItem::update(), if the user
   * has set the QQuickItem::ItemHasContents flag on the item. The function
   * should return the root of the scene graph subtree for this item. Most
   * implementations will return a single QSGGeometryNode containing the
   * visual representation of this item. oldNode is the node that was
   * returned the last time the function was called. updatePaintNodeData
   * provides a pointer to the QSGTransformNode associated with this
   * QQuickItem.
   *
   * The main thread is blocked while this function is executed so it is safe
   * to read values from the QQuickItem instance and other objects in the main
   * thread.
   *
   * If no call to QQuickItem::updatePaintNode() result in actual scene graph
   * changes, like QSGNode::markDirty() or adding and removing nodes, then the
   * underlying implementation may decide to not render the scene again as the
   * visual outcome is identical.
   *
   * Warning: It is crucial that OpenGL operations and interaction with the
   * scene graph happens exclusively on the render thread, primarily during
   * the QQuickItem::updatePaintNode() call. The best rule of thumb is to only
   * use classes with the "QSG" prefix inside the
   * QQuickItem::updatePaintNode() function.
   *
   * Warning: This function is called on the render thread. This means any
   * QObjects or thread local storage that is created will have affinity to
   * the render thread, so apply caution when doing anything other than
   * rendering in this function. Similarly for signals, these will be emitted
   * on the render thread and will thus often be delivered via queued
   * connections.
   *
   * Note: All classes with QSG prefix should be used solely on the scene
   * graph's rendering thread. See Scene Graph and Rendering for more
   * information. */
  QSGNode* gl_window::updatePaintNode( QSGNode* old_node, UpdatePaintNodeData* )
  {
    if( !m_renderer->m_context )
      {
        QOpenGLContext* current = window()->openglContext();
        // Some GL implementations requires that the currently bound context is
        // made non-current before we set up sharing, so we doneCurrent here
        // and makeCurrent down below while setting up our own context.
        current->doneCurrent();

        m_renderer->m_context = new QOpenGLContext();
        m_renderer->m_context->setFormat(current->format());
        m_renderer->m_context->setShareContext(current);
        m_renderer->m_context->create();
        m_renderer->m_context->moveToThread(m_renderer);

        current->makeCurrent(window());
        initialize_glew_for_current_context();
        initialize_current_context();
        QMetaObject::invokeMethod(this, "ready");
        return nullptr;
      }

    gl_texture_node* node = static_cast< gl_texture_node* >( old_node );
    if( !node )
      {
        node = new gl_texture_node( window() );
        /* Set up connections to get the production of FBO textures in sync with vsync on the
         * rendering thread.
         *
         * When a new texture is ready on the rendering thread, we use a direct connection to
         * the texture node to let it know a new texture can be used. The node will then
         * emit pendingNewTexture which we bind to QQuickWindow::update to schedule a redraw.
         *
         * When the scene graph starts rendering the next frame, the prepareNode() function
         * is used to update the node with the new texture. Once it completes, it emits
         * textureInUse() which we connect to the FBO rendering thread's renderNext() to have
         * it start producing content into its current "back buffer".
         *
         * This FBO rendering pipeline is throttled by vsync on the scene graph rendering thread.
         */
        connect( m_renderer, SIGNAL(texture_ready(int,QSize)), node, SLOT(new_texture(int,QSize)), Qt::DirectConnection);
        connect( node, SIGNAL(pending_new_texture()), window(), SLOT(update()), Qt::QueuedConnection);
        connect( window(), SIGNAL(beforeRendering()), node, SLOT(prepare_node()), Qt::DirectConnection);
        connect( node, SIGNAL(texture_in_use()), m_renderer, SLOT(render_next()), Qt::QueuedConnection);

        // Get the production of FBO textures started..
        QMetaObject::invokeMethod( m_renderer, "render_next", Qt::QueuedConnection);
      }
    node->setRect( boundingRect() );
    return node;
  }

  void
  gl_window::handle_size_changed()
  {
    m_renderer->set_size( std::max( qreal{5}, width() ), std::max( qreal{5}, height() ) );
  }

  void
  gl_window::handle_window_changed( QQuickWindow* win )
  {
    if( win )
      {
        ///TODO: not sure about that. I believe this should be done only one time and not at avery signal.
//        connect( win, SIGNAL(beforeSynchronizing()), this, SLOT(sync()), Qt::DirectConnection);
//        connect( win, SIGNAL(sceneGraphInvalidated()), this, SLOT(cleanup()), Qt::DirectConnection);
//        win->setClearBeforeRendering(false); // otherwise, QML will always clear what we draw
      }
  }

  qreal
  gl_window::get_fps() const
  {
    return m_renderer->get_fps();
  }

}}
