/*  Created on: Jan 24, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */

# ifndef GRAPHICS_ORIGIN_BVH_H_
# define GRAPHICS_ORIGIN_BVH_H_
# include "../graphics_origin.h"
# include "./concepts/bounding_box_computer.h"
# include "./box.h"
# include "../tools/log.h"

# include <vector>

BEGIN_GO_NAMESPACE
namespace geometry {

  /** BVH with boxes as bounding volume.
   *
   */
  class box_bvh {
  public:
    struct internal_node {
      internal_node();
      aabox bounding;
      uint32_t parent_index;
      uint32_t left_index;
      uint32_t right_index;
    };

    struct leaf_node {
      leaf_node();
      aabox bounding;
      uint64_t morton_code;
      uint32_t parent_index;
      uint32_t element_index;
    };

    template< typename element >
    box_bvh( const element* elements, size_t number_of_elements )
      : sizeof_element{ sizeof( element ) },
        number_of_elements{ number_of_elements },
        elements{ elements }
    {
      if( !number_of_elements )
        {
          LOG( error, "no elements to create a box bvh.");
        }
      leaves.resize( number_of_elements );
      internals.resize( number_of_elements - 1 );
      set_leaf_nodes();
      set_nodes_hierarchy();
      set_internal_bounding_boxes();
    }

    const internal_node&
    get_root() const;

  private:
    void set_leaf_nodes();
    void set_nodes_hierarchy();
    void set_internal_bounding_boxes();

    int longest_common_prefix( const uint64_t& c1, int j );
    uivec2 determine_range( uint32_t i );
    uint32_t find_split( const uivec2& range );

    const size_t sizeof_element;
    const size_t number_of_elements;
    const bounding_box_computer* elements;

    std::vector< leaf_node > leaves;
    std::vector< internal_node > internals;
  };
}
END_GO_NAMESPACE
# endif
