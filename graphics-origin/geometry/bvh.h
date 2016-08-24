# ifndef GRAPHICS_ORIGIN_BVH_H_
# define GRAPHICS_ORIGIN_BVH_H_
# include "../graphics_origin.h"
# include "box.h"
# include <vector>
namespace graphics_origin {
  namespace geometry {

    template< typename bounding_volume >
    struct bvh_builder;

    template<
        typename bounding_volume = aabox >
      class bvh {
      public:
        typedef uint32_t node_index;
        typedef uint32_t element_index;
        static constexpr size_t max_number_of_elements = (1U << uint8_t(sizeof(node_index)*8 - 1)) - 1;

        static_assert(
            std::is_default_constructible< bounding_volume >::value,
            "Bounding volume type must be default constructible to initialize the set of nodes.");

        struct node {
          bounding_volume bounding;
          node_index parent_index;
          union {
            node_index left_index;
            element_index element;
          };
          node_index right_index;
        };

        template< typename bounded_element >
        bvh( const bounded_element* elements, size_t number_of_elements );

        template< typename bounded_element >
        bvh(
            const bounded_element* elements,
            size_t number_of_elements,
            bounding_volume& root_bounding_volume );

        size_t get_number_of_nodes() const noexcept
        {
          return m_nodes.size();
        }

        size_t get_number_of_internal_nodes() const noexcept
        {
          return number_of_internal_nodes;
        }

        size_t get_number_of_leaf_nodes() const noexcept
        {
          return number_of_internal_nodes + 1;
        }

        const node& get_node( node_index index ) const
        {
          return m_nodes[ index ];
        }

        bool is_leaf( node_index index ) const noexcept
        {
          return index >= number_of_internal_nodes;
        }

        bool is_leaf( const node* pnode ) const noexcept
        {
          return std::distance( m_nodes.data(), pnode ) >= number_of_internal_nodes;
        }

      private:
        friend struct bvh_builder<bounding_volume>;
        const size_t number_of_internal_nodes;
        std::vector< node > m_nodes;
      };

    /**Customization point to compute a bounding volume of a specific type for
     * a bounded element of a specific type. */
    template< typename bounding_volume, typename bounded_element >
    struct bounding_volume_computer {
      static_assert(
         implementation_required<bounding_volume, bounded_element>::value,
         "Please, provide an implementation of bounding_volumes_computer for those specific bounding volume and bounded element types");

      static void compute( const bounded_element& element, bounding_volume& volume );
    };

    template< typename bounding_volume >
    struct bounding_volume_merger {
      static_assert(
         implementation_required<bounding_volume>::value,
         "Please, provide an implementation of bounding_volume_merger for this specific bounding volume type.");

      // Returns the bounding volume (preferably as minimal as possible) that
      // contains bounding volumes a and b.
      static bounding_volume merge( const bounding_volume& a, const bounding_volume& b );
    };

    /**Customization point to compute the corners of a bounding volume of a
     * specific type as well as it center. This is necessary to compute morton
     * codes. An implementation for balls and boxes is already given. */
    template< typename bounding_volume >
    struct bounding_volume_analyzer {

      static_assert(
          implementation_required<bounding_volume>::value,
          "Please, provide an implementation of bounding_volume_analyzer for this specific bounding volume type");

      /**The lower corner of a bounding volume is the point l such that for
       * every point p inside the bounding volume, l <= p. */
      static vec3 compute_lower_corner( const bounding_volume& volume );

      /**The upper corner of a bounding volume is the point u such that for
       * every point p inside the bounding volume, u >= p. */
      static vec3 compute_upper_corner( const bounding_volume& volume );

      /**In order to compute the morton code of a bounded element, we use the
       * position of its bounding volume center relatively to the lower and
       * upper corners. */
      static vec3 compute_center( const bounding_volume& volume );
    };
  }
}
# include "detail/bvh_implementation.h"
# include "detail/bvh_customization_points.h"
# endif
