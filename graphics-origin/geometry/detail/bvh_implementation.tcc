/* Created on: Mar 15, 2016
 *     Author: T.Delame (tdelame@gmail.com)
 */
# ifndef GRAPHICS_ORIGIN_BVH_IMPLEMENTATION_TCC_
# define GRAPHICS_ORIGIN_BVH_IMPLEMENTATION_TCC_

/*  Created on: Jan 24, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 *
 * The implementation is based on a CUDA version I made in an another project.
 * I found it easier to have sensibly the same code for both the GPU and the CPU
 * versions. It might be a little sub-efficient for the CPU, but it does the job.
 * This CUDA version was itself based on:
 *
 *  - the paper "Maximizing Parallelism in the Construction of BVHs, Octrees, and k-d Trees"
 *  Tero Karras, High Performance Graphics (2012)
 *  (a copy is available here:
 *  http://devblogs.nvidia.com/parallelforall/wp-content/uploads/sites/3/2012/11/karras2012hpg_paper.pdf)
 *
 *  - the related blog articles
 *  http://devblogs.nvidia.com/parallelforall/thinking-parallel-part-i-collision-detection-gpu/
 *  http://devblogs.nvidia.com/parallelforall/thinking-parallel-part-ii-tree-traversal-gpu/
 *  http://devblogs.nvidia.com/parallelforall/thinking-parallel-part-iii-tree-construction-gpu/
 *
 *  - the following github repository https://github.com/andrewwuan/smallpt-parallel-bvh-gpu
 *  with the accompanying license text:
LICENSE

Copyright (c) 2006-2008 Kevin Beason (kevin.beason@gmail.com)

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**@file
 * @todo Integrate the CUDA version with transfer functions GPU <--> CPU
 */


# include "../box.h"
# include "../ball.h"

# include <thrust/sort.h>

