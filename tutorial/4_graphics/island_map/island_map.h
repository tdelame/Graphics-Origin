/*  Created on: May 25, 2016
 *      Author: T. Delame (tdelame@gmail.com) */
# ifndef ISLAND_MAP_H_
# define ISLAND_MAP_H_
# include "../../../graphics-origin/geometry/vec.h"
# include <stdexcept>
namespace graphics_origin {
  namespace application {
    struct island_map_texture_overflow
        : public std::runtime_error
    {
      island_map_texture_overflow( const std::string& file, size_t line )
          : std::runtime_error( "cannot allocate enough space for the texture at line "
              + std::to_string( line ) + " of file " + file )
      {}
    };

    /**
     * Dummy class while the building process is not designed yet.
     */
    class island_map {
    public:
      island_map()
        : m_map_radius{ 1 },
          m_texture_size{ 0 }
      {}

      void set_radius( gpu_real map_radius_in_km )
      {
        if( map_radius_in_km > 0 )
          m_map_radius = map_radius_in_km;
      }

      gpu_real get_radius() const noexcept
      {
        return m_map_radius;
      }

      unsigned int get_texture_size() const noexcept
      {
        return m_texture_size;
      }

      void allocate_texture( unsigned int texture_size )
      {
        try
          {
            m_normal_height_texture.resize( texture_size * texture_size );
          }
        catch( ... )
          {
            throw island_map_texture_overflow( __FILE__, __LINE__ );
          }
        m_texture_size = texture_size;
      }

      float* get_raw_texture_pointer()
      {
        return &m_normal_height_texture[0].x;
      }

      gpu_vec4* get_texture_pointer()
      {
        return m_normal_height_texture.data();
      }

    private:
      std::vector<gpu_vec4> m_normal_height_texture;
      gpu_real m_map_radius;
      unsigned int m_texture_size;
    };
  }
}
#endif /* ISLAND_MAP_H_ */
