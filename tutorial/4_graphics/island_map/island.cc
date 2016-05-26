/*  Created on: May 26, 2016
 *      Author: T. Delame (tdelame@gmail.com) */
# include "island.h"
# include "../../../graphics-origin/application/gl_helper.h"
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

    island::island(
      noise::module::Module& land_generator,
      shader_program_ptr program,
      gpu_real resolution_in_km = 0.001,
      gpu_real map_radius,
      unsigned int texture_size = 2048 ) :
        m_map_radius{ map_radius }, m_resolution{ resolution_in_km },
        m_texture_size{ texture_size },

        m_vao{0}, m_vbos{ 0, 0 }
    {
      m_model = gpu_mat4(1.0);
      m_program = program;
    }

    void
    island::update_gpu_data()
    {
      if( !m_vao )
        {
          glcheck(glGenVertexArrays( 1, &m_vao ));
          glcheck(glGenBuffers( number_of_vbos, m_vbos ));
        }

      int position_location = m_program->get_attribute_location( "position" );
      int texture_location = m_program->get_attribute_location( "texture" );
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
      const unsigned int number_of_patches = std::ceil( 0.5 * m_map_radius / patch_size );
      patch_size = gpu_real(0.5) * m_map_radius / gpu_real( number_of_patches );

      glcheck(glBindVertexArray( m_vao ));
        try
          {
            std::vector< gpu_vec4 > positions(
                (number_of_patches + 1) * (number_of_patches + 1),
                gpu_vec4( 0, 0, 0, 1 ) );
            # pragma omp parallel for
            for( unsigned int j = 0; j <= number_of_patches; ++j )
              {
                gpu_vec4* row = positions.data( ) + j * (number_of_patches + 1);
                gpu_real y = -m_map_radius + j * patch_size;
                gpu_real x = -m_map_radius;
                for( unsigned int i = 0; i <= number_of_patches; ++i, ++row, x +=
                    patch_size )
                  {
                    row->x = x;
                    row->y = y;
                  }
              }
            glcheck(glBindBuffer( GL_ARRAY_BUFFER, m_vbos[ positions_vbo_id ] ));
            glcheck(glBufferData( GL_ARRAY_BUFFER, sizeof(gpu_vec4) * positions.size(), positions.data(), GL_STATIC_DRAW ));
            glcheck(glEnableVertexAttribArray( position_location ));
            glcheck(glVertexAttribPointer( position_location,
              4, GL_FLOAT, GL_FALSE, sizeof(gpu_vec4), 0 ));
          }
        catch( std::bad_alloc& e )
          {
            glcheck(glBindVertexArray( 0 ));
            remove_gpu_data();
            throw island_map_positions_overflow( __FILE__, __LINE__ );
          }

        try
          {
            std::vector< unsigned int > indices( number_of_patches * number_of_patches * 4, 0 );
            # pragma omp parallel for
            for( unsigned int j = 0; j < number_of_patches; ++ j )
              {
                unsigned int* row = indices.data() + number_of_patches * j * 4;
                for( unsigned int i = 0; i < number_of_patches; ++ i )
                  {
                    row[0] = (i + number_of_patches * j) << 2;
                    row[1] = row[0] + (number_of_patches << 2);
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
      glcheck(glBindVertexArray( 0 ));



      try
        {
          m_normal_height_texture.resize( m_texture_size * m_texture_size );
        }
      catch( ... )
        {
          throw island_map_texture_overflow( __FILE__, __LINE__ );
        }



      // fill positions and the texture

// transfer heights

// compute normals

    }

    void
    island::do_render()
    {
      // set patch vertices
      //drawElements GL_PATCHES
    }

    void
    island::set_radius(
      gpu_real map_radius_in_km )
    {
      if( map_radius_in_km > 0 )
        m_map_radius = map_radius_in_km;
    }

    gpu_real
    island::get_radius( ) const noexcept
    {
      return m_map_radius;
    }

    unsigned int
    island::get_texture_size( ) const noexcept
    {
      return m_texture_size;
    }

    float*
    island::get_raw_texture_pointer( )
    {
      return &m_normal_height_texture[0].x;
    }

    gpu_vec4*
    island::get_texture_pointer( )
    {
      return m_normal_height_texture.data( );
    }

  }
}

