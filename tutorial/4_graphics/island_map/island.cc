/*  Created on: May 26, 2016
 *      Author: T. Delame (tdelame@gmail.com) */
# include "island.h"
# include "../../../graphics-origin/application/gl_helper.h"
# include "../../../graphics-origin/application/camera.h"
# include "../../../graphics-origin/application/gl_window_renderer.h"
# include "../../../graphics-origin/tools/log.h"
# include <GL/glew.h>

namespace graphics_origin
{
  namespace application
  {
    struct island_map_texture_overflow : public std::runtime_error
    {
      island_map_texture_overflow(
        const std::string& file,
        size_t line ) :
          std::runtime_error(
              "cannot allocate enough space for the texture at line "
                  + std::to_string( line ) + " of file " + file )
      {
      }
    };
    struct island_map_positions_overflow : public std::runtime_error
    {
      island_map_positions_overflow(
        const std::string& file,
        size_t line ) :
          std::runtime_error(
              "cannot allocate enough space for the positions buffer at line "
                  + std::to_string( line ) + " of file " + file )
      {
      }
    };
    struct island_map_indices_overflow : public std::runtime_error
    {
      island_map_indices_overflow(
        const std::string& file,
        size_t line ) :
          std::runtime_error(
              "cannot allocate enough space for the indices buffer at line "
                  + std::to_string( line ) + " of file " + file )
      {
      }
    };

    island::island()
      : m_map_radius{ 5000 },
        m_resolution{ 1 }, m_maximum_elevation{300},
        m_texture_size{ 2048 }, m_number_of_patches{0},
        m_vao{0}, m_vbos{ 0, 0 }, m_texture_id{0}
    {
      m_model = gpu_mat4(1.0);
    }

    void
    island::set_resolution( gpu_real resolution_in_m )
    {
      m_resolution = resolution_in_m;
      set_dirty();
    }

    void
    island::set_heightmap(
      noise::module::Module& land_generator,
      unsigned int texture_size )
    {
      try
        {
          m_texture_size = texture_size;
          m_normal_height_texture.resize( m_texture_size * m_texture_size, gpu_vec4{} );
          const float extent = m_map_radius * 2.0f ;
          const float step = extent / float( m_texture_size );

          // compute the height for every texel of the texture
          # pragma omp parallel for
          for( unsigned int j = 0; j < m_texture_size; ++ j )
            {
              float y = -m_map_radius + j * step;
              float x = -m_map_radius           ;
              gpu_vec4* dest = m_normal_height_texture.data() + j * m_texture_size;
              for( unsigned int i = 0; i < m_texture_size; ++ i, x += step, ++ dest )
                {
                  dest->a = land_generator.GetValue( x, y, 0 );
                }
            }

          // compute the normal for every texel of the texture
          const gpu_real scale = gpu_real(2.0) * m_map_radius / gpu_real( m_texture_size );
          # pragma omp parallel for
          for( unsigned int j = 0; j < m_texture_size; ++ j )
            {
              gpu_vec4* dest = m_normal_height_texture.data() + j * m_texture_size;
              for( unsigned int i = 0; i < m_texture_size; ++ i, ++ dest )
                {
                  float nc = dest->a;
                  float nu = dest[ (((j + 1) == m_texture_size) ? 0 : 1) * m_texture_size ].a;
                  float nr = dest[ (((i + 1) == m_texture_size) ? 0 : 1) ].a;

                  dest->x = nc - nr;
                  dest->y = nc - nu;
                  float inv_norm = float(1.0) / std::sqrt( dest->x * dest->x + dest->y * dest->y + 1.0f );
                  dest->x *= inv_norm;
                  dest->y *= inv_norm;
                  dest->z = inv_norm;
                }
            }

          // when the texture has changed, all data need to be resent to the GPU
          set_dirty();
        }
      catch( std::bad_alloc& e )
        {
          throw island_map_texture_overflow( __FILE__, __LINE__ );
        }
    }

