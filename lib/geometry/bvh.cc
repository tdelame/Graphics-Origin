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

# include "../../graphics-origin/geometry/bvh.h"

# include <atomic>

# include <thrust/sort.h>

BEGIN_GO_NAMESPACE
namespace geometry {
# define MCODE_SIZE   64
# define STACK_SIZE   64
# define MCODE_LENGTH (21)
# define MCODE_OFFSET (1<<(MCODE_LENGTH))
# define LEAF_MASK       0x80000000
# define LEAF_INDEX_MASK 0x7FFFFFFF
# define CLZ( a ) __builtin_clzl( a )

  struct morton_code_order1 {
    bool
    operator()( const box_bvh::leaf_node& a, const box_bvh::leaf_node& b ) const
    {
      return a.morton_code < b.morton_code;
    }
  };

  bool
  box_bvh::is_leaf( uint32_t node_index ) const
  {
     return (node_index & LEAF_MASK );
  }



  box_bvh::internal_node::internal_node()
    : parent_index{0}, left_index{0}, right_index{0}
  {}

  box_bvh::leaf_node::leaf_node()
    : morton_code{0}, parent_index{0}, element_index{0}
  {}

  const box_bvh::internal_node&
  box_bvh::get_root() const
  {
    return internals[0];
  }

  void
  box_bvh::set_leaf_nodes()
  {
    aabox bounding_box;
    elements[0].compute_bounding_box( bounding_box );

    # pragma omp parallel
    {
      aabox thread_bounding_box = bounding_box;
      # pragma omp for
      for( uint32_t i = 0; i < number_of_elements; ++ i )
        {
          auto& leaf = leaves[ i ];
          leaf.element_index = i;
          ((bounding_box_computer*)((char*)elements + sizeof_element * i))->compute_bounding_box( leaf.bounding );
          thread_bounding_box.merge( leaf.bounding );
        }

      # pragma omp critical
      bounding_box.merge( thread_bounding_box );
    }

    // now we have the bounding box, we can compute morton codes
    auto lower = bounding_box.get_min();
    auto inv_extents_times_mcode_offset = vec3{
      real(0.5 * MCODE_OFFSET) / bounding_box.get_half_sides().x,
      real(0.5 * MCODE_OFFSET) / bounding_box.get_half_sides().y,
      real(0.5 * MCODE_OFFSET) / bounding_box.get_half_sides().z
    };

    # pragma omp parallel for schedule(static)
    for( uint32_t i = 0; i < number_of_elements; ++ i )
      {
        auto& leaf = leaves[ i ];
        const auto& p = leaf.bounding.get_center();

        uint64_t a = (uint64_t)( (p.x - lower.x) * inv_extents_times_mcode_offset.x );
        uint64_t b = (uint64_t)( (p.y - lower.y) * inv_extents_times_mcode_offset.y );
        uint64_t c = (uint64_t)( (p.z - lower.z) * inv_extents_times_mcode_offset.z );

        for( uint j = 0; j < MCODE_LENGTH; ++ j )
          {
            leaf.morton_code |=
            ((((a >> (MCODE_LENGTH - 1 - j)) & 1) << ((MCODE_LENGTH - j) * 3 - 1)) |
             (((b >> (MCODE_LENGTH - 1 - j)) & 1) << ((MCODE_LENGTH - j) * 3 - 2)) |
             (((c >> (MCODE_LENGTH - 1 - j)) & 1) << ((MCODE_LENGTH - j) * 3 - 3)) );
          }
      }

    thrust::sort( leaves.begin(), leaves.end(), morton_code_order1{} );
  }


  int
  box_bvh::longest_common_prefix( const uint64_t& c1, int j )
  {
    if( j >= 0 && j < number_of_elements )
      return CLZ( c1 ^leaves[ j ].morton_code );
    return -1;
  }

