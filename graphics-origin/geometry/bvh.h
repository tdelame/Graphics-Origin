/*  Created on: Jan 24, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */

# ifndef GRAPHICS_ORIGIN_BVH_H_
# define GRAPHICS_ORIGIN_BVH_H_
# include "../graphics_origin.h"
# include "concepts/bounding_box_computer.h"
# include "traits.h"
# include "box.h"
# include "../tools/log.h"

# include <vector>
# include <type_traits>

BEGIN_GO_NAMESPACE
namespace geometry {

  template<
    typename bounding_object = aabox >
  class bvh {
  public:
    static_assert(
        geometric_traits<bounding_object>::is_bounding_volume_merger,
        "The bounding objects must be bounding volume and must be able to merge themselves");

    /**We store indices in uint32_t. We then have 2^32 nodes in total. Those nodes
     * are composed of n - 1 internal nodes and n leaves, for a bvh of n primitives.
     * The maximum number of primitives is the solution of
     * 2 n = 2^32 - 1
     * thus n = 2^31 - 1 (since n is an integer).
     *
     * The node of index i is a leaf if i >= n - 1
     */

    /**@brief A unique structure to represent both internal and leaf nodes.
     *
     *
     *
     */
    struct node {
      node();
      bounding_object bounding;
      uint32_t parent_index;
      union {
        uint32_t left_index;
        uint32_t element_index;
      };
      uint32_t right_index;
    };

    template< typename bounded_element >
    bvh( const bounded_element* elements, size_t number_of_elements );

    template< typename bounded_element >
    bvh( const bounded_element* elements, bounding_object& root_bounding_object, size_t number_of_elements );

    size_t get_number_of_nodes() const
    {
      return m_nodes.size();
    }

    size_t get_number_of_internal_nodes() const
    {
      return m_number_of_internal_nodes;
    }

    size_t get_number_of_leaf_nodes() const
    {
      return m_number_of_internal_nodes + 1;
    }

    const node&
    get_node( uint32_t node_index ) const
    {
      return m_nodes[ node_index ];
    }

    bool is_leaf( uint32_t node_index ) const
    {
      return node_index >= m_number_of_internal_nodes;
    }

    bool is_leaf( const node* pnode ) const
    {
      return std::distance( m_nodes.data(), pnode ) >= m_number_of_internal_nodes;
    }

  private:
    const size_t m_number_of_internal_nodes;
    std::vector< node > m_nodes;
  };

  /**@brief Create the leaf nodes of a BVH.
   *
   * This structure set the leaves of a BVH. It is specialized to build BVH with
   * balls or boxes as bounding volumes. If you want to have other type of bounding
   * volumes, have a look at the implementation and specialize this structure with
   * the type you want.
   */
  template< typename bounding_object, typename bounded_element >
  struct set_leaf_nodes {
    set_leaf_nodes(
      const bounded_element* elements,
      std::vector<typename bvh<bounding_object>::node>& nodes,
      size_t number_of_internals,
      std::vector<uint64_t>& morton_codes);

    set_leaf_nodes(
      const bounded_element* elements,
      bounding_object& root_bounding_object,
      std::vector<typename bvh<bounding_object>::node>& nodes,
      size_t number_of_internals,
      std::vector<uint64_t>& morton_codes );
  };

}
END_GO_NAMESPACE

# include "detail/bvh_implementation.h"
# endif
