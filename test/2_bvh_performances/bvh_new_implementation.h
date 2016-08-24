# ifndef BVH_NEW_IMPLEMENTATION_H_
# define BVH_NEW_IMPLEMENTATION_H_
namespace graphics_origin {
namespace geometry {
// Since we work with templates, the implementation must reside inside a
// header. We use here an anonymous namespace to make the implementation local
// to this file and thus hide it from graphics_origin::geometry scope.
namespace {

  template< typename bounding_volume >
  struct bvh_building_variables {
    bvh_building_variables(
        typename bvh<bounding_volume>::node* nodes,
        typename bvh<bounding_volume>::node_index number_of_internal_nodes,
        typename bvh<bounding_volume>::node_index number_of_leaf_nodes ) :
      nodes{ nodes },
      number_of_internal_nodes{ number_of_internal_nodes },
      number_of_leaf_nodes{ number_of_leaf_nodes }
    {}

    typename bvh<bounding_volume>::node* nodes;
    const typename bvh<bounding_volume>::node_index number_of_internal_nodes;
    const typename bvh<bounding_volume>::node_index number_of_leaf_nodes;
  };

  template< typename bounding_volume >
  struct bvh_tree_structure_builder {
    typedef uint64_t morton_code;
    typedef typename bvh<bounding_volume>::node_index node_index;
    typedef typename bvh<bounding_volume>::node node;

    static constexpr uint8_t mcode_size = sizeof(morton_code) * 8;
    static constexpr uint8_t mcode_length = mcode_size / 3;
    static constexpr uint32_t mcode_offset = 1U << mcode_length;

# ifdef _WIN32
# define CLZ64( a ) __lzcnt64( a )
# define CLZ32( a ) __lzcnt  ( a )
# else
# define CLZ64( a ) __builtin_clzl( a )
# define CLZ32( a ) __builtin_clz ( a )
# endif

    template< typename bounded_element >
    bvh_tree_structure_builder(
        bvh_building_variables<bounding_volume>& input,
        const bounded_element* elements,
        morton_code* morton_codes ) :
      input{ input },
      morton_codes{ morton_codes }
    {
      compute_leaves_bounding_volumes( elements );
      compute_root_bounding_volume();
      compute_morton_codes_and_leaves();
      compute_node_hierarchy();
    }

    template< typename bounded_element >
    bvh_tree_structure_builder(
        bvh_building_variables<bounding_volume>& input,
        const bounded_element* elements,
        morton_code* morton_codes,
        bounding_volume& root_bounding_volume ) :
      input{ input },
      root_bounding_volume{ root_bounding_volume },
      morton_codes{ morton_codes }
    {
      compute_leaves_bounding_volumes( elements );
      compute_morton_codes_and_leaves();
      compute_node_hierarchy();
    }

    template< typename bounded_element >
    void compute_leaves_bounding_volumes(
        const bounded_element* elements )
    {
       # pragma omp parallel for
       for( node_index i = 0; i < input.number_of_leaf_nodes; ++ i )
         {
           bounding_volume_computer< bounding_volume, bounded_element >::compute(
               elements[ i ],
               input.nodes[ i + input.number_of_internal_nodes ].bounding );
         }
    }

    void compute_root_bounding_volume()
    {
      const node_index start = input.number_of_internal_nodes;
      const node_index stop  = input.number_of_internal_nodes + input.number_of_leaf_nodes;
      root_bounding_volume = input.nodes[ start ].bounding;  //had been computed previously;

      # pragma omp declare \
        reduction(bvmerge: bounding_volume: omp_out = bounding_volume_merger<bounding_volume>::merge( omp_in, omp_out )) \
        initializer(omp_priv = omp_orig )

      # pragma omp parallel for reduction(bvmerge: root_bounding_volume )
      for( node_index i = start; i < stop; ++ i )
        {
          root_bounding_volume = bounding_volume_merger<bounding_volume>::merge( root_bounding_volume, input.nodes[i].bounding );
        }
    }

