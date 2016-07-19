# include "../../graphics-origin/application/renderables/mesh_renderable.h"
# include "../../graphics-origin/application/gl_helper.h"
# include "../../graphics-origin/application/camera.h"
# include "../../graphics-origin/application/renderer.h"
# include "../../graphics-origin/geometry/mesh.h"
# include "../../graphics-origin/tools/log.h"

# include <GL/glew.h>

namespace graphics_origin { namespace application {

  mesh_renderable::mesh_renderable(
      shader_program_ptr program )
    : m_vao{ 0 }
  {
    model = gpu_mat4(1.0);
    this->program = program;
  }

  void
  mesh_renderable::update_gpu_data()
  {
    if( !m_vao )
      {
        glcheck(glGenVertexArrays( 1, &m_vao ));
        glcheck(glGenBuffers( NUMBER_OF_VBO, m_vbo ));
      }

    const auto nvertices = m_mesh.n_vertices();
    std::vector< gpu_real > positions_normals( nvertices * 6 ); // fvec3 + fvec3
# ifdef _WIN32
# pragma message("MSVC does not allow unsigned index variable in OpenMP for statement")
# pragma omp parallel for
	for (long i = 0; i < nvertices; ++i )
# else
    # pragma omp parallel for schedule(dynamic)
    for( size_t i = 0; i < nvertices; ++ i )
# endif
      {
        auto vh = geometry::mesh::VertexHandle( i );
        auto& point = m_mesh.point( vh );
        auto& normal = m_mesh.normal( vh );

        auto dst = positions_normals.data() + 6 * i;
        dst[0] = point[0];
        dst[1] = point[1];
        dst[2] = point[2];
        dst[3] = normal[0];
        dst[4] = normal[1];
        dst[5] = normal[2];
      }

    const auto nfaces = m_mesh.n_faces();
    std::vector< unsigned int > indices( nfaces * 3 );
# ifdef _WIN32
# pragma message("MSVC does not allow unsigned index variable in OpenMP for statement")
# pragma omp parallel for schedule(static)
	for (long i = 0; i < nfaces; ++i)
# else
    # pragma omp parallel for schedule(static)
    for( size_t i = 0; i < nfaces; ++ i )
# endif
      {
        auto fvit = m_mesh.fv_begin( geometry::mesh::FaceHandle( i ) );
        auto dst = indices.data() + 3 * i;
        dst[ 0 ] = fvit->idx(); ++ fvit;
        dst[ 1 ] = fvit->idx(); ++ fvit;
        dst[ 2 ] = fvit->idx();
      }

    int position_location = program->get_attribute_location( "position" );
    int   normal_location = program->get_attribute_location( "normal"   );

    glcheck(glBindVertexArray( m_vao ));
      glcheck(glBindBuffer( GL_ARRAY_BUFFER, m_vbo[POSITION_NORMAL] ));
      glcheck(glBufferData( GL_ARRAY_BUFFER, positions_normals.size() * sizeof( gpu_real ), positions_normals.data(), GL_STATIC_DRAW ));

      glcheck(glEnableVertexAttribArray( position_location ));
      glcheck(glVertexAttribPointer( position_location,
        3, GL_FLOAT, GL_FALSE,
        6 * sizeof( gpu_real ),
        reinterpret_cast<void*>( 0 )));

      glcheck(glEnableVertexAttribArray( normal_location ));
      glcheck(glVertexAttribPointer( normal_location,
        3, GL_FLOAT, GL_FALSE,
        6 * sizeof( gpu_real ),
        reinterpret_cast<void*>( 3 * sizeof( gpu_real ))));

      glcheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo[INDICES]));
      glcheck(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW ));
    glcheck(glBindVertexArray( 0 ));
  }

  void
  mesh_renderable::do_render()
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
    glcheck(glDrawElements( GL_TRIANGLES, m_mesh.n_faces() * 3, GL_UNSIGNED_INT, (void*)0));
    glcheck(glBindVertexArray( 0 ) );
  }

  void
  mesh_renderable::remove_gpu_data()
  {
    if( m_vao )
      {
        glcheck(glDeleteBuffers( NUMBER_OF_VBO, m_vbo));
        glcheck(glDeleteVertexArrays( 1, &m_vao ));
        m_vao = 0;
      }
  }

  mesh_renderable::~mesh_renderable()
  {
    remove_gpu_data();
  }

  void
  mesh_renderable::load( const std::string& filename )
  {
    m_mesh.load( filename );
    m_dirty = true;
  }

}}
