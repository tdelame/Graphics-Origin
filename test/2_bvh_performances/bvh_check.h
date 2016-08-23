# ifndef BVH_CHECK_H_
# define BVH_CHECK_H_
# include <vector>
# include <list>

//fixme: temp
# include <iostream>
namespace graphics_origin {
  namespace geometry {
    template< typename bvh_type >
    bool check_one_leaf_by_object( bvh_type& tree )
    {
      if( tree.get_number_of_nodes() == 0 )
        return false;
      size_t nb_elements = tree.get_number_of_leaf_nodes();
      std::vector<bool> marked( nb_elements, false );
      size_t idx = tree.get_number_of_internal_nodes();
      for( size_t i = 0; i < nb_elements; ++ i, ++ idx )
        {
          uint32_t elt = tree.get_node( idx ).element;
          if( marked[ elt ] )
            return false;
          marked[ elt ] = true;
        }
      return true;
    }

    template< typename bvh_type >
    bool check_is_tree( bvh_type& tree )
    {
      if( tree.get_number_of_nodes() == 0 )
        return false;

      size_t unreached_nodes = tree.get_number_of_nodes();
      std::vector<bool> marked( unreached_nodes, false );
      std::list< uint32_t > front_indices;
      front_indices.push_back( 0 );

      while(!front_indices.empty() )
        {
          uint32_t current_idx = front_indices.front();
          front_indices.pop_front();
          if( marked[ current_idx ] )
            return false;
          marked[ current_idx ] = true;
          --unreached_nodes;

          if( !tree.is_leaf( current_idx ) )
            {
              const auto& node = tree.get_node( current_idx );
              if( tree.get_node( node.right_index ).parent_index != current_idx
                || tree.get_node( node.left_index ).parent_index != current_idx )
                return false;
              front_indices.push_front( node.left_index );
              front_indices.push_front( node.right_index );
            }
        }

      return !unreached_nodes;
    }


    bool is_null_point( const vec3& a )
    {
      return std::abs( a.x ) < REAL_EPSILON
          && std::abs( a.y ) < REAL_EPSILON
          && std::abs( a.z ) < REAL_EPSILON;
    }

    bool is_same_point( const vec3& a, const vec3& b )
    {
      return is_null_point( b - a );
    }

    bool is_same_box( const aabox& a, const aabox& b )
    {
      return is_same_point( a.m_center, b.m_center ) && is_same_point( a.m_hsides, b.m_hsides );
    }


    //fixme: temp
    void display_node( auto& node )
    {
      std::cout
          << "box         : " << node.bounding.get_min() << " " << node.bounding.get_max() << "\n"
          << "parent      : " << node.parent_index << "\n"
          << "left/element: " << node.left_index << "\n"
          << "right       : " << node.right_index << "\n";
    }

    template< typename bvh1, typename bvh2 >
    bool are_equal( bvh1& a, bvh2& b )
    {
      if( a.get_number_of_nodes() == b.get_number_of_nodes() )
        {
          const size_t nodes = a.get_number_of_nodes();
          for( size_t i = 0; i < nodes; ++ i )
            {
              auto& anode = a.get_node( i );
              auto& bnode = b.get_node( i );

              if( !is_same_box( anode.bounding, bnode.bounding )
                || anode.parent_index != bnode.parent_index
                || anode.left_index != bnode.left_index
                || anode.right_index != bnode.right_index )
                {
                  std::cout << "nodes #" << i << " are different\n"
                      << "- first node\n";
                  display_node( anode );
                  std::cout << "- second node\n";
                  display_node( bnode );
                  std::cout << std::flush;
                return false;
                }
            }
          return true;
        }
      return false;
    }

    template< typename bvh_type >
    bool check_bounding_volumes( bvh_type& tree )
    {
      // leaves contains triangles

      // internal nodes contains child boxes


      return false;
    }
  }
}
# endif