  uivec2
  box_bvh::determine_range( uint32_t i )
  {
    const auto first_code = leaves[ i ].morton_code;

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

  uint32_t
  box_bvh::find_split( const uivec2& range )
  {
    const uint64_t first_code = leaves[ range.x ].morton_code;
    const uint64_t  last_code = leaves[ range.y ].morton_code;

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
            if( CLZ( first_code ^ leaves[ new_split].morton_code ) > common_prefix )
              split = new_split; // accept proposal
         }
      }
    while( step > 1 );
    return split;
  }


  void
  box_bvh::set_nodes_hierarchy()
  {
    const auto ninternals = internals.size();
    # pragma omp parallel for schedule(dynamic)
    for( uint32_t i = 0; i < ninternals; ++ i )
      {
        auto& intern = internals[ i ];
        uivec2 range = determine_range( i );
        uint32_t split = find_split( range );
        uint32_t child_index = split;
        if( split == range.x )
          {
            leaves[ split ].parent_index = i;
            child_index |= LEAF_MASK;
          }
        else
          {
            internals[ split ].parent_index = i;
          }
        intern.left_index = child_index;

        ++split;
        child_index = split;
        if( split == range.y )
          {
            leaves[ split ].parent_index = i;
            child_index |= LEAF_MASK;
          }
        else internals[ split ].parent_index = i;
        intern.right_index = child_index;
      }
  }

  struct kernel_build_bvh_tree_emulation {

    struct thread_variables {
        bool active;
        uint32_t index;
        unsigned int res;
      };

    kernel_build_bvh_tree_emulation(
      box_bvh::leaf_node* leaves,
      box_bvh::internal_node* internals,
      uint32_t nelements ) :
        leaves{leaves}, internals{ internals }, nelements{ nelements },
        counters(nelements)
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
      for( uint32_t i = 0; i < nelements; ++ i )
        {
          counters[ i ] = 0;
        }
    }

    void init_threads()
    {
      # pragma omp parallel for schedule(static)
      for( uint32_t i = 0; i < nelements; ++ i )
        {
          auto& var = variables[ i ];
          var.active = true;
          var.index = leaves[ i ].parent_index;
          var.res = counters[ var.index ].fetch_add(1);
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

      get_bbox( internals[ var.index ].left_index ).merge(
          get_bbox( internals[ var.index ].right_index ));

      if( var.index == 0 )
        {
          var.active = false;
          return false;
        }

      var.index = internals[ var.index ].parent_index;
      var.res = counters[ var.index ].fetch_add(1);
      return true;
    }

    aabox& get_bbox( uint32_t index )
    {
      if( index & LEAF_MASK )
        return leaves[ index & LEAF_INDEX_MASK ].bounding;
      return internals[ index ].bounding;
    }

    box_bvh::leaf_node* leaves;
    box_bvh::internal_node* internals;
    const uint32_t nelements;

    std::vector< std::atomic<int> > counters;
    std::vector< thread_variables > variables;
  };

  const box_bvh::internal_node&
  box_bvh::get_internal_node( uint32_t node_index ) const
  {
    assert( node_index + 1 < number_of_elements && "out of bound internal node index");
    return internals[ node_index ];
  }

  const box_bvh::leaf_node&
  box_bvh::get_leaf_node( uint32_t node_index ) const
  {
    assert( (node_index & LEAF_INDEX_MASK) < number_of_elements && "out of bound leaf node index");
    return leaves[ node_index & LEAF_INDEX_MASK ];
  }

  size_t
  box_bvh::get_number_of_nodes() const
  {
    return get_number_of_internal_nodes() + get_number_of_leaf_nodes();
  }

  size_t
  box_bvh::get_number_of_internal_nodes() const
  {
    return internals.size();
  }

  size_t
  box_bvh::get_number_of_leaf_nodes() const
  {
    return leaves.size();
  }

  void
  box_bvh::set_internal_bounding_boxes()
  {
    kernel_build_bvh_tree_emulation(
        leaves.data(),
        internals.data(),
        number_of_elements );
  }
}
END_GO_NAMESPACE