    void compute_morton_codes_and_leaves()
    {
      const vec3 lower = bounding_volume_analyzer<bounding_volume>::compute_lower_corner( root_bounding_volume );
      const vec3 sides = bounding_volume_analyzer<bounding_volume>::compute_upper_corner( root_bounding_volume ) - lower;
      const vec3 inv_extents_times_mcode_offset = vec3 {
        real( mcode_offset ) / sides.x,
        real( mcode_offset ) / sides.y,
        real( mcode_offset ) / sides.z
      };

      # pragma omp parallel for schedule(static)
      for( node_index i = 0; i < input.number_of_leaf_nodes; ++ i )
        {
          node* leaf = input.nodes + i + input.number_of_internal_nodes;
          leaf->element = i;
          vec3 coordinates =
              (bounding_volume_analyzer<bounding_volume>::compute_center( leaf->bounding ) - lower)
              * inv_extents_times_mcode_offset;
          morton_code a = morton_code( coordinates.x );
          morton_code b = morton_code( coordinates.y );
          morton_code c = morton_code( coordinates.z );

          for( uint j = 0; j < mcode_length; ++ j )
            {
              morton_codes[i] |=
              ((((a >> (mcode_length - 1 - j)) & 1) << ((mcode_length - j) * 3 - 1)) |
               (((b >> (mcode_length - 1 - j)) & 1) << ((mcode_length - j) * 3 - 2)) |
               (((c >> (mcode_length - 1 - j)) & 1) << ((mcode_length - j) * 3 - 3)) );
            }
        }
      thrust::sort_by_key(
          thrust::omp::par, morton_codes, morton_codes + input.number_of_leaf_nodes,
          input.nodes + input.number_of_internal_nodes );
    }

    void node_hierarchy_kernel( node_index i, const uint32_vec2& range )
    {
      node& this_node = input.nodes[ i ];
      uint32_t split = find_split( range );
      uint32_t child_index = ( split == range.x ? split + input.number_of_internal_nodes : split );
      this_node.left_index = child_index;
      input.nodes[ child_index ].parent_index = i;

      ++split;
      child_index = ( split == range.y ? split + input.number_of_internal_nodes : split );
      this_node.right_index = child_index;
      input.nodes[ child_index ].parent_index = i;
    }

    void compute_node_hierarchy()
    {
      node_hierarchy_kernel( 0, uint32_vec2( 0, input.number_of_internal_nodes ) );
      # pragma omp parallel for
      for( node_index i = 1; i < input.number_of_internal_nodes; ++ i )
        {
          node_hierarchy_kernel( i, determine_range( i ) );
        }
    }

    int safe_lcp(
        const morton_code& left_code,
        node_index left,
        int right )
    {
      if( right >= int{0} && node_index(right) < input.number_of_leaf_nodes )
        return lcp( left_code, left, right );
      return -1;
    }

    int lcp(
        const morton_code& left_code,
        node_index left,
        node_index right )
    {
      const auto& right_code = morton_codes[ right ];
      if( right_code == left_code )
        return 64 + CLZ32( left ^ right );
      return CLZ64( left_code ^ right_code );
    }

    // note: this function is only called for i in [1, number_of_internal_nodes[
    // so calling lcp( ?, ?, i + 1 ) and lcp( ?, ?, i - 1 ) is not an issue.
    uint32_vec2 determine_range( node_index i )
    {
      const morton_code& mcodei = morton_codes[ i ];

      // determine the direction of the range (+1 or -1)
      int d = lcp( mcodei, i, i + 1 ) - lcp( mcodei, i, i - 1 ) > 0 ? 1 : -1;

      // compute upper bound for the length of the range
      int d_min = lcp( mcodei, i,  i - d );
      int lmax = 2;
      while( safe_lcp( mcodei, i,  i + lmax * d ) > d_min )
        lmax <<= 1;

      // find the other end using binary search
      uint32_t l = 0;
      for( uint32_t t = lmax >> 1; t >= 1; t >>= 1 )
        {
          if( safe_lcp( mcodei, i, i + ( l + t ) * d ) > d_min )
            l += t;
        }

      if( d > 0 )
        return uint32_vec2( i, i + l );
      return uint32_vec2( i - l, i );
    }

    node_index find_split( const uint32_vec2& range )
    {
      const uint64_t first_code = morton_codes[ range.x ];

      // Calculate the number of highest bits that are the same
      // for all objects, using the count-leading-zeros intrinsic.
      int common_prefix = lcp( first_code, range.x, range.y );

      // Use binary search to find where the next bit differs.
      // Specifically, we are looking for the highest object that
      // shares more than common_prefix bits with the first one.
      uint32_t split = range.x; // initial guess
      uint32_t step = range.y - range.x;
      do
        {
          step = (step + 1) >> 1; // exponential decrease
          uint32_t new_split = split + step; // proposed new position

          if( new_split < range.y )
            {
              if( lcp( first_code, range.x, new_split ) > common_prefix )
                split = new_split; // accept proposal
           }
        }
      while( step > 1 );
      return split;
    }

