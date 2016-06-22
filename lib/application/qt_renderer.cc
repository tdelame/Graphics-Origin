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
      renderer::renderer() :
          surface( nullptr ), context( nullptr ),
          frame_buffer_object{0}, color_textures{0,0}, depth_render_buffer{0}, current_color_texture{0},
          gl_camera( nullptr ),
          size_changed( 0 ), is_running( 1 ), width( 0 ), height( 0 ), samples(0)
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

      void renderer::set_samples( int samples )
      {
        this->samples = samples;
        // Ok, I should use another atomic_char for that. But I do not really
        // want to test for size change AND samples changes, and then to have
        // a function to update just the relevant openGL objects after a change
        // in the number of samples.
        size_changed = 1;
      }

      int renderer::get_samples() const
      {
        return samples;
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




      // when the texture node is using the texture of the display FBO,
      // it sends a queued signal that execute the following function
      void renderer::render_next()
      {
        context->makeCurrent( surface );

        if( !frame_buffer_object )
          {
            setup_opengl_objects();
          }
        else if( size_changed )
          {
            size_changed = 0;
            destroy_textures();
            destroy_render_buffers();

            build_textures();
            build_render_buffers();
            complete_frame_buffer();
          }

//        // initialize the buffers and renderer
//        if( !render_fbo )
//          {
//            auto format = get_format ();
//            downsampled_fbo = new QOpenGLFramebufferObject( QSize( width, height ), format );
////            format.setSamples( 4 );
//            render_fbo = new QOpenGLFramebufferObject( QSize( width, height ), format );
//            display_fbo = new QOpenGLFramebufferObject( QSize( width, height ), format );
//            glcheck(glViewport( 0, 0, width, height ));
//          }
//        else if( size_changed )
//          {
//            size_changed = 0;
//            delete render_fbo;
//            delete display_fbo;
//            delete downsampled_fbo;
//
//            auto format = get_format ();
//            downsampled_fbo = new QOpenGLFramebufferObject( QSize( width, height ), format );
////            format.setSamples( 4 );
//            render_fbo = new QOpenGLFramebufferObject( QSize( width, height ), format );
//            display_fbo = new QOpenGLFramebufferObject( QSize( width, height ), format );
//            glcheck(glViewport( 0, 0, width, height ));
//          }

        if( !is_running )
          {
            std::unique_lock<std::mutex> l( lock );
            cv.wait (l, [this]{ return is_running;});
          }

        render_gl();

        GLuint display_texture = current_color_texture;
//        current_color_texture = current_color_texture ? 0 : 1 ;
        emit texture_ready( color_textures[display_texture], QSize( width, height ) );


//        QOpenGLFramebufferObject::blitFramebuffer (
//            downsampled_fbo, render_fbo,
//            GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT,
//            GL_NEAREST);

      }

      void renderer::shut_down()
      {
        do_shut_down();
        context->makeCurrent( surface );

        destroy_opengl_objects();

        context->doneCurrent();
        delete context;
        context = nullptr;

        surface->deleteLater();
        is_running = 0;
      }

}}}
