/*  Created on: May 26, 2016
 *      Author: T. Delame (tdelame@gmail.com) */
# include "island.h"
# include "../../../graphics-origin/application/gl_helper.h"
# include "../../../graphics-origin/application/gl_window_renderer.h"
# include "../../../graphics-origin/tools/log.h"
# include <GL/glew.h>


//temp
# include <noise/noiseutils.h>
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
      : m_map_radius{ 5 },
        m_resolution{ 0.001 },
        m_texture_size{ 2048 }, m_number_of_patches{0},
        m_vao{0}, m_vbos{ 0, 0 }, m_texture_id{0}
    {
      m_model = gpu_mat4(1.0);
    }

    island::island(
      noise::module::Module& land_generator,
      shader_program_ptr program,
      gpu_real resolution_in_km,
      gpu_real map_radius,
      unsigned int texture_size) :
        m_number_of_patches{0}, m_vao{0}, m_vbos{ 0, 0 }
    {
      m_model = gpu_mat4(1.0);
      m_program = program;

      set_radius( map_radius );
      set_resolution( resolution_in_km );
      set_heightmap( land_generator, texture_size );
    }

    void
    island::set_resolution( gpu_real resolution_in_km )
    {
      m_resolution = resolution_in_km;
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

          {
            noise::utils::NoiseMap noise_map;
            noise::utils::NoiseMapBuilderPlane noise_map_builder;
            noise_map_builder.SetSourceModule( land_generator );
            noise_map_builder.SetDestNoiseMap( noise_map );
            noise_map_builder.SetDestSize( m_texture_size, m_texture_size );
            noise_map_builder.SetBounds( -m_map_radius, m_map_radius, -m_map_radius, m_map_radius );
            noise_map_builder.Build();

            noise::utils::Image noise_image;
            noise::utils::RendererImage renderer;
            renderer.SetSourceNoiseMap( noise_map );
            renderer.SetDestImage( noise_image );
            renderer.ClearGradient();
            renderer.AddGradientPoint(-1.0000, noise::utils::Color(  0,   0, 128, 255)); // deeps
            renderer.AddGradientPoint(-0.2500, noise::utils::Color(  0,   0, 255, 255)); // shallow
            renderer.AddGradientPoint( 0.0000, noise::utils::Color(  0, 128, 255, 255)); // shore
            renderer.AddGradientPoint( 0.0625, noise::utils::Color(240, 240,  64, 255)); // sand
            renderer.AddGradientPoint( 0.1250, noise::utils::Color( 32, 160,   0, 255)); // grass
            renderer.AddGradientPoint( 0.3750, noise::utils::Color(224, 224,   0, 255)); // dirt
            renderer.AddGradientPoint( 0.7500, noise::utils::Color(128, 128, 128, 255)); // rock
            renderer.AddGradientPoint( 1.0000, noise::utils::Color(255, 255, 255, 255)); // snow
            renderer.EnableLight();
            renderer.SetLightContrast(3.0);
            renderer.SetLightBrightness(2.0);
            renderer.Render();

            noise::utils::WriterBMP writer;
            writer.SetSourceImage( noise_image );
            writer.SetDestFilename( "land.bmp" );
            writer.WriteDestFile();

            for( unsigned int j = 0; j < m_texture_size; ++ j )
              {
                for( unsigned int i = 0; i < m_texture_size; ++ i )
                  {
                    m_normal_height_texture[ i + j * m_texture_size ].a = noise_map.GetValue( i, j );
                  }
              }

          }



//          const float step = m_map_radius * 2.0f / float( m_texture_size );
//
//          # pragma omp parallel for
//          for( unsigned int j = 0; j < m_texture_size; ++ j )
//            {
//              float y = -m_map_radius + j * step;
//              float x = -m_map_radius           ;
//              gpu_vec4* dest = m_normal_height_texture.data() + j * m_texture_size;
//              for( unsigned int i = 0; i < m_texture_size; ++ i, x += step, ++ dest )
//                {
//                  dest->a = land_generator.GetValue( x, y, 0 );
//                }
//            }
          set_dirty();

          //todo: compute normals
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
      int texture_location = m_program->get_attribute_location( "terrain" );
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
                gpu_vec4( 0, 0, 0, 1 ) );
            # pragma omp parallel for
            for( unsigned int j = 0; j <= m_number_of_patches; ++j )
              {
                gpu_vec4* row = positions.data( ) + j * (m_number_of_patches + 1);
                gpu_real y = -m_map_radius + j * patch_size;
                gpu_real x = -m_map_radius;
                for( unsigned int i = 0; i <= m_number_of_patches; ++i, ++row, x +=
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
//      glcheck(glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ));
      glcheck(glUniform2fv( m_program->get_uniform_location( "window_dimensions"), 1, glm::value_ptr( m_renderer->get_window_dimensions())));
      glcheck(glUniformMatrix4fv( m_program->get_uniform_location( "mvp"), 1, GL_FALSE, glm::value_ptr( m_renderer->get_projection_matrix() * m_renderer->get_view_matrix())));
      glcheck(glUniformMatrix3fv( m_program->get_uniform_location( "nit" ), 1, GL_FALSE, glm::value_ptr( gpu_mat3( glm::transpose( glm::inverse( m_model ) ) ) ) ));
      glcheck(glUniform1f( m_program->get_uniform_location( "lod_factor"), 4.0f ));
      glcheck(glUniform1f( m_program->get_uniform_location( "inv_radius"), gpu_real(1.0/m_map_radius) ));
      glcheck(glPatchParameteri( GL_PATCH_VERTICES, 4 ));

      glcheck(glBindTexture(GL_TEXTURE_2D, m_texture_id ));
      glcheck(glUniform1i( m_program->get_uniform_location( "terrain"), 0));

      glcheck(glBindVertexArray( m_vao ));
      glcheck(glDrawElements( GL_PATCHES, m_number_of_patches * m_number_of_patches * 4, GL_UNSIGNED_INT, 0 ));
      glcheck(glBindVertexArray( 0 ));
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

