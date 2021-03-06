# include "../../graphics-origin/application/renderables/meshes_renderable.h"
# include "../../graphics-origin/application/gl_helper.h"
# include "../../graphics-origin/application/camera.h"
# include "../../graphics-origin/application/renderer.h"
# include "../../graphics-origin/geometry/mesh.h"

# include <GL/glew.h>

namespace graphics_origin {
  namespace application {

    meshes_renderable::storage::storage()
      : mesh{ new geometry::mesh{} }, vao{ 0 }, dirty{ true }, active{ false }, destroyed{ false }
    {
      for( int i = 0; i < number_of_buffers; ++ i )
        {
          buffer_ids[ i ] = 0;
        }
    }

    meshes_renderable::storage&
    meshes_renderable::storage::operator=( storage&& other )
    {
      std::swap( mesh, other.mesh );
      std::swap( vao, other.vao );
      std::swap( dirty, other.dirty );
      std::swap( active, other.active );
      std::swap( destroyed, other.destroyed );
      for( int i = 0; i < number_of_buffers; ++ i )
        {
          std::swap( buffer_ids[ i ], other.buffer_ids[ i ] );
        }
      return *this;
    }

    meshes_renderable::storage::~storage()
    {
      delete mesh;
    }

    meshes_renderable::meshes_renderable( shader_program_ptr program )
    {
      model = gl_mat4(1.0);
      this->program = program;
    }

    meshes_renderable::~meshes_renderable()
    {
      remove_gpu_data();
    }

    void
    meshes_renderable::remove_gpu_data()
    {
	  storage* data = m_meshes.data();
	  for( size_t i = 0; i < m_meshes.get_size(); ++i, ++data )
	    {
		  glcheck(glDeleteVertexArrays(1, &data->vao));
		  glcheck(glDeleteBuffers( number_of_buffers, data->buffer_ids));
	    }
    }

    void
    meshes_renderable::update_gpu_data()
    {
      std::vector< gl_real > positions_normals;
      std::vector< unsigned int > indices;
      storage* data = m_meshes.data();
      for( size_t i = 0; i < m_meshes.get_size(); ++i, ++data)
        {
          if( data->destroyed )
            {
              glcheck( glDeleteVertexArrays( 1, &data->vao ));
              glcheck( glDeleteBuffers( number_of_buffers, data->buffer_ids ));
              // reset the storage
              *data = storage{};
              // remove from the buffer
              m_meshes.remove( data );
            }
          else
            {
              if( data->dirty && data->mesh )
                {
                  if( !data->vao )
                    {
                      glcheck( glGenVertexArrays( 1, &data->vao ));
                      glcheck( glGenBuffers( number_of_buffers, data->buffer_ids ));
                    }
                  const auto nvertices = data->mesh->n_vertices();
                  positions_normals.resize( nvertices * 6 ); // fvec3 + fvec3
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
                      auto& point = data->mesh->point( vh );
                      auto& normal = data->mesh->normal( vh );

                      auto dst = positions_normals.data() + 6 * i;
                      dst[0] = point[0];
                      dst[1] = point[1];
                      dst[2] = point[2];
                      dst[3] = normal[0];
                      dst[4] = normal[1];
                      dst[5] = normal[2];
                    }

                  const auto nfaces = data->mesh->n_faces();
                  indices.resize( nfaces * 3 );
              # ifdef _WIN32
              # pragma message("MSVC does not allow unsigned index variable in OpenMP for statement")
              # pragma omp parallel for schedule(static)
                for (long i = 0; i < nfaces; ++i)
              # else
                  # pragma omp parallel for schedule(static)
                  for( size_t i = 0; i < nfaces; ++ i )
              # endif
                    {
                      auto fvit = data->mesh->fv_begin( geometry::mesh::FaceHandle( i ) );
                      auto dst = indices.data() + 3 * i;
                      dst[ 0 ] = fvit->idx(); ++ fvit;
                      dst[ 1 ] = fvit->idx(); ++ fvit;
                      dst[ 2 ] = fvit->idx();
                    }

                  int position_location = program->get_attribute_location( "position" );
                  int   normal_location = program->get_attribute_location( "normal"   );

                  glcheck(glBindVertexArray( data->vao ));
                    glcheck(glBindBuffer( GL_ARRAY_BUFFER, data->buffer_ids[ position_normal_vbo ] ));
                    glcheck(glBufferData( GL_ARRAY_BUFFER, positions_normals.size() * sizeof( gl_real ), positions_normals.data(), GL_STATIC_DRAW ));

                    glcheck(glEnableVertexAttribArray( position_location ));
                    glcheck(glVertexAttribPointer( position_location,
                      3, GL_FLOAT, GL_FALSE,
                      6 * sizeof( gl_real ),
                      reinterpret_cast<void*>( 0 )));

                    glcheck(glEnableVertexAttribArray( normal_location ));
                    glcheck(glVertexAttribPointer( normal_location,
                      3, GL_FLOAT, GL_FALSE,
                      6 * sizeof( gl_real ),
                      reinterpret_cast<void*>( 3 * sizeof( gl_real ))));

                    glcheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->buffer_ids[ indices_vbo ]));
                    glcheck(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW ));
                  glcheck(glBindVertexArray( 0 ));
                }

            }
        }
    }

    void
    meshes_renderable::do_render()
    {
      shader_program::identifier location = program->get_uniform_location( "window_dimensions");
      if( location != shader_program::null_identifier )
        glcheck(glUniform2fv( location, 1, glm::value_ptr( renderer_ptr->get_window_dimensions())));
      glcheck(glUniform3fv( program->get_uniform_location( "light_position"), 1, glm::value_ptr( renderer_ptr->get_camera_position() )));
      glcheck(glUniformMatrix4fv( program->get_uniform_location( "model"), 1, GL_FALSE, glm::value_ptr( model )));
      glcheck(glUniformMatrix4fv( program->get_uniform_location( "view"), 1, GL_FALSE, glm::value_ptr( renderer_ptr->get_view_matrix() )));
      glcheck(glUniformMatrix4fv( program->get_uniform_location( "projection"), 1, GL_FALSE, glm::value_ptr( renderer_ptr->get_projection_matrix())));
      glcheck(glUniformMatrix3fv( program->get_uniform_location( "nit" ), 1, GL_FALSE, glm::value_ptr( gl_mat3( glm::transpose( glm::inverse( model ) ) ) ) ));

      storage* data = m_meshes.data();
      for( size_t i = 0; i < m_meshes.get_size(); ++ i, ++ data )
        {
          if( data->active )
            {
              glcheck( glBindVertexArray( data->vao ));
              glcheck( glDrawElements( GL_TRIANGLES, data->mesh->n_faces() * 3, GL_UNSIGNED_INT, 0 ));
            }
        }
      glcheck(glBindVertexArray( 0 ));
    }

    meshes_renderable::handle
    meshes_renderable::add( const std::string& mesh_filename )
    {
      auto pair = m_meshes.create();
      pair.second.mesh->load( mesh_filename );
      pair.second.dirty = true;
      return pair.first;
    }

    void
    meshes_renderable::remove( handle h)
    {
      if( h.is_valid() )
        m_meshes.get( h ).destroyed = true;
    }

     meshes_renderable::storage&
     meshes_renderable::get( handle h)
     {
       return m_meshes.get( h );
     }
  }
}
