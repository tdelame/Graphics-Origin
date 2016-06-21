# include <graphics-origin/application/camera.h>
# include <graphics-origin/application/gl_window_renderer.h>
# include <graphics-origin/application/gl_helper.h>
# include <graphics-origin/application/renderable.h>

# include <QtGui/QOpenGLContext>
# include <QtGui/QOpenGLFramebufferObject>
# include <QtGui/QOffscreenSurface>

namespace graphics_origin {
namespace application {

  gl_window_renderer::gl_window_renderer()
    : m_surface{ nullptr }, m_context{ nullptr },
      m_render_fbo{ nullptr }, m_display_fbo{ nullptr },
      m_downsampled_fbo{ nullptr },

      m_camera{ nullptr }, m_size_changed{ 0 },
      m_is_running{ 1 }, m_width{ 0 }, m_height{ 0 },

      m_fps_counter{ 2.0 }, m_cached_fps{ 0 }
  {}

  gl_window_renderer::~gl_window_renderer()
  {}

  void
  gl_window_renderer::add( renderable* r )
  {
    do_add( r );
    r->set_renderer( this );
  }

  void
  gl_window_renderer::set_size( const real& width, const real& height )
  {
    m_width = width;
    m_height = height;
    m_size_changed = 1;
  }

  void
  gl_window_renderer::pause()
  {
    m_is_running = 0;
  }

  void
  gl_window_renderer::resume()
  {
    m_is_running = 1;
    m_cv.notify_all();
  }

  const QOpenGLFramebufferObjectFormat&
  get_format()
  {
    static bool initialized = false;
    static QOpenGLFramebufferObjectFormat instance;
    if( !initialized )
      {
        instance.setAttachment( QOpenGLFramebufferObject::CombinedDepthStencil );
      }
    return instance;
  }

  void
  gl_window_renderer::render_next()
  {
    m_context->makeCurrent(m_surface);

     // initialize the buffers and renderer
     if( !m_render_fbo)
       {
         auto format = get_format();
         m_downsampled_fbo = new QOpenGLFramebufferObject( QSize(m_width, m_height), format );
         format.setSamples( 4 );
         m_render_fbo = new QOpenGLFramebufferObject( QSize(m_width,m_height), format );
         m_display_fbo = new QOpenGLFramebufferObject( QSize(m_width,m_height), format );
         glcheck(glClearColor( 1.0, 1.0, 1.0, 1.0 ));
         glcheck(glViewport( 0, 0, m_width, m_height ));
       }
     else if ( m_size_changed )
       {
         m_size_changed = 0;
         delete m_render_fbo;
         delete m_display_fbo;
         delete m_downsampled_fbo;

         auto format = get_format();
         m_downsampled_fbo = new QOpenGLFramebufferObject( QSize(m_width, m_height), format );
         format.setSamples( 4 );
         m_render_fbo = new QOpenGLFramebufferObject( QSize(m_width,m_height), format );
         m_display_fbo = new QOpenGLFramebufferObject( QSize(m_width,m_height), format );
         glcheck(glViewport( 0, 0, m_width, m_height ));
       }

     if( !m_is_running )
       {
         std::unique_lock<std::mutex> l (m_lock);
         m_cv.wait( l, [this]{ return m_is_running; } );
       }

     m_render_fbo->bind();
     m_cached_fps = m_fps_counter.get_fps();
     glEnable(GL_DEPTH_TEST);
     glDepthFunc(GL_LESS);
     glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
     do_render();
     m_render_fbo->bindDefault();
     // We need to flush the contents to the FBO before posting the texture to the
     // other thread, otherwise, we might get unexpected results.
     //glFlush();
     glFinish();



     QOpenGLFramebufferObject::blitFramebuffer(
         m_downsampled_fbo,
         m_render_fbo,
         GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT| GL_STENCIL_BUFFER_BIT,GL_NEAREST);

     std::swap( m_render_fbo, m_display_fbo );
     emit texture_ready( m_downsampled_fbo->texture(), QSize(m_width,m_height));
  }

  void
  gl_window_renderer::shut_down()
  {
    do_shut_down();
    this->quit();
    m_context->makeCurrent( m_surface );
    delete m_render_fbo;
    delete m_display_fbo;
    delete m_downsampled_fbo;

    m_context->doneCurrent();
    delete m_context;
    m_context = nullptr;

    m_surface->deleteLater();
  }

  const gpu_mat4&
  gl_window_renderer::get_view_matrix() const
  {
    return m_camera->get_view_matrix();
  }

  void
  gl_window_renderer::set_view_matrix( const gpu_mat4& view )
  {
    m_camera->set_view_matrix( view );
  }

  const gpu_mat4&
  gl_window_renderer::get_projection_matrix() const
  {
    return m_camera->get_projection_matrix();
  }

  real
  gl_window_renderer::get_fps() const
  {
    return m_cached_fps;
  }

  gpu_vec2
  gl_window_renderer::get_window_dimensions() const
  {
    return gpu_vec2{ m_width, m_height };
  }
}}