    bvh_building_variables<bounding_volume> input;
    bounding_volume root_bounding_volume;
    morton_code* morton_codes;
  };

  template< typename bounding_volume >
  struct bvh_bounding_volumes_builder {
    typedef uint64_t morton_code;
    typedef typename bvh<bounding_volume>::node_index node_index;
    typedef typename bvh<bounding_volume>::node node;
    typedef node_index thread_index;

    struct thread_variables {
      node_index index;
      uint8_t number_of_threads_having_reached_this_node;
      bool is_active;
    };

    bvh_bounding_volumes_builder(
        bvh_building_variables<bounding_volume>& input,
        uint8_t* counters,
        thread_variables* variables ) :
      input{ input },
      variables{ variables },
      counters{ counters }
    {
      # pragma omp parallel for schedule(static)
      for( node_index i = 0; i < input.number_of_internal_nodes; ++ i )
        {
          counters[ i ] = 0;
        }

      # pragma omp parallel for schedule(static)
      for( node_index i = 0; i < input.number_of_leaf_nodes; ++ i )
        {
          thread_variables& vars = variables[ i ];
          vars.is_active = true;
          vars.index = input.nodes[ i + input.number_of_internal_nodes ].parent_index;

          # pragma omp atomic capture
          {
            vars.number_of_threads_having_reached_this_node = counters[ vars.index ];
            ++counters[ vars.index ];
          }
        }

      # pragma omp declare \
        reduction(bvbactivity_reduction: bool: omp_out |= omp_in) \
        initializer(omp_priv = false )
      bool iteration_required = true;
      while (iteration_required)
        {
          iteration_required = false;
          # pragma omp parallel for reduction(bvbactivity_reduction: iteration_required)
          for( thread_index tid = 0; tid < input.number_of_internal_nodes; ++ tid )
            {
              kernel( tid, iteration_required );
            }
        }
    }


    void kernel( thread_index tid, bool& next_iteration_required )
    {
      thread_variables& vars = variables[ tid ];
      if( vars.is_active )
        {
          // The first thread to reach this node will stop to work.
          if( !vars.number_of_threads_having_reached_this_node )
            {
              vars.is_active = false;
            }
          // The second thread to reach this node will process it.
          else
            {
              // The bounding volume of this node is the union of the bounding
              // volumes of its two children.
              node& n = input.nodes[ vars.index ];
              n.bounding = bounding_volume_merger<bounding_volume>(
                  input.nodes[ n.left_index ].bounding,
                  input.nodes[ n.right_index ].bounding );

              // Go up until the root is reached.
              if( vars.index )
                {
                  vars.index = n.parent_index;
                  # pragma omp atomic capture
                  {
                    vars.number_of_threads_having_reached_this_node = counters[ vars.index ];
                    ++counters[ vars.index ];
                  }
                  next_iteration_required = true;
                }
              else
                {
                  vars.is_active = false;
                }
            }
        }
    }
    bvh_building_variables<bounding_volume> input;
    thread_variables* variables;
    uint8_t* counters;
  };
} // end of anonymous name space

  /**
   * The construction algorithm can be divided into two parts:
   * - computing the tree structure of the BVH
   * - computing bounding volumes of the internal nodes.
   *
   * For the first part, we need first to compute bounding volumes of
   * leaf nodes, that is the bounding volumes of bounded elements. If
   * necessary, we will use those bounding volumes to compute the root
   * bounding volume. Then, we can compute Morton codes of the leaves,
   * and order the leaves in increasing Morton codes. Those Morton codes
   * are then used to determine the node hierarchy, i.e. set the left and
   * right childs of each internal node. After this step, we have an almost
   * complete BVH: all we need to compute are the bounding volumes of
   * internal nodes. The Morton codes are no longer necessary.
   *
   * The second part iterates on nodes, from leaves to the root, one level
   * at a time. The code is adapted from a previous implementation I made
   * with CUDA: we have as much as threads as there are leaves. One of those
   * threads works on a particular node index. When its work is over, it will
   * set the node index to the parent of the current one. At the next iteration,
   * two threads will have the same node index. We use then a counter for each
   * node to determine which thread arrives first on a node. The first thread to
   * arrive to a node will stop, only the second one will continue to work. This
   * way we are sure that:
   * - each node is processed once
   * - each node is processed after both of its children.
   */
  template<
     typename bounding_volume >
  struct bvh_builder {
    typedef uint64_t morton_code;
    typedef typename bvh<bounding_volume>::node_index node_index;
    typedef typename bvh<bounding_volume>::node node;

