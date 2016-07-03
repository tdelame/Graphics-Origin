# include "../graphics-origin/graphics_origin.h"
# include "../graphics-origin/tools/memory.h"

# include <omp.h>
# include <iostream>
# include <vector>
namespace graphics_origin {

  namespace test {

    static void constructor_called()
    {
      struct constructor_called_data {
        constructor_called_data( unsigned int i, bool b ) :
          i{ i }, is_odd{ b }
        {}
        unsigned int i;
        bool is_odd;
      };

      static constexpr size_t instances = 100;
      tools::memory_area::on_stack<(sizeof(constructor_called_data)+tools::allocation_policy::linear::size_front)*(instances)> area;
      tools::allocation_policy::linear allocator( area );
      tools::memory_arena<
        tools::allocation_policy::linear,
        tools::thread_policy::single_thread,
        tools::bounds_checking_policy::no,
        tools::memory_tracking_policy::no,
        tools::memory_tagging_policy::no >
      memory_arena( &allocator );

      std::vector< constructor_called_data* > allocations( instances, nullptr );
      for( size_t i = 0; i < instances; ++ i )
        {
          allocations[i] = go_new_align( constructor_called_data, 1, memory_arena )( i, i & 1 );
        }

      for( size_t i = 0; i < instances; ++ i )
        {
          constructor_called_data* ptr = allocations[i];
          assert( ptr->i == i );
          assert( ptr->is_odd == (i & 1 ) );
        }
    }

    static void destructor_called()
    {
      struct destructor_called_data {

      };
    }



    static int execute( int argc, char* argv[] )
    {
      (void)argc;
      (void)argv;

      constructor_called();

      struct data {
        data( double d, unsigned int ui, bool b ) :
          d{ d }, ui{ ui }, b{ b }
        {}

        double d;
        unsigned int ui;
        bool b;
      };

      static constexpr size_t area_size = 100000000;
      tools::memory_area::on_heap area( area_size );
      tools::allocation_policy::linear allocator( area );

      tools::memory_arena<
        tools::allocation_policy::linear,
        tools::thread_policy::single_thread,
        tools::bounds_checking_policy::no,
        tools::memory_tracking_policy::no,
        tools::memory_tagging_policy::no >
      memory_arena( &allocator );

      size_t instances = 1000000;
      std::vector< data* > allocator_instances( instances, nullptr );

      std::cout << "allocating " << instances << " variables inside already available memory vs using new" << std::endl;

      auto start = omp_get_wtime();
      for( int i = 0; i < instances; ++ i )
        {
          allocator_instances[i] =
              go_new( data, memory_arena ){ double(i) * 1.0, uint(i), i & 1 };
        }
      auto stop = omp_get_wtime();
      std::cout << "heap allocator    = " << (stop - start) * 1000.0 << " ms" << std::endl;

      std::vector< data* > new_instances( instances, nullptr );
      start = omp_get_wtime();
      for( int i = 0; i < instances; ++ i )
        {
          new_instances[i] = new data{ double(i) * 1.0, uint(i), i & 1 };
        }
      stop = omp_get_wtime();
      std::cout << "without allocator = " << (stop - start) * 1000.0 << " ms" << std::endl;

      for( auto& d : new_instances )
        delete d;
      return 0;
    }
  }
}


int main( int argc, char* argv[] )
{
  return graphics_origin::test::execute( argc, argv );
}
