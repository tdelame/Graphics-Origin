# ifndef BVH_PRINTER_H_
# define BVH_PRINTER_H_

namespace graphics_origin {
namespace geometry {

//  struct node_printer {
//
//    void print_leaf( node_index i, const node* leaf );
//
//    void print_internal( node_index i, const node* internal );
//
//    void begin_printing();
//    void end_printing();
//
//    void begin_tree();
//    void end_tree();
//
//    void begin_depth();
//    void end_depth();
//  };


  template<
    typename bounding_volume,
    typename node_printer >
  class bvh_printer {
  public:
    typedef typename bvh_new<bounding_volume>::node node;
    typedef typename bvh_new<bounding_volume>::node_index node_index;

    bvh_printer( bvh_new<bounding_volume>& bvh, node_printer& printer ) :
      number_of_internals{ bvh.get_number_of_internal_nodes() },
      number_of_leaves{ bvh.get_number_of_leaf_nodes() },
      number_of_nodes{ number_of_internals + number_of_leaves },
      nodes{ &bvh.get_node( 0 ) },
      reached{ new bool[number_of_nodes] },
      printer{ printer }
    {
      for( node_index i = 0; i < number_of_nodes; ++ i )
        reached[i] = false;
      process_trees();
    }

    ~bvh_printer()
    {
      delete[] reached;
    }

  private:
    void process_trees()
    {
      printer.begin_printing();
      node_index next_root_index;
      while( find_next_root( next_root_index ) )
        {
          printer.begin_tree();
          process_next_tree( next_root_index );
          printer.end_tree();
        }
      printer.end_printing();
    }

    void process_next_tree( node_index root_index )
    {
      std::list< node_index > depths[ 2 ] = {
          std::list<node_index>{},
          std::list<node_index>{}
      };

      unsigned int current_depth = 0;
      unsigned int next_depth = 1;
      depths[current_depth].push_back( root_index );
      reached[ root_index ] = true;

      while( !depths[current_depth].empty() )
        {
          printer.begin_depth();
          while(!depths[current_depth].empty() )
            {
              node_index i = depths[current_depth].front();
              depths[current_depth].pop_front();
              const node* current_node = nodes + i;
              if( i < number_of_internals )
                {
                  printer.print_internal( i, current_node );
                  depths[next_depth].push_back( current_node->left_index );
                  depths[next_depth].push_back( current_node->right_index );
                  reached[ current_node->left_index ] = true;
                  reached[ current_node->right_index ] = true;
                }
              else
                printer.print_leaf( i, current_node );
            }
          printer.end_depth();
          std::swap( current_depth, next_depth );
        }
    }

    bool find_next_root( node_index& next_root_index )
    {
      auto search = std::find( reached, reached + number_of_nodes, false );
      if( search < reached + number_of_nodes )
        {
          next_root_index = std::distance( reached, search );
          return true;
        }
      return false;
    }

    const node_index number_of_internals;
    const node_index number_of_leaves;
    const node_index number_of_nodes;
    const node* nodes;
    bool* reached;
    node_printer& printer;
  };


} // graphics_origin
} // geometry



# endif /* BVH_PRINTER_H_ */
