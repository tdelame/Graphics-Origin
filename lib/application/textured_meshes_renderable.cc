# include "../../graphics-origin/application/renderables/textured_mesh_renderable.h"
# include "../../graphics-origin/application/gl_helper.h"
# include "../../graphics-origin/application/camera.h"
# include "../../graphics-origin/application/renderer.h"
# include "../../graphics-origin/geometry/mesh.h"
# include "../../graphics-origin/tools/log.h"

# include <FreeImage.h>

# include <GL/glew.h>

namespace graphics_origin { namespace application {

  textured_mesh_renderable::textured_mesh_renderable(
      shader_program_ptr program )
    : m_fib{ nullptr }, m_vbo{ 0 }, m_vao{ 0 }, m_texture_id{ 0 }
  {
    model = gpu_mat4(1.0);
    this->program = program;
  }

  void
  textured_mesh_renderable::update_gpu_data()
  {
    if( !m_vao )
      {
        glcheck(glGenVertexArrays( 1, &m_vao ));
        glcheck(glGenBuffers( number_of_buffers, m_vbo ));
        glcheck(glGenTextures( 1, &m_texture_id ));
      }

    /**Note: this is far from optimal.
     *
     * Texture coordinates at a specific position can be different depending
     * on the face passing by this point. Thus, texture coordinates are
     * always considered by face. Also, this means that positions and normals
     * must be by face too, in order to define coherent vertex attributes.
     * So, all we can do is to render a set of triangles and repeat the vertex
     * positions and normals, inducing a higher memory consumption.
     *
     * We could reduce the memory consumption if we read the mesh directly from
     * a file. However, this is not something I would like to spend time on. So,
     * if memory consumption is an issue, I recommend you to load your vertices
     * with a library like tinyobjloader (https://github.com/syoyo/tinyobjloader).
     */
    const auto nfaces = mesh.n_faces();
    const auto nvertices = nfaces * 3;
    const auto attribute_dimension = 8; // fvec3 + fvec3 + fvec2
    const auto attribute_size = attribute_dimension * sizeof(gl_real);
    std::vector< gl_real > attributes( nvertices * attribute_dimension );
# ifdef _WIN32
# pragma message("MSVC does not allow unsigned index variable in OpenMP for statement")
# pragma omp parallel for
  for (long i = 0; i < nfaces; ++i )
# else
    # pragma omp parallel for schedule(dynamic)
    for( size_t i = 0; i < nfaces; ++ i )
# endif
      {
        auto dst = attributes.data() + attribute_dimension * i * 3;
        auto heh = mesh.halfedge_handle( geometry::mesh::FaceHandle( i ) );
        for( int j = 0; j < 3; ++ j, dst += attribute_dimension, heh = mesh.next_halfedge_handle( heh ) )
          {
            auto vh = mesh.to_vertex_handle( heh );
            auto& point = mesh.point( vh );
            auto& normal = mesh.normal( vh );
            auto uv = mesh.texcoord2D( heh );

            dst[0] = point[0];
            dst[1] = point[1];
            dst[2] = point[2];
            dst[3] = normal[0];
            dst[4] = normal[1];
            dst[5] = normal[2];
            dst[6] = uv[0];
            dst[7] = uv[1];
          }
      }

    int position_location = program->get_attribute_location( "position" );
    int  texture_location = program->get_attribute_location( "texture"  );
    int   normal_location = program->get_attribute_location( "normal"   );

    glcheck(glBindVertexArray( m_vao ));
      glcheck(glBindBuffer( GL_ARRAY_BUFFER, m_vbo[ position_normal_texture_vbo ] ));
      glcheck(glBufferData( GL_ARRAY_BUFFER, nvertices * attribute_size, attributes.data(), GL_STATIC_DRAW ));

      glcheck(glEnableVertexAttribArray( position_location ));
      glcheck(glVertexAttribPointer( position_location,
        3, GL_FLOAT, GL_FALSE,
        attribute_size,
        reinterpret_cast<void*>( 0 )));

      glcheck(glEnableVertexAttribArray( normal_location ));
      glcheck(glVertexAttribPointer( normal_location,
        3, GL_FLOAT, GL_FALSE,
        attribute_size,
        reinterpret_cast<void*>( 3 * sizeof( gl_real ))));

      glcheck(glEnableVertexAttribArray( texture_location ));
      glcheck(glVertexAttribPointer( texture_location,
        2, GL_FLOAT, GL_FALSE,
        attribute_size,
        reinterpret_cast<void*>( 6 * sizeof( gl_real ))));

      glcheck(glActiveTexture(GL_TEXTURE0));
      glcheck(glBindTexture(GL_TEXTURE_2D, m_texture_id));
      glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
      glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
      glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
      glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
      glcheck(glTexImage2D( GL_TEXTURE_2D,
        0, GL_RGBA32F, FreeImage_GetWidth(m_fib), FreeImage_GetHeight(m_fib), 0,
        GL_BGRA, GL_UNSIGNED_BYTE, FreeImage_GetBits(m_fib)));

    glcheck(glBindVertexArray( 0 ));
    glcheck(glBindBuffer( GL_ARRAY_BUFFER, 0));
    glcheck(glBindTexture(GL_TEXTURE_2D, 0));
  }

