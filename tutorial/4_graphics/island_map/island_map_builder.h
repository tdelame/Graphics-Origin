/*  Created on: May 25, 2016
 *      Author: T. Delame (tdelame@gmail.com) */

#ifndef ISLAND_MAP_BUILDER_H_
#define ISLAND_MAP_BUILDER_H_

# include <noise/noise.h>
namespace graphics_origin {
  namespace application {

    class island;

    /**
     * A dummy builder for now.
     * It will be necessary later on to separate the building process from the island map itself
     */
    class island_map_builder {
    public:
      struct parameters {
        parameters();

        // land module: determine which part of the space is land/water
        /**
         * Base frequency for the land module. The default value is 1.
         */
        float m_land_frequency;
        /**
         * Frequency multiplier between two consecutive octaves for the land module.
         * The default value is 2.0.
         */
        float m_land_lacunarity;
        /**
         * Amplitude multiplier between two consecutive octaves for the land module.
         * The default value is 0.5.
         */
        float m_land_persistance;
        /**
         * A value in [0,1] to determine the threshold between water and land.
         * When it increases, there will be less and less lands. A good value
         * would be (empirically) between 0.1 and 0.4. */
        float m_land_threshold;
        /**
         * Number of octaves in the land module. The default value is 4.
         */
        int m_land_octaves;
        /**
         * Seed for the land module. The default value is initialized thanks to
         * the clock time.
         */
        int m_land_seed;

        /**
         * Radius of the map, in kilometer. The default value is 5.
         */
        float m_radius;
      };

      island_map_builder(
        const parameters& params,
        island& output );


    private:
      void output_results();

      const parameters& m_params;
      island& m_output;
    };

  }
}



#endif /* ISLAND_MAP_BUILDER_H_ */
