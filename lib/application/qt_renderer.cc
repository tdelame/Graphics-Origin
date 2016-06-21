# include "../../graphics-origin/application/qt_application/window.h"
# include "../../graphics-origin/application/qt_application/renderer.h"
# include "../../graphics-origin/application/camera.h"
# include "../../graphics-origin/application/renderable.h"
# include "../../graphics-origin/application/gl_helper.h"
# include "../../graphics-origin/tools/log.h"

# include <QtGui/QOpenGLContext>
# include <QtGui/QOpenGLFramebufferObject>
# include <QtGui/QOffscreenSurface>

namespace graphics_origin {
  namespace application {
    namespace qt {

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

      renderer::renderer() :
          surface( nullptr ), context( nullptr ),
          render_fbo( nullptr ), display_fbo( nullptr ), downsampled_fbo( nullptr ),
          gl_camera( nullptr ),
          size_changed( 0 ), is_running( 1 ), width( 0 ), height( 0 )
      {}

      renderer::~renderer()
      {}

      void renderer::add( renderable* r )
      {
        do_add( r );
        r->set_renderer( this );
      }

      void renderer::set_size( const real& width, const real& height )
      {
        this->width = width;
        this->height = height;
        size_changed = 1;
      }

      void renderer::pause()
      {
        is_running = 0;
      }

      void renderer::resume()
      {
        is_running = 1;
        cv.notify_all();
      }

      const gpu_mat4& renderer::get_view_matrix() const
      {
        return gl_camera->get_view_matrix();
      }
      void renderer::set_view_matrix( const gpu_mat4& new_view_matrix )
      {
        gl_camera->set_view_matrix( new_view_matrix );
      }

      const gpu_mat4& renderer::get_projection_matrix() const
      {
        return gl_camera->get_projection_matrix();
      }

      gpu_vec2 renderer::get_window_dimensions() const
      {
        return gpu_vec2{ width, height };
      }

      void renderer::render_next()
      {
        context->makeCurrent( surface );

        // initialize the buffers and renderer
        if( !render_fbo )
          {
            auto format = get_format ();
            downsampled_fbo = new QOpenGLFramebufferObject( QSize( width, height ), format );
            format.setSamples( 4 );
            render_fbo = new QOpenGLFramebufferObject( QSize( width, height ), format );
            display_fbo = new QOpenGLFramebufferObject( QSize( width, height ), format );
            glcheck(glClearColor( 1.0, 1.0, 1.0, 1.0 ));
            glcheck(glViewport( 0, 0, width, height ));
          }
        else if( size_changed )
          {
            size_changed = 0;
            delete render_fbo;
            delete display_fbo;
            delete downsampled_fbo;

            auto format = get_format ();
            downsampled_fbo = new QOpenGLFramebufferObject( QSize( width, height ), format );
            format.setSamples( 4 );
            render_fbo = new QOpenGLFramebufferObject( QSize( width, height ), format );
            display_fbo = new QOpenGLFramebufferObject( QSize( width, height ), format );
            glcheck(glViewport( 0, 0, width, height ));
          }

        if( !is_running )
          {
            std::unique_lock<std::mutex> l( lock );
            cv.wait (l, [this]{ return is_running;});
          }

        render_fbo->bind ();
        glEnable (GL_DEPTH_TEST);
        glDepthFunc (GL_LESS);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
          do_render();
          render_fbo->bindDefault ();
        // We need to flush the contents to the FBO before posting the texture to the
        // other thread, otherwise, we might get unexpected results.
        //glFlush();
        glFinish ();

        QOpenGLFramebufferObject::blitFramebuffer (
            downsampled_fbo, render_fbo,
            GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
            GL_NEAREST);

        std::swap (render_fbo, display_fbo);
        emit texture_ready (downsampled_fbo->texture (),  QSize( width, height ));
      }

      void renderer::shut_down()
      {
        do_shut_down();
        context->makeCurrent( surface );
        delete render_fbo;
        delete display_fbo;
        delete downsampled_fbo;

        context->doneCurrent();
        delete context;
        context = nullptr;

        surface->deleteLater();
        is_running = 0;
      }

}}}