  void
  textured_mesh_renderable::do_render()
  {
    shader_program::identifier location = program->get_uniform_location( "window_dimensions");
    if( location != shader_program::null_identifier )
      glcheck(glUniform2fv( location, 1, glm::value_ptr( renderer_ptr->get_window_dimensions())));
    glcheck(glUniform3fv( program->get_uniform_location( "light_position"), 1, glm::value_ptr( renderer_ptr->get_camera_position() )));
    glcheck(glUniformMatrix4fv( program->get_uniform_location( "model"), 1, GL_FALSE, glm::value_ptr( model )));
    glcheck(glUniformMatrix4fv( program->get_uniform_location( "view"), 1, GL_FALSE, glm::value_ptr( renderer_ptr->get_view_matrix() )));
    glcheck(glUniformMatrix4fv( program->get_uniform_location( "projection"), 1, GL_FALSE, glm::value_ptr( renderer_ptr->get_projection_matrix())));
    glcheck(glUniformMatrix3fv( program->get_uniform_location( "nit" ), 1, GL_FALSE, glm::value_ptr( gpu_mat3( glm::transpose( glm::inverse( model ) ) ) ) ));
    glcheck(glBindVertexArray( m_vao ));

    glcheck(glBindTexture(GL_TEXTURE_2D, m_texture_id ));
    glcheck(glUniform1i( program->get_uniform_location( "sampler"), 0));
    glcheck(glDrawArrays( GL_TRIANGLES, 0, mesh.n_faces() * 3 ));
    glcheck(glBindVertexArray( 0 ) );
    glcheck(glBindTexture(GL_TEXTURE_2D, 0 ));
  }

  void
  textured_mesh_renderable::remove_gpu_data()
  {
    if( m_vao )
      {
        glcheck(glDeleteBuffers( number_of_buffers, m_vbo));
        glcheck(glDeleteVertexArrays( 1, &m_vao ));
        m_vao = 0;
      }
  }

  textured_mesh_renderable::~textured_mesh_renderable()
  {
    remove_gpu_data();
    if( m_fib )
      FreeImage_Unload(m_fib);
  }

  void
  textured_mesh_renderable::load_mesh( const std::string& filename )
  {
    mesh.load( filename );
    m_dirty = true;
  }

  void
  textured_mesh_renderable::load_texture( const std::string& filename )
  {
    FREE_IMAGE_FORMAT fif = FreeImage_GetFileType( filename.c_str(), 0 );
    if( fif == FIF_UNKNOWN )
      fif = FreeImage_GetFIFFromFilename( filename.c_str() );
    if( fif == FIF_UNKNOWN )
      {
        LOG( error, "[" << filename << "] is an unknown image file");
        LOG( error, "cannot load texture in textured mesh renderable " << this );
        return;
      }

    if( FreeImage_FIFSupportsReading( fif ) )
      {
        m_fib = FreeImage_Load( fif, filename.c_str() );

        int bitsPerPixel =  FreeImage_GetBPP( m_fib );
        if( bitsPerPixel != 32 )
          {
            FIBITMAP* fib32 = FreeImage_ConvertTo32Bits( m_fib );
            FreeImage_Unload( m_fib );
            m_fib = fib32;
          }
        set_dirty();
      }
    else
      {
        LOG( error, "FreeImage cannot read image file [" << filename << "]");
      }
  }

}}
