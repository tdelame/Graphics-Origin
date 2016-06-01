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
    /**@brief A class to render in 3D an island map
     *
     * This class uses a noise module to generate an height map
     * for an island and render it in 3D. The map is a square, centered
     * at the origin, with a size equals to two time a radius R.
     */
    class island : public renderable
    {
    public:
      island();

      ~island()
      {
        remove_gpu_data();
      }

      /** @brief Set the desired horizontal resolution.
       *
       * Request for a particular horizontal resolution, i.e. what is the
       * minimum size of a tile to represent all the features you want.
       * @param resolution_in_m The resolution requested, in meter(s).
       */
      void
      set_resolution( gpu_real resolution_in_m = 1.0 );

      /** @brief Specify the height to generate an height map.
       *
       * Send a noise module such that an height map can be built to render
       * the whole scene.
       * @param land_generator A noise module that gives the height of a 2D point
       * @param texture_size Size of the height map texture to use internally.
       */
      void
      set_heightmap(
        noise::module::Module& land_generator,
        unsigned int texture_size = 2048 );

      /**@brief Specify the radius of the map.
       *
       * Set the radius of the map.
       * @param map_radius_in_m The new map radius in meters.
       */
      void
      set_radius(
        gpu_real map_radius_in_m );

      /**@brief Specify the maximum elevation of the map.
       *
       * Set the maximum elevation of the map. This will be sent to the GPU
       * to compute a color depending on the height of a point.
       * @param maximum_elevation_in_m The new maximum elevation in meters.
       */
      void
      set_maximum_elevation(
        gpu_real maximum_elevation_in_m );

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
      gpu_real m_maximum_elevation;
      unsigned int m_texture_size;
      unsigned int m_number_of_patches;


      enum
      {
        positions_texcoords_vbo_id, indices_vbo_id, number_of_vbos
      };
      unsigned int m_vao;
      unsigned int m_vbos[number_of_vbos];
      unsigned int m_texture_id;
    };
  }
}
#endif /* ISLAND_H_ */
