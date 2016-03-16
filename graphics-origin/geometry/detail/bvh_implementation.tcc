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

# include <atomic>
# include <thrust/sort.h>

BEGIN_GO_NAMESPACE namespace geometry {

  static constexpr uint8_t  mcode_size = 64;
  static constexpr uint8_t  mcode_length = 21;
  static constexpr uint32_t mcode_offset = 1U << mcode_length;
  static constexpr uint32_t leaf_mask = 0x80000000;
  static constexpr uint32_t leaf_index_mask = 0x7FFFFFFF;

# define CLZ( a ) __builtin_clzl( a )

  template< typename bounding_object >
  bvh<bounding_object>::internal_node::internal_node()
    : parent_index{0}, left_index{0}, right_index{0}
  {}

  template< typename bounding_object >
  bvh<bounding_object>::leaf_node::leaf_node()
    : morton_code{0}, parent_index{0}, element_index{0}
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
        typename bvh<aabox>::leaf_node* leaves,
        size_t number_of_leaves )
    {
      # pragma omp parallel for schedule(static)
      for( uint32_t i = 0; i < number_of_leaves; ++ i )
        {
          auto& leaf = leaves[ i ];
          leaf.element_index = i;
          elements[i].compute_bounding_box( leaf.bounding );
        }

      // now we have the bounding box, we can compute morton codes
      auto lower = root_bounding_object.get_min();
      auto inv_extents_times_mcode_offset = vec3{
        real(0.5 * mcode_offset) / root_bounding_object.get_half_sides().x,
        real(0.5 * mcode_offset) / root_bounding_object.get_half_sides().y,
        real(0.5 * mcode_offset) / root_bounding_object.get_half_sides().z
      };

      # pragma omp parallel for schedule(static)
      for( uint32_t i = 0; i < number_of_leaves; ++ i )
        {
          auto& leaf = leaves[ i ];
          const auto& p = leaf.bounding.get_center();

          uint64_t a = (uint64_t)( (p.x - lower.x) * inv_extents_times_mcode_offset.x );
          uint64_t b = (uint64_t)( (p.y - lower.y) * inv_extents_times_mcode_offset.y );
          uint64_t c = (uint64_t)( (p.z - lower.z) * inv_extents_times_mcode_offset.z );

          for( uint j = 0; j < mcode_length; ++ j )
            {
              leaf.morton_code |=
              ((((a >> (mcode_length - 1 - j)) & 1) << ((mcode_length - j) * 3 - 1)) |
               (((b >> (mcode_length - 1 - j)) & 1) << ((mcode_length - j) * 3 - 2)) |
               (((c >> (mcode_length - 1 - j)) & 1) << ((mcode_length - j) * 3 - 3)) );
            }
        }
   # ifdef GO_USE_CUDA_THRUST
      thrust::sort( leaves, leaves + number_of_leaves, morton_code_order<aabox>{} );
  # else
      //todo: a parallel version with OpenMP
      std::sort( leaves, leaves + number_of_leaves, morton_code_order<aabox>{} );
  # endif
    }

    set_leaf_nodes(
        const bounded_element* elements,
        typename bvh<aabox>::leaf_node* leaves,
        size_t number_of_leaves )
    {
      aabox bounding;
      elements[0].compute_bounding_box( bounding );

      # pragma omp parallel
      {
        aabox thread_bounding = bounding;
        # pragma omp for  schedule(static)
        for( uint32_t i = 0; i < number_of_leaves; ++ i )
          {
            auto& leaf = leaves[ i ];
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
        real(0.5 * mcode_offset) / bounding.get_half_sides().x,
        real(0.5 * mcode_offset) / bounding.get_half_sides().y,
        real(0.5 * mcode_offset) / bounding.get_half_sides().z
      };

      # pragma omp parallel for schedule(static)
      for( uint32_t i = 0; i < number_of_leaves; ++ i )
        {
          auto& leaf = leaves[ i ];
          const auto& p = leaf.bounding.get_center();

          uint64_t a = (uint64_t)( (p.x - lower.x) * inv_extents_times_mcode_offset.x );
          uint64_t b = (uint64_t)( (p.y - lower.y) * inv_extents_times_mcode_offset.y );
          uint64_t c = (uint64_t)( (p.z - lower.z) * inv_extents_times_mcode_offset.z );

          for( uint j = 0; j < mcode_length; ++ j )
            {
              leaf.morton_code |=
              ((((a >> (mcode_length - 1 - j)) & 1) << ((mcode_length - j) * 3 - 1)) |
               (((b >> (mcode_length - 1 - j)) & 1) << ((mcode_length - j) * 3 - 2)) |
               (((c >> (mcode_length - 1 - j)) & 1) << ((mcode_length - j) * 3 - 3)) );
            }
        }
   # ifdef GO_USE_CUDA_THRUST
      thrust::sort( leaves, leaves + number_of_leaves, morton_code_order<aabox>{} );
  # else
      //todo: a parallel version with OpenMP
      std::sort( leaves, leaves + number_of_leaves, morton_code_order<aabox>{} );
  # endif
    }
  };

  template< typename bounded_element >
  struct set_leaf_nodes< ball, bounded_element> {

    set_leaf_nodes(
        const bounded_element* elements,
        ball& root_bounding_object,
        typename bvh<ball>::leaf_node* leaves,
        size_t number_of_leaves )
    {
      static_assert(
        geometric_traits<bounded_element>::is_bounding_ball_computer,
        "The elements bounded by bounding balls in the BVH must be able to compute a bounding ball");

      # pragma omp parallel for schedule(static)
      for( uint32_t i = 0; i < number_of_leaves; ++ i )
        {
          auto& leaf = leaves[ i ];
          leaf.element_index = i;
          elements[i].compute_bounding_ball( leaf.bounding );
        }

      // now we have the bounding ball, we can compute morton codes
      auto lower = vec3{root_bounding_object} - vec3{root_bounding_object.w, root_bounding_object.w, root_bounding_object.w};
      auto inv_extents_times_mcode_offset = vec3{
        real(0.5 * mcode_offset) / root_bounding_object.w,
        real(0.5 * mcode_offset) / root_bounding_object.w,
        real(0.5 * mcode_offset) / root_bounding_object.w
      };

      # pragma omp parallel for
      for( uint32_t i = 0; i < number_of_leaves; ++ i )
        {
          auto& leaf = leaves[ i ];
          const auto& p = leaf.bounding;

          uint64_t a = (uint64_t)( (p.x - lower.x) * inv_extents_times_mcode_offset.x );
          uint64_t b = (uint64_t)( (p.y - lower.y) * inv_extents_times_mcode_offset.y );
          uint64_t c = (uint64_t)( (p.z - lower.z) * inv_extents_times_mcode_offset.z );

          for( uint j = 0; j < mcode_length; ++ j )
            {
              leaf.morton_code |=
              ((((a >> (mcode_length - 1 - j)) & 1) << ((mcode_length - j) * 3 - 1)) |
               (((b >> (mcode_length - 1 - j)) & 1) << ((mcode_length - j) * 3 - 2)) |
               (((c >> (mcode_length - 1 - j)) & 1) << ((mcode_length - j) * 3 - 3)) );
            }
        }
  # ifdef GO_USE_CUDA_THRUST
      thrust::sort( leaves, leaves, morton_code_order<ball>{} );
  # else
      //todo: a parallel version with OpenMP
      std::sort( m_leaves, m_leaves + number_of_leaves, morton_code_order<ball>{} );
  # endif
    }

    set_leaf_nodes(
        const bounded_element* elements,
        typename bvh<ball>::leaf_node* leaves,
        size_t number_of_leaves )
    {
      static_assert(
        geometric_traits<bounded_element>::is_bounding_ball_computer,
        "The elements bounded by bounding balls in the BVH must be able to compute a bounding ball");

      ball bounding;
      elements[0].compute_bounding_ball( bounding );

      # pragma omp parallel
      {
        ball thread_bounding = bounding;
        # pragma omp for
        for( uint32_t i = 0; i < number_of_leaves; ++ i )
          {
            auto& leaf = leaves[ i ];
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

      # pragma omp parallel for
      for( uint32_t i = 0; i < number_of_leaves; ++ i )
        {
          auto& leaf = leaves[ i ];
          const auto& p = leaf.bounding;

          uint64_t a = (uint64_t)( (p.x - lower.x) * inv_extents_times_mcode_offset.x );
          uint64_t b = (uint64_t)( (p.y - lower.y) * inv_extents_times_mcode_offset.y );
          uint64_t c = (uint64_t)( (p.z - lower.z) * inv_extents_times_mcode_offset.z );

          for( uint j = 0; j < mcode_length; ++ j )
            {
              leaf.morton_code |=
              ((((a >> (mcode_length - 1 - j)) & 1) << ((mcode_length - j) * 3 - 1)) |
               (((b >> (mcode_length - 1 - j)) & 1) << ((mcode_length - j) * 3 - 2)) |
               (((c >> (mcode_length - 1 - j)) & 1) << ((mcode_length - j) * 3 - 3)) );
            }
        }
  # ifdef GO_USE_CUDA_THRUST
      thrust::sort( leaves, leaves, morton_code_order<ball>{} );
  # else
      //todo: a parallel version with OpenMP
      std::sort( m_leaves, m_leaves + number_of_leaves, morton_code_order<ball>{} );
  # endif
    }
  };

  template< typename bounding_object >
  struct determine_nodes_hierarchy {
    int longest_common_prefix( const uint64_t& c1, int j )
    {
      if( j >= 0 && j < m_number_of_leaves )
        return CLZ( c1 ^ m_leaves[ j ].morton_code );
      return -1;
    }

    uivec2 determine_range( uint32_t i )
    {
      const auto first_code = m_leaves[ i ].morton_code;

      // determine the direction of the range (+1 or -1)
      int d = longest_common_prefix( first_code, i + 1 )
            - longest_common_prefix( first_code, i - 1 ) > 0 ? 1 : -1;

      // compute upper bound for the length of the range
      int d_min = longest_common_prefix( first_code, i - d );
      int lmax = 2;
      while( longest_common_prefix( first_code, i + lmax * d ) > d_min )
        lmax <<= 1;

      // find the other end using binary search
      uint32_t l = 0;
      for( uint32_t t = lmax >> 1; t >= 1; t >>= 1 )
        {
          if( longest_common_prefix( first_code, i + ( l + t ) * d ) > d_min )
            l += t;
        }

      if( d > 0 )
        return uivec2( i, i + l );
      return uivec2( i - l, i );
    }

    uint32_t find_split( const uivec2& range )
    {
      const uint64_t first_code = m_leaves[ range.x ].morton_code;
      const uint64_t  last_code = m_leaves[ range.y ].morton_code;

      // Identical Morton codes => split the range in the middle.
      if( first_code == last_code )
        return (range.x + range.y) >> 1;

      // Calculate the number of highest bits that are the same
      // for all objects, using the count-leading-zeros intrinsic.
      int common_prefix = CLZ( first_code ^ last_code );

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
              if( CLZ( first_code ^ m_leaves[ new_split].morton_code ) > common_prefix )
                split = new_split; // accept proposal
           }
        }
      while( step > 1 );
      return split;
    }

    determine_nodes_hierarchy(
        typename bvh<bounding_object>::leaf_node* leaves,
        typename bvh<bounding_object>::internal_node* internals,
        size_t number_of_internals )
      : m_leaves{ leaves }, m_internals{ internals },
        m_number_of_internals{ number_of_internals }, m_number_of_leaves{ m_number_of_internals + 1 }
    {
      # pragma omp parallel for schedule(dynamic)
      for( uint32_t i = 0; i < m_number_of_internals; ++ i )
        {
          auto& intern = m_internals[ i ];
          uivec2 range = determine_range( i );
          uint32_t split = find_split( range );
          uint32_t child_index = split;
          if( split == range.x )
            {
              m_leaves[ split ].parent_index = i;
              child_index |= leaf_mask;
            }
          else
            {
              m_internals[ split ].parent_index = i;
            }
          intern.left_index = child_index;

          ++split;
          child_index = split;
          if( split == range.y )
            {
              m_leaves[ split ].parent_index = i;
              child_index |= leaf_mask;
            }
          else m_internals[ split ].parent_index = i;
          intern.right_index = child_index;
        }
    }

    typename bvh<bounding_object>::internal_node* m_internals;
    typename bvh<bounding_object>::leaf_node* m_leaves;

    const size_t m_number_of_internals;
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
      typename bvh<bounding_object>::leaf_node* leaves,
      typename bvh<bounding_object>::internal_node* internals,
      uint32_t nelements ) :
        m_leaves{leaves}, m_internals{ internals }, m_nelements{ nelements },
        m_counters(nelements)
    {
      variables.resize( nelements );
      init_counters();
      init_threads();

      bool activity = true;
      while( activity )
        {
          activity = false;
          # pragma omp parallel for schedule(static)
          for( uint32_t i = 0; i < nelements; ++ i )
            {
              if( emulate_one_thread_loop( i ) )
                activity = true;
            }
        }
    }

    void init_counters()
    {
      # pragma omp parallel for schedule(static)
      for( uint32_t i = 0; i < m_nelements; ++ i )
        {
          m_counters[ i ] = 0;
        }
    }

    void init_threads()
    {
      # pragma omp parallel for schedule(static)
      for( uint32_t i = 0; i < m_nelements; ++ i )
        {
          auto& var = variables[ i ];
          var.active = true;
          var.index = m_leaves[ i ].parent_index;
          var.res = m_counters[ var.index ].fetch_add(1);
        }
    }

    bool emulate_one_thread_loop( uint32_t tid )
    {
      auto& var = variables[ tid ];
      if( !var.active ) return false;
      if( !var.res )
        {
          var.active = false;
          return false;
        }

      get_bo( m_internals[ var.index ].left_index ).merge(
          get_bo( m_internals[ var.index ].right_index ));

      if( var.index == 0 )
        {
          var.active = false;
          return false;
        }

      var.index = m_internals[ var.index ].parent_index;
      var.res = m_counters[ var.index ].fetch_add(1);
      return true;
    }

    bounding_object& get_bo( uint32_t index )
    {
      if( index & leaf_mask )
        return m_leaves[ index & leaf_index_mask ].bounding;
      return m_internals[ index ].bounding;
    }

    typename bvh<bounding_object>::leaf_node* m_leaves;
    typename bvh<bounding_object>::internal_node* m_internals;
    const uint32_t m_nelements;

    std::vector< std::atomic<int> > m_counters;
    std::vector< thread_variables > variables;
  };

  template< typename bounding_object >
   template< typename bounded_element >
   bvh<bounding_object>::bvh( const bounded_element* elements, bounding_object& root_bounding_object, size_t number_of_elements )
     : m_elements{ number_of_elements }
   {
     static_assert(
         std::is_default_constructible<bounding_object>::value,
         "The bounding objects must be default constructible");

     if( m_elements > max_number_of_elements )
       {
         LOG( fatal, "internal structures cannot handle the requested number of elements");
         return;
       }
     else if( m_elements < 2 )
       {
         LOG( fatal, "not enough elements to create a bounding volume hierarchy");
         return;
       }
     m_leaves.resize( m_elements );
     m_internals.resize( m_elements - 1 );
     set_leaf_nodes<bounding_object,bounded_element>( elements, root_bounding_object, m_leaves.data(), m_leaves.size() );
     determine_nodes_hierarchy<bounding_object>( m_leaves.data(), m_internals.data(), m_internals.size() );
     build_bvh_tree_kernel_emulation<bounding_object>( m_leaves.data(), m_internals.data(), m_leaves.size() );
   }


  template< typename bounding_object >
   template< typename bounded_element >
   bvh<bounding_object>::bvh(
       const bounded_element* elements,
       size_t number_of_elements )
     : m_elements{ number_of_elements }
   {
     static_assert(
         std::is_default_constructible<bounding_object>::value,
         "The bounding objects must be default constructible");

     if( m_elements > max_number_of_elements )
       {
         LOG( fatal, "internal structures cannot handle the requested number of elements");
         return;
       }
     else if( m_elements < 2 )
       {
         LOG( fatal, "not enough elements to create a bounding volume hierarchy");
         return;
       }
     m_leaves.resize( m_elements );
     m_internals.resize( m_elements - 1 );
     set_leaf_nodes<bounding_object,bounded_element>( elements, m_leaves.data(), m_leaves.size() );
     determine_nodes_hierarchy<bounding_object>( m_leaves.data(), m_internals.data(), m_internals.size() );
     build_bvh_tree_kernel_emulation<bounding_object>( m_leaves.data(), m_internals.data(), m_leaves.size() );
   }
 template< typename bounding_object >
 bool bvh<bounding_object>::is_leaf( uint32_t node_index ) const
 {
   return node_index & leaf_mask;
 }

} END_GO_NAMESPACE

# endif 
