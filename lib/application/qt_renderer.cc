# include "../../graphics-origin/application/window.h"
# include "../../graphics-origin/application/renderer.h"
# include "../../graphics-origin/application/camera.h"
# include "../../graphics-origin/application/renderable.h"
# include "../../graphics-origin/application/gl_helper.h"
# include "../../graphics-origin/tools/log.h"

# include <QtGui/QOpenGLContext>
# include <QtGui/QOpenGLFramebufferObject>
# include <QtGui/QOffscreenSurface>

namespace graphics_origin {
  namespace application {
    renderer::renderer() :
        surface( nullptr ), context( nullptr ),
# ifdef GO_QT_RENDERER_USE_DOUBLE_NORMAL_TEXTURES
        current_render_texture{ normal },
        frame_buffer_objects{ 0, 0, 0 }, color_textures{ 0, 0, 0 },
# else
        frame_buffer_objects{ 0, 0 }, color_textures{ 0, 0 },
# endif
        depth_render_buffer{ 0 },
        gl_camera( nullptr ),
        size_changed( 0 ), is_running( 1 ), width( 0 ), height( 0 ), samples(4)
    {}

    renderer::~renderer()
    {}

    void renderer::add( renderable* r )
    {
      do_add( r );
      r->renderer_ptr = this;
    }

    gl_vec3 renderer::get_camera_position() const
    {
      return gl_camera->get_position();
    }

    void renderer::set_surface_dimensions( const real& width, const real& height )
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

    const gl_mat4& renderer::get_view_matrix() const
    {
      return gl_camera->get_view_matrix();
    }

    const gl_mat4& renderer::get_projection_matrix() const
    {
      return gl_camera->get_projection_matrix();
    }

    gl_vec2 renderer::get_window_dimensions() const
    {
      return gl_vec2{ width, height };
    }

    // when the texture node is using the texture of the display FBO,
    // it sends a queued signal that execute the following function
    void renderer::render_next()
    {
      period.tick();
      context->makeCurrent( surface );

      if( !frame_buffer_objects[0] )
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

      if( !is_running )
        {
          std::unique_lock<std::mutex> l( lock );
          cv.wait (l, [this]{ return is_running;});
        }

      render_gl();
      transfer_to_normal_fbo();
      period.tock();
# ifdef GO_QT_RENDERER_USE_DOUBLE_NORMAL_TEXTURES
      emit texture_ready( color_textures[current_render_texture], QSize( width, height ) );
      current_render_texture = ( current_render_texture == normal ? normalbis : normal );
# else
      emit texture_ready( color_textures[normal], QSize( width, height ) );
# endif
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
}}