BEGIN_GO_NAMESPACE namespace geometry {

  static constexpr size_t bvh_max_number_of_elements = (1U << uint8_t(31)) - 1;

  static constexpr uint8_t  mcode_size = 64;
  static constexpr uint8_t  mcode_length = 21;
  static constexpr uint32_t mcode_offset = 1U << mcode_length;

# ifdef _WIN32
#  define CLZ( a ) __lzcnt64(a)
# else
#  define CLZ( a ) __builtin_clzl( a )
# endif

  /**@brief Order BVH leaf nodes by increasing Morton code.
   *
   * This structure orders BVH leaf nodes by increasing Morton code. It is
   * used by set_leaf_nodes. */
  template< typename bounding_object >
  struct morton_code_order {
    bool
    operator()( const thrust::tuple< const typename bvh<bounding_object>::node&, uint64_t>& a,
                const thrust::tuple< const typename bvh<bounding_object>::node&, uint64_t>& b ) const
    {
      return a.tail < b.tail;
    }
  };


  template< typename bounding_object >
  bvh<bounding_object>::node::node()
    : parent_index{0}, left_index{0}, right_index{0}
  {}

  /**Specialization of set_leaf_nodes structure
   */
  template< typename bounded_element >
  struct set_leaf_nodes< aabox, bounded_element> {
    static_assert(
        geometric_traits<bounded_element>::is_bounding_box_computer,
        "The elements bounded by bounding boxes in the BVH must be able to compute a bounding box");

    set_leaf_nodes(
        const bounded_element* elements,
        aabox& root_bounding_object,
        std::vector<typename bvh<aabox>::node>& nodes,
        size_t number_of_internals,
        std::vector<uint64_t>& morton_codes )
    {
      auto lower = root_bounding_object.get_min();
      auto inv_extents_times_mcode_offset = vec3{
        real(0.5 * mcode_offset) / root_bounding_object.m_hsides.x,
        real(0.5 * mcode_offset) / root_bounding_object.m_hsides.y,
        real(0.5 * mcode_offset) / root_bounding_object.m_hsides.z
      };

      /* There are n elements, thus n leaves and n - 1 internal nodes.
       * The node buffer is first filled with internal nodes, thus from index 0 to index n - 2.
       * Leaves are then between indices n - 1 and n - 1 + n - 1
       *
       */
	  # ifdef _WIN32
	  #   pragma message("MSVC does not allow unsigned index variable in OpenMP for statement")
	  #   pragma omp parallel for schedule(static)
	  for( _int32 i = 0; i <= number_of_internals; ++ i )
	  # else
      #   pragma omp parallel for schedule(static)
      for( uint32_t i = 0; i <= number_of_internals; ++ i )
	  # endif
        {
          auto& leaf = nodes[ i + number_of_internals ];
          leaf.element_index = i;
          elements[i].compute_bounding_box( leaf.bounding );
          const auto& p = leaf.bounding.m_center;

          uint64_t a = (uint64_t)( (p.x - lower.x) * inv_extents_times_mcode_offset.x );
          uint64_t b = (uint64_t)( (p.y - lower.y) * inv_extents_times_mcode_offset.y );
          uint64_t c = (uint64_t)( (p.z - lower.z) * inv_extents_times_mcode_offset.z );

          for( uint j = 0; j < mcode_length; ++ j )
            {
              morton_codes[i] |=
              ((((a >> (mcode_length - 1 - j)) & 1) << ((mcode_length - j) * 3 - 1)) |
               (((b >> (mcode_length - 1 - j)) & 1) << ((mcode_length - j) * 3 - 2)) |
               (((c >> (mcode_length - 1 - j)) & 1) << ((mcode_length - j) * 3 - 3)) );
            }
        }

   # ifdef GO_USE_CUDA_THRUST
      thrust::sort(
          thrust::make_zip_iterator(
              thrust::make_tuple(
                  nodes.data() + number_of_internals,
                  morton_codes.data() ) ),
          thrust::make_zip_iterator(
              thrust::make_tuple(
                  nodes.data() + nodes.size(),
                  morton_codes.data() + morton_codes.size() )),
          morton_code_order<aabox>{});
  # else
      static_assert( false,
                     "the implementation without Cuda/Thrust of this part is not available yet");
  # endif
    }

    set_leaf_nodes(
        const bounded_element* elements,
        std::vector<typename bvh<aabox>::node>& nodes,
        size_t number_of_internals,
        std::vector<uint64_t>& morton_codes )
    {
      aabox bounding;
      elements[0].compute_bounding_box( bounding );

      # pragma omp parallel
      {
        aabox thread_bounding = bounding;
	  # ifdef _WIN32
	  #   pragma message("MSVC does not allow unsigned index variable in OpenMP for statement")
	  #   pragma omp parallel for schedule(static)
	  for( _int32 i = 0; i <= number_of_internals; ++ i )
	  # else
      #   pragma omp parallel for schedule(static)
      for( uint32_t i = 0; i <= number_of_internals; ++ i )
	  # endif
         {
            auto& leaf = nodes[ i + number_of_internals ];
            leaf.element_index = i;
            elements[i].compute_bounding_box( leaf.bounding );
            thread_bounding.merge( leaf.bounding );
          }
        # pragma omp critical
        bounding.merge( thread_bounding );
      }

      // now we have the bounding box, we can compute morton codes
      auto lower = bounding.get_min();
      auto inv_extents_times_mcode_offset = vec3{
        real(0.5 * mcode_offset) / bounding.m_hsides.x,
        real(0.5 * mcode_offset) / bounding.m_hsides.y,
        real(0.5 * mcode_offset) / bounding.m_hsides.z
      };

	  # ifdef _WIN32
	  #   pragma message("MSVC does not allow unsigned index variable in OpenMP for statement")
	  #   pragma omp parallel for schedule(static)
	  for( _int32 i = 0; i <= number_of_internals; ++ i )
	  # else
      #   pragma omp parallel for schedule(static)
      for( uint32_t i = 0; i <= number_of_internals; ++ i )
	  # endif
        {
          auto& leaf = nodes[ i + number_of_internals ];
          const auto& p = leaf.bounding.m_center;

          uint64_t a = (uint64_t)( (p.x - lower.x) * inv_extents_times_mcode_offset.x );
          uint64_t b = (uint64_t)( (p.y - lower.y) * inv_extents_times_mcode_offset.y );
          uint64_t c = (uint64_t)( (p.z - lower.z) * inv_extents_times_mcode_offset.z );

          for( unsigned int j = 0; j < mcode_length; ++ j )
            {
              morton_codes[i] |=
              ((((a >> (mcode_length - 1 - j)) & 1) << ((mcode_length - j) * 3 - 1)) |
               (((b >> (mcode_length - 1 - j)) & 1) << ((mcode_length - j) * 3 - 2)) |
               (((c >> (mcode_length - 1 - j)) & 1) << ((mcode_length - j) * 3 - 3)) );
            }
        }
    # ifdef GO_USE_CUDA_THRUST
      thrust::sort(
          thrust::make_zip_iterator(
              thrust::make_tuple(
                  nodes.data() + number_of_internals,
                  morton_codes.data() ) ),
          thrust::make_zip_iterator(
              thrust::make_tuple(
                  nodes.data() + nodes.size(),
                  morton_codes.data() + morton_codes.size() )),
          morton_code_order<aabox>{});
    # else
       static_assert( false,
                      "the implementation without Cuda/Thrust of this part is not available yet");
    # endif
    }
  };

  template< typename bounded_element >
  struct set_leaf_nodes< ball, bounded_element> {

    set_leaf_nodes(
        const bounded_element* elements,
        ball& root_bounding_object,
        std::vector<typename bvh<ball>::node>& nodes,
        size_t number_of_internals,
        std::vector<uint64_t>& morton_codes )
    {
      static_assert(
        geometric_traits<bounded_element>::is_bounding_ball_computer,
        "The elements bounded by bounding balls in the BVH must be able to compute a bounding ball");

      auto lower = vec3{root_bounding_object} - vec3{root_bounding_object.w, root_bounding_object.w, root_bounding_object.w};
      auto inv_extents_times_mcode_offset = vec3{
        real(0.5 * mcode_offset) / root_bounding_object.w,
        real(0.5 * mcode_offset) / root_bounding_object.w,
        real(0.5 * mcode_offset) / root_bounding_object.w
      };
	  # ifdef _WIN32
	  #   pragma message("MSVC does not allow unsigned index variable in OpenMP for statement")
	  #   pragma omp parallel for schedule(static)
	  for( _int32 i = 0; i <= number_of_internals; ++ i )
	  # else
      #   pragma omp parallel for schedule(static)
      for( uint32_t i = 0; i <= number_of_internals; ++ i )
	  # endif
        {
          auto& leaf = nodes[ i + number_of_internals ];
          leaf.element_index = i;
          elements[i].compute_bounding_ball( leaf.bounding );
          const auto& p = leaf.bounding;

          uint64_t a = (uint64_t)( (p.x - lower.x) * inv_extents_times_mcode_offset.x );
          uint64_t b = (uint64_t)( (p.y - lower.y) * inv_extents_times_mcode_offset.y );
          uint64_t c = (uint64_t)( (p.z - lower.z) * inv_extents_times_mcode_offset.z );

          for( uint j = 0; j < mcode_length; ++ j )
            {
              morton_codes[ i ] |=
              ((((a >> (mcode_length - 1 - j)) & 1) << ((mcode_length - j) * 3 - 1)) |
               (((b >> (mcode_length - 1 - j)) & 1) << ((mcode_length - j) * 3 - 2)) |
               (((c >> (mcode_length - 1 - j)) & 1) << ((mcode_length - j) * 3 - 3)) );
            }
        }
# ifdef GO_USE_CUDA_THRUST
      thrust::sort(
          thrust::make_zip_iterator(
              thrust::make_tuple(
                  nodes.data() + number_of_internals,
                  morton_codes.data() ) ),
          thrust::make_zip_iterator(
              thrust::make_tuple(
                  nodes.data() + nodes.size(),
                  morton_codes.data() + morton_codes.size() )),
          morton_code_order<ball>{});
# else
       static_assert( false,
                  "the implementation without Cuda/Thrust of this part is not available yet");
# endif
    }

    set_leaf_nodes(
        const bounded_element* elements,
        std::vector<typename bvh<ball>::node>& nodes,
        size_t number_of_internals,
        std::vector<uint64_t>& morton_codes  )
    {
      static_assert(
        geometric_traits<bounded_element>::is_bounding_ball_computer,
        "The elements bounded by bounding balls in the BVH must be able to compute a bounding ball");

      ball bounding;
      elements[0].compute_bounding_ball( bounding );

      # pragma omp parallel
      {
        ball thread_bounding = bounding;
	  # ifdef _WIN32
	  #   pragma message("MSVC does not allow unsigned index variable in OpenMP for statement")
	  #   pragma omp for
	  for( _int32 i = 0; i <= number_of_internals; ++ i )
	  # else
      #   pragma omp for
      for( uint32_t i = 0; i <= number_of_internals; ++ i )
	  # endif
          {
            auto& leaf = nodes[ i + number_of_internals ];
            leaf.element_index = i;
            elements[i].compute_bounding_ball( leaf.bounding );
            thread_bounding.merge( leaf.bounding );
          }
        # pragma omp critical
        bounding.merge( thread_bounding );
      }

      // now we have the bounding ball, we can compute morton codes
      auto lower = vec3{bounding} - vec3{bounding.w, bounding.w, bounding.w};
      auto inv_extents_times_mcode_offset = vec3{
        real(0.5 * mcode_offset) / bounding.w,
        real(0.5 * mcode_offset) / bounding.w,
        real(0.5 * mcode_offset) / bounding.w
      };

 
	  # ifdef _WIN32
	  #   pragma message("MSVC does not allow unsigned index variable in OpenMP for statement")
	  #   pragma omp parallel for schedule(static)
	  for( _int32 i = 0; i <= number_of_internals; ++ i )
	  # else
      #   pragma omp parallel for schedule(static)
      for( uint32_t i = 0; i <= number_of_internals; ++ i )
	  # endif
        {
          auto& leaf = nodes[ i + number_of_internals ];
          const auto& p = leaf.bounding;

          uint64_t a = (uint64_t)( (p.x - lower.x) * inv_extents_times_mcode_offset.x );
          uint64_t b = (uint64_t)( (p.y - lower.y) * inv_extents_times_mcode_offset.y );
          uint64_t c = (uint64_t)( (p.z - lower.z) * inv_extents_times_mcode_offset.z );

          for( uint j = 0; j < mcode_length; ++ j )
            {
              morton_codes[ i ] |=
              ((((a >> (mcode_length - 1 - j)) & 1) << ((mcode_length - j) * 3 - 1)) |
               (((b >> (mcode_length - 1 - j)) & 1) << ((mcode_length - j) * 3 - 2)) |
               (((c >> (mcode_length - 1 - j)) & 1) << ((mcode_length - j) * 3 - 3)) );
            }
        }
      # ifdef GO_USE_CUDA_THRUST
      thrust::sort(
          thrust::make_zip_iterator(
              thrust::make_tuple(
                  nodes.data() + number_of_internals,
                  morton_codes.data() ) ),
          thrust::make_zip_iterator(
              thrust::make_tuple(
                  nodes.data() + nodes.size(),
                  morton_codes.data() + morton_codes.size() )),
          morton_code_order<ball>{});
      # else
       static_assert( false,
                  "the implementation without Cuda/Thrust of this part is not available yet");
      # endif
    }
  };

  template< typename bounding_object >
  struct determine_nodes_hierarchy {

    int safe_longest_common_prefix( const uint64_t& c, uint32_t i, uint32_t j )
    {
      if( j >= uint32_t(0) && j < m_number_of_leaves )
        return lgp( c, i, j );
      return -1;
    }

    int lgp( const uint64_t& c, uint32_t i, uint32_t j )
    {
      auto& code = m_morton_codes[j];
      if( code == c )
	  # ifdef _WIN32
		return 64 + __lzcnt( i ^ j );
	  # else
        return 64 + __builtin_clz( i ^ j );
	  # endif
      return CLZ( code ^ c );
    }


    /**
     *
     * i is in [0, number_of_internals - 1 ]
     * thus m_morton_codes[i+1] is always valid
     *
     * for i = 0, the expected result is [0, number_of_internals ]
     * otherwise, m_morton_codes[i-1] is valid
     *
     * Thus, this function can be called only for i in [1, number_of_internals - 1]
     */
    uivec2 determine_range( uint32_t i )
    {
//      const auto& mcodei_minus = m_morton_codes[i-1];
      const auto& mcodei       = m_morton_codes[i  ];
//      const auto& mcodei_plus  = m_morton_codes[i+1];
//
//      // this case happen when there are duplicate morton codes
//      if( mcodei_minus == mcodei && mcodei == mcodei_plus )
//        {
//          //look for the last equal morton code
//          uint32_t end = i;
//          do
//            {
//              ++end;
//            }
//          while( (end + 1 < m_number_of_leaves) && m_morton_codes[end + 1] == mcodei );
//          return uivec2( i, end );
//        }

      // determine the direction of the range (+1 or -1)
      int d = lgp( mcodei, i, i + 1 ) - lgp( mcodei, i, i - 1 ) > 0 ? 1 : -1;

      // compute upper bound for the length of the range
      int d_min = lgp( mcodei, i,  i - d );
      int lmax = 2;
      while( safe_longest_common_prefix( mcodei, i,  i + lmax * d ) > d_min )
        lmax <<= 1;

      // find the other end using binary search
      uint32_t l = 0;
      for( uint32_t t = lmax >> 1; t >= 1; t >>= 1 )
        {
          if( safe_longest_common_prefix( mcodei, i, i + ( l + t ) * d ) > d_min )
            l += t;
        }

      if( d > 0 )
        return uivec2( i, i + l );
      return uivec2( i - l, i );
    }

    uint32_t find_split( const uivec2& range )
    {
      const uint64_t first_code = m_morton_codes[ range.x ];
      const uint64_t  last_code = m_morton_codes[ range.y ];

      // Identical Morton codes => split the range in the middle.
//      if( first_code == last_code )
////        return range.x;
//        return (range.x + range.y) >> 1;

      // Calculate the number of highest bits that are the same
      // for all objects, using the count-leading-zeros intrinsic.
      int common_prefix = lgp( first_code, range.x, range.y );

      // Use binary search to find where the next bit differs.
      // Specifically, we are looking for the highest object that
      // shares more than commonPrefix bits with the first one.
      uint32_t split = range.x; // initial guess
      uint32_t step = range.y - range.x;
      do
        {
          step = (step + 1) >> 1; // exponential decrease
          uint32_t new_split = split + step; // proposed new position

          if( new_split < range.y )
            {
              if( lgp( first_code, range.x, new_split ) > common_prefix )
                split = new_split; // accept proposal
           }
        }
      while( step > 1 );
      return split;
    }

    determine_nodes_hierarchy(
        typename bvh<bounding_object>::node* nodes,
        uint64_t* morton_codes,
        size_t number_of_internals )
      : m_nodes{ nodes }, m_morton_codes{ morton_codes },
        m_number_of_leaves{ number_of_internals + 1 }
    {
      //process the root independently
      {
        uivec2 range( 0, number_of_internals );
        uint32_t split = find_split( range );
        uint32_t child_index = ( split == range.x ? split + number_of_internals : split );
        m_nodes[0].left_index = child_index;

        ++split;
        child_index = ( split == range.y ? split + number_of_internals : split );
        m_nodes[0].right_index = child_index;
      }
	  # ifdef _WIN32
      #   pragma message("MSVC does not allow unsigned index variable in OpenMP for statement")
      #   pragma omp parallel for schedule(dynamic)
	  for (_int32 i = 1; i < number_of_internals; ++i )
	  # else
	  #   pragma omp parallel for schedule(dynamic)
	  for (uint32_t i = 1; i < number_of_internals; ++ i )
	  # endif
        {
          auto& intern = m_nodes[ i ];
          uivec2 range = determine_range( i );
          uint32_t split = find_split( range );
          uint32_t child_index = ( split == range.x ? split + number_of_internals : split );
          m_nodes[ child_index ].parent_index = i;
          intern.left_index = child_index;

          ++split;
          child_index = ( split == range.y ? split + number_of_internals : split );
          m_nodes[ child_index ].parent_index = i;
          intern.right_index = child_index;
        }
    }

    uint64_t* m_morton_codes;
    typename bvh<bounding_object>::node* m_nodes;
    const size_t m_number_of_leaves;
  };

  template<
    typename bounding_object >
  struct build_bvh_tree_kernel_emulation {

    struct thread_variables {
        bool active;
        uint32_t index;
        unsigned int res;
      };

    build_bvh_tree_kernel_emulation(
      typename bvh<bounding_object>::node* nodes,
      uint32_t number_of_internals ) :
        m_nodes{nodes}, m_number_of_internals{ number_of_internals },
        m_counters( number_of_internals ), variables( number_of_internals +1 )
    {
      init_counters();
      init_threads();

      bool activity = true;
      while( activity )
        {
          activity = false;
      # ifdef _WIN32
      #   pragma message("MSVC does not allow unsigned index variable in OpenMP for statement")
      #   pragma omp parallel for schedule(static)
	  for (_int32 i = 0; i < m_number_of_internals; ++i)
	  # else
	  #   pragma omp parallel for schedule(static)
	  for (uint32_t i = 0; i < m_number_of_internals; ++ i )
	  # endif
            {
              if( emulate_one_thread_loop( i ) )
                activity = true;
            }
        }
    }

    void init_counters()
    {
	  # ifdef _WIN32
	  #   pragma message("MSVC does not allow unsigned index variable in OpenMP for statement")
	  #   pragma omp parallel for schedule(static)
	  for( _int32 i = 0; i < m_number_of_internals; ++ i )
	  # else
      #   pragma omp parallel for schedule(static)
      for( uint32_t i = 0; i < m_number_of_internals; ++ i )
	  # endif
        {
          m_counters[ i ] = 0;
        }
    }

    void init_threads()
    {
	  # ifdef _WIN32
	  #   pragma message("MSVC does not allow unsigned index variable in OpenMP for statement")
	  #   pragma omp parallel for schedule(static)
	  for( _int32 i = 0; i <= m_number_of_internals; ++ i )
	  # else
      #   pragma omp parallel for schedule(static)
      for( uint32_t i = 0; i <= m_number_of_internals; ++ i )
	  # endif
        {
          auto& var = variables[ i ];
          var.active = true;
          var.index = m_nodes[ i + m_number_of_internals ].parent_index;
		  # ifdef _WIN32
	      #  pragma message ("MSVC does not support capture directive for an openMP atomic construct")
		  # pragma omp critical
		  {
            var.res = m_counters[ var.index ];
            ++m_counters[ var.index ];
          }
		  # else
          # pragma omp atomic capture
		  {
            var.res = m_counters[ var.index ];
            ++m_counters[ var.index ];
          }
		  # endif

        }
    }

    bool emulate_one_thread_loop( uint32_t tid )
    {
      auto& var = variables[ tid ];
      if( !var.active )
        {
//          LOG( debug, "node #" << var.index << " was rejected because tid " << tid << " is inactive");
          return false;
        }
      if( !var.res )
        {
//          LOG( debug, "node #" << var.index << " was rejected because tid " << tid << " is the first to arrive here");
          var.active = false;
          return false;
        }

      auto& node = m_nodes[ var.index ];
      node.bounding = m_nodes[ node.left_index ].bounding;
      node.bounding.merge( m_nodes[ node.right_index ].bounding );

//      LOG( debug, "node #" << var.index << " just got initialized to {" << node.bounding.get_min() << " , " << node.bounding.get_max() << "} by tid " << tid );

      if( var.index == 0 )
        {
          var.active = false;
          return false;
        }

      var.index = node.parent_index;
	  # ifdef _WIN32
	  #  pragma message ("MSVC does not support capture directive for an openMP atomic construct")
	  #  pragma omp critical
	  {
        var.res = m_counters[ var.index ];
        ++m_counters[ var.index ];
      }
	  # else
      #  pragma omp atomic capture
	        {
        var.res = m_counters[ var.index ];
        ++m_counters[ var.index ];
      }
	  # endif

      return true;
    }

    typename bvh<bounding_object>::node* m_nodes;
    const uint32_t m_number_of_internals;

    std::vector< int > m_counters;
    std::vector< thread_variables > variables;
  };

  template< typename bounding_object >
   template< typename bounded_element >
   bvh<bounding_object>::bvh( const bounded_element* elements, bounding_object& root_bounding_object, size_t number_of_elements )
     : m_number_of_internal_nodes{ number_of_elements ? number_of_elements - 1 : 0 }
   {
     static_assert(
         std::is_default_constructible<bounding_object>::value,
         "The bounding objects must be default constructible");
     if( number_of_elements > bvh_max_number_of_elements )
       {
         LOG( fatal, "internal structures cannot handle the requested number of elements");
         return;
       }
     else if( number_of_elements < 2 )
       {
         LOG( fatal, "not enough elements to create a bounding volume hierarchy");
         return;
       }
     m_nodes.resize( (m_number_of_internal_nodes << 1) + 1);
     std::vector<uint64_t> morton_codes( m_number_of_internal_nodes + 1, 0 );
     set_leaf_nodes<bounding_object,bounded_element>( elements, root_bounding_object, m_nodes, m_number_of_internal_nodes, morton_codes );
     determine_nodes_hierarchy<bounding_object>( m_nodes.data(), morton_codes.data(), m_number_of_internal_nodes );
     build_bvh_tree_kernel_emulation<bounding_object>( m_nodes.data(), m_number_of_internal_nodes );
   }


  template< typename bounding_object >
   template< typename bounded_element >
   bvh<bounding_object>::bvh(
       const bounded_element* elements,
       size_t number_of_elements )
     : m_number_of_internal_nodes{ number_of_elements ? number_of_elements - 1 : 0 }
   {
     static_assert(
         std::is_default_constructible<bounding_object>::value,
         "The bounding objects must be default constructible");

     if( number_of_elements > bvh_max_number_of_elements )
       {
         LOG( fatal, "internal structures cannot handle the requested number of elements");
         return;
       }
     else if( number_of_elements < 2 )
       {
         LOG( fatal, "not enough elements to create a bounding volume hierarchy");
         return;
       }
     m_nodes.resize( (m_number_of_internal_nodes << 1 ) + 1 );
     std::vector< uint64_t > morton_codes( m_number_of_internal_nodes + 1, 0 );
     set_leaf_nodes<bounding_object,bounded_element>( elements, m_nodes, m_number_of_internal_nodes, morton_codes );
     determine_nodes_hierarchy<bounding_object>( m_nodes.data(), morton_codes.data(), m_number_of_internal_nodes );
     build_bvh_tree_kernel_emulation<bounding_object>( m_nodes.data(), m_number_of_internal_nodes );
   }
} END_GO_NAMESPACE

# endif 
