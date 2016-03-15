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
# include <omp.h>

BEGIN_GO_NAMESPACE
namespace geometry {

  template<
    typename bounding_object = aabox >
  class bvh {
  public:
    static_assert(
        geometric_traits<bounding_object>::is_bounding_volume_merger,
        "The bounding objects must be bounding volume and must be able to merge themselves");
    struct internal_node {
      internal_node();
      bounding_object bounding;
      uint32_t parent_index;
      uint32_t left_index;
      uint32_t right_index;
    };
    struct leaf_node {
      leaf_node();
      bounding_object bounding;
      uint64_t morton_code;
      uint32_t parent_index;
      uint32_t element_index;
    };

    template< typename bounded_element >
    bvh( const bounded_element* elements, size_t number_of_elements );

    size_t get_number_of_nodes() const
    {
      return get_number_of_internal_nodes() + get_number_of_leaf_nodes();
    }
    size_t get_number_of_internal_nodes() const
    {
      return m_internals.size();
    }
    size_t get_number_of_leaf_nodes() const
    {
      return m_leaves.size();
    }

    const internal_node&
    get_internal_node( uint32_t node_index ) const
    {
      return m_internals[ node_index ];
    }

    const leaf_node&
    get_leaf_node( uint32_t node_index ) const
    {
      return m_leaves[ node_index ];
    }

  private:
    static const size_t max_number_of_elements = (1U << uint8_t(32)) - 1;

    const size_t m_elements;

    std::vector< leaf_node > m_leaves;
    std::vector< internal_node > m_internals;
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
      typename bvh<bounding_object>::leaf_node* leaves,
      size_t number_of_leaves );
  };

  /**@brief Order BVH leaf nodes by increasing Morton code.
   *
   * This structure orders BVH leaf nodes by increasing Morton code. It is
   * used by set_leaf_nodes. */
  template< typename bounding_object >
  struct morton_code_order {
    bool
    operator()( const typename bvh<bounding_object>::leaf_node& a, const  typename bvh<bounding_object>::leaf_node& b ) const
    {
      return a.morton_code < b.morton_code;
    }
  };


  /** BVH with boxes as bounding volume.
   *
   *TODO: put the element buffer in a temporary variable. Since after the construction,
   *TODO: the buffer does not matter anymore.
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
      if( number_of_elements < 2 )
        {
          LOG( error, "not enough elements to create a box bvh.");
          return;
        }
      leaves.resize( number_of_elements );
      internals.resize( number_of_elements - 1 );


      auto time = omp_get_wtime();
      set_leaf_nodes();
      time = omp_get_wtime() - time;
      LOG( debug, "leaves    = " << time );

      time = omp_get_wtime();
      set_nodes_hierarchy();
      time = omp_get_wtime() - time;
      LOG( debug, "leaves    = " << time );

      time = omp_get_wtime();
      set_internal_bounding_boxes();
      time = omp_get_wtime() - time;
      LOG( debug, "leaves    = " << time );
    }

    const internal_node&
    get_root() const;

    bool is_leaf( uint32_t node_index ) const;

    const internal_node&
    get_internal_node( uint32_t node_index ) const;

    const leaf_node&
    get_leaf_node( uint32_t node_index ) const;

    size_t get_number_of_nodes() const;
    size_t get_number_of_internal_nodes() const;
    size_t get_number_of_leaf_nodes() const;

  private:

//    struct cpu_bvh_builder {
//      void set_leaf_nodes();
//      void set_nodes_hierarchy();
//      void set_internal_bounding_boxes();
//      box_bvh* parent;
//    };


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

# include "detail/bvh_implementation.tcc"
# endif
