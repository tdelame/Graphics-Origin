# include "common.h"
# include "../../graphics-origin/tools/memory.h"
namespace graphics_origin {
  namespace tools {
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

      test_suite* memory_arena_test_suite()
      {
        test_suite* suite = BOOST_TEST_SUITE("memory arena");
        ADD_TEST_CASE( constructor_called );
        return suite;
      }

    }
  }
}

