# ifndef BVH_CHECK_H_
# define BVH_CHECK_H_
# include <vector>
# include <list>
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

      std::vector<bool> marked( tree.get_number_of_nodes(), false );
      std::list< uint32_t > front_indices;
      front_indices.push_back( 0 );

      while(!front_indices.empty() )
        {
          uint32_t current_idx = front_indices.front();
          front_indices.pop_front();
          if( marked[ current_idx ] )
            return false;
          marked[ current_idx ] = true;

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
      return true;
    }
  }
}
# endif