    template< typename bounded_element >
    bvh_builder(
        bvh<bounding_volume>& target,
        const bounded_element* elements )
    {
      const size_t size_of_thread_variables =
          sizeof( typename bvh_bounding_volumes_builder<bounding_volume>::thread_variables)
          * (target.get_number_of_leaf_nodes() );
      const size_t size_of_counters = sizeof(uint8_t) * target.get_number_of_internal_nodes();
      const size_t size = std::max(
          target.get_number_of_leaf_nodes() * sizeof(uint64_t),
          size_of_counters + size_of_thread_variables
      );

      char* raw_pointer = (char*)malloc( size );
      std::memset( raw_pointer, 0, size );
      bvh_building_variables<bounding_volume> input( target.m_nodes.data(), target.number_of_internal_nodes, target.get_number_of_leaf_nodes() );
      bvh_tree_structure_builder<bounding_volume>( input, elements, reinterpret_cast<morton_code*>(raw_pointer) );
      bvh_bounding_volumes_builder<bounding_volume>(
          input,
          reinterpret_cast<uint8_t*>(raw_pointer),
          reinterpret_cast<typename bvh_bounding_volumes_builder<bounding_volume>::thread_variables*>(raw_pointer + size_of_counters )
      );
      free( raw_pointer );
    }

    template< typename bounded_element >
    bvh_builder(
        bvh<bounding_volume>& target,
        const bounded_element* elements,
        bounding_volume& root_bounding_volume )
    {
      const size_t size_of_thread_variables =
          sizeof( typename bvh_bounding_volumes_builder<bounding_volume>::thread_variables)
          * (target.get_number_of_leaf_nodes() );
      const size_t size_of_counters = sizeof(uint8_t) * target.get_number_of_internal_nodes();
      const size_t size = std::max(
          target.get_number_of_leaf_nodes() * sizeof(uint64_t),
          size_of_counters + size_of_thread_variables
      );

      char* raw_pointer = (char*)malloc( size );
      std::memset( raw_pointer, 0, size );
      bvh_building_variables<bounding_volume> input( target.m_nodes.data(), target.number_of_internal_nodes, target.get_number_of_leaf_nodes() );
      bvh_tree_structure_builder<bounding_volume>(
          input,
          elements,
          reinterpret_cast<morton_code*>(raw_pointer),
          root_bounding_volume );
      bvh_bounding_volumes_builder<bounding_volume>(
          input,
          reinterpret_cast<uint8_t*>(raw_pointer),
          reinterpret_cast<typename bvh_bounding_volumes_builder<bounding_volume>::thread_variables*>(raw_pointer + size_of_counters )
      );
      free( raw_pointer );
    }
   };

  template< typename bounding_volume >
  template< typename bounded_element >
  bvh<bounding_volume>::bvh( const bounded_element* elements, size_t number_of_elements ) :
    number_of_internal_nodes{ number_of_elements ? number_of_elements - 1 : 0 }
  {
    if( number_of_elements > max_number_of_elements )
      {
        LOG( fatal, "internal structures cannot handle the requested number of elements." );
        return;
      }

    if( number_of_elements < 2 )
      {
        LOG( fatal, "not enough elements to create a bounding volume hierarchy." );
        return;
      }

    m_nodes.resize( ( number_of_internal_nodes << 1 ) + 1 );

    bvh_builder<bounding_volume>( *this, elements );
  }

  template< typename bounding_volume >
  template< typename bounded_element >
  bvh<bounding_volume>::bvh(
      const bounded_element* elements,
      size_t number_of_elements,
      bounding_volume& root_bounding_volume ) :
    number_of_internal_nodes{ number_of_elements ? number_of_elements - 1 : 0 }
  {
    if( number_of_elements > max_number_of_elements )
      {
        LOG( fatal, "internal structures cannot handle the requested number of elements." );
        return;
      }

    if( number_of_elements < 2 )
      {
        LOG( fatal, "not enough elements to create a bounding volume hierarchy." );
        return;
      }

    m_nodes.resize( ( number_of_internal_nodes << 1 ) + 1 );

    bvh_builder<bounding_volume>( *this, elements, root_bounding_volume );
  }
} // end of geometry name space
} // end of graphics_origin name space
# endif
