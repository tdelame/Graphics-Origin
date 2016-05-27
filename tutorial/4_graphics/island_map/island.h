/*  Created on: May 25, 2016
 *      Author: T. Delame (tdelame@gmail.com) */
# ifndef ISLAND_H_
# define ISLAND_H_
# include "../../../graphics-origin/geometry/vec.h"
# include "../../../graphics-origin/application/renderable.h"
# include "../../../graphics-origin/application/shader_program.h"
# include <stdexcept>
# include <vector>
# include <noise/noise.h>
namespace graphics_origin
{
  namespace application
  {
    /**
     * Dummy class while the building process is not designed yet.
     */
    class island : public renderable
    {
    public:
      island(
        noise::module::Module& land_generator,
        shader_program_ptr program,
        gpu_real resolution_in_km = 0.001,
        gpu_real map_radius = 5,
        unsigned int texture_size = 2048 );

      island();

      ~island()
      {
        remove_gpu_data();
      }

      void
      set_resolution( gpu_real resolution_in_km = 0.001 );

      void
      set_heightmap(
        noise::module::Module& land_generator,
        unsigned int texture_size = 2048 );

      void
      set_radius(
        gpu_real map_radius_in_km );

      gpu_real
      get_radius( ) const noexcept;

      unsigned int
      get_texture_size( ) const noexcept;

      float*
      get_raw_texture_pointer( );

      gpu_vec4*
      get_texture_pointer( );

    private:
      void
      update_gpu_data( ) override;

      void
      do_render( ) override;

      void
      remove_gpu_data() override;

      std::vector< gpu_vec4 > m_normal_height_texture;
      gpu_real m_map_radius;
      gpu_real m_resolution;
      unsigned int m_texture_size;
      unsigned int m_number_of_patches;

      enum
      {
        positions_vbo_id, indices_vbo_id, number_of_vbos
      };
      unsigned int m_vao;
      unsigned int m_vbos[number_of_vbos];
      unsigned int m_texture_id;
    };
  }
}
#endif /* ISLAND_H_ */