    void
    island::update_gpu_data()
    {
      if( !m_vao )
        {
          glcheck(glGenVertexArrays( 1, &m_vao ));
          glcheck(glGenBuffers( number_of_vbos, m_vbos ));
          glcheck(glGenBuffers( 1, &m_texture_id ));
        }

      int position_location = m_program->get_attribute_location( "position" );
      int texcoord_location = m_program->get_attribute_location( "texcoord" );
      int texture_location = m_program->get_uniform_location( "terrain" );
      int max_tess_levels    = 64; //minimum in the specification
      glcheck(glGetIntegerv( GL_MAX_TESS_GEN_LEVEL, &max_tess_levels));

      /**
       * TODO: make a tutorial on tessellation.
       *
       * We have a square of length 2 * m_map_radius, centered at the origin to
       * render. We will use quad patches for the tessellation, i.e. we will
       * build triangles from quads by subdividing them depending on the distance
       * to camera.
       *
       * To have a quad of the requested resolution, we can subdivide a patch
       * max_tess_levels x max_tess_levels times. Thus, to have a minimum
       * number of patch, each patch has the following size: */
      gpu_real patch_size = m_resolution * max_tess_levels;

      /**
       * Now, by dividing 2.0 x m_map_radius by patch_size, we have the ideal number
       * of patches. Since there is no guarantee the result will be an integer
       * we have to update a little patch_size
       */
      m_number_of_patches = std::ceil( 2.0 * m_map_radius / patch_size );
      patch_size = gpu_real(2.0) * m_map_radius / gpu_real( m_number_of_patches );

      glcheck(glBindVertexArray( m_vao ));
        try
          {
            std::vector< gpu_vec4 > positions(
                (m_number_of_patches + 1) * (m_number_of_patches + 1),
                gpu_vec4( 0, 0, 0, 0 ) );

            const gpu_real inv_double_radius = gpu_real(1.0) / (gpu_real(2.0) * m_map_radius );
            # pragma omp parallel for
            for( unsigned int j = 0; j <= m_number_of_patches; ++j )
              {
                gpu_vec4* row = positions.data( ) + j * (m_number_of_patches + 1);
                gpu_real y = -m_map_radius + j * patch_size;
                gpu_real x = -m_map_radius;
                gpu_real ty = y * inv_double_radius + gpu_real(0.5);
                for( unsigned int i = 0; i <= m_number_of_patches; ++i, ++row, x += patch_size )
                  {
                    row->x = x;
                    row->y = y;
                    row->z = x * inv_double_radius + gpu_real(0.5);
                    row->w = ty;
                  }
              }

            glcheck(glBindBuffer( GL_ARRAY_BUFFER, m_vbos[ positions_texcoords_vbo_id ] ));
            glcheck(glBufferData( GL_ARRAY_BUFFER, sizeof(gpu_vec4) * positions.size(), positions.data(), GL_STATIC_DRAW ));
            glcheck(glEnableVertexAttribArray( position_location ));
            glcheck(glVertexAttribPointer( position_location,
              2, GL_FLOAT, GL_FALSE, sizeof(gpu_vec4), 0 ));
            glcheck(glEnableVertexAttribArray( texcoord_location ));
            glcheck(glVertexAttribPointer( texcoord_location,
              2, GL_FLOAT, GL_FALSE, sizeof(gpu_vec4), reinterpret_cast<void*>( 2 * sizeof(gpu_real))));
          }
        catch( std::bad_alloc& e )
          {
            glcheck(glBindVertexArray( 0 ));
            remove_gpu_data();
            throw island_map_positions_overflow( __FILE__, __LINE__ );
          }

        try
          {
            std::vector< unsigned int > indices( m_number_of_patches * m_number_of_patches * 4, 0 );
            # pragma omp parallel for
            for( unsigned int j = 0; j < m_number_of_patches; ++ j )
              {
                unsigned int* row = indices.data() + m_number_of_patches * j * 4;
                unsigned int first_index = (m_number_of_patches + 1 )* j;
                for( unsigned int i = 0; i < m_number_of_patches; ++ i, row += 4, ++first_index )
                  {
                    row[0] = first_index;
                    row[1] = row[0] + m_number_of_patches + 1;
                    row[2] = row[1] + 1;
                    row[3] = row[0] + 1;
                  }
              }

            glcheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbos[ indices_vbo_id ] ));
            glcheck(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW ));
          }
        catch( std::bad_alloc& e )
          {
            glcheck(glBindVertexArray( 0 ));
            remove_gpu_data();
            throw island_map_indices_overflow( __FILE__, __LINE__ );
          }


        glcheck(glActiveTexture(GL_TEXTURE0));
        // create and setup the texture
        glcheck(glBindTexture(GL_TEXTURE_2D, m_texture_id ));
        glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        glcheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

        // load the texture
        glcheck(glTexImage2D(GL_TEXTURE_2D,
           0,
           GL_RGBA32F, m_texture_size, m_texture_size, 0, // format, width, height, border
           GL_RGBA,
           GL_FLOAT, (const GLvoid*)&m_normal_height_texture[0].x));
        glcheck(glUniform1i(texture_location, 0));


      glcheck(glBindVertexArray( 0 ));
      glcheck(glBindBuffer( GL_ARRAY_BUFFER, 0));
      glcheck(glBindTexture(GL_TEXTURE_2D, 0));
    }

    void
    island::remove_gpu_data()
    {
      glcheck(glDeleteVertexArrays( 1, &m_vao ));
      glcheck(glDeleteBuffers( number_of_vbos, m_vbos ));
      glcheck(glDeleteTextures( 1, &m_texture_id ));
    }

    void
    island::do_render()
    {
      glcheck(glUniform2fv( m_program->get_uniform_location( "window_dimensions"), 1, glm::value_ptr( m_renderer->get_window_dimensions())));
      glcheck(glUniformMatrix4fv( m_program->get_uniform_location( "mvp"), 1, GL_FALSE, glm::value_ptr( m_renderer->get_projection_matrix() * m_renderer->get_view_matrix())));
      glcheck(glUniform1f( m_program->get_uniform_location( "lod_factor"), 4.0f ));
      glcheck(glUniform1f( m_program->get_uniform_location( "maximum_elevation"), m_maximum_elevation ));
      glcheck(glUniform3fv( m_program->get_uniform_location( "camera_position"), 1, glm::value_ptr( m_renderer->get_camera()->get_position())));
      glcheck(glPatchParameteri( GL_PATCH_VERTICES, 4 ));

      glcheck(glBindTexture(GL_TEXTURE_2D, m_texture_id ));
      glcheck(glUniform1i( m_program->get_uniform_location( "terrain"), 0));

      glcheck(glBindVertexArray( m_vao ));
      glcheck(glDrawElements( GL_PATCHES, m_number_of_patches * m_number_of_patches * 4, GL_UNSIGNED_INT, 0 ));
      glcheck(glBindVertexArray( 0 ));
    }

    void
    island::set_radius(
      gpu_real map_radius_in_m )
    {
      if( map_radius_in_m > 0 )
        m_map_radius = map_radius_in_m;
    }

    void
    island::set_maximum_elevation(
      gpu_real maximum_elevation_in_m )
    {
      if( maximum_elevation_in_m > 0 )
        m_maximum_elevation = maximum_elevation_in_m;
    }
  }
}

