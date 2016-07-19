# include "../graphics-origin/graphics_origin.h"
# include "../graphics-origin/tools/memory.h"

# include <iostream>
# include <vector>

namespace graphics_origin {

  namespace test {


    static int execute( int argc, char* argv[] )
    {
      (void)argc;
      (void)argv;

      const auto page_size = tools::virtual_memory::get_page_size();
      std::cout << "page size = " << page_size << std::endl;

      tools::allocation_policy::growing_stack allocator( page_size * 40, page_size );
      tools::memory_arena<
        tools::allocation_policy::growing_stack,
        tools::thread_policy::single_thread,
        tools::bounds_checking_policy::per_allocation,
        tools::memory_tracking_policy::no,
        tools::memory_tagging_policy::yes > arena( &allocator );

      std::vector< char* > allocations ( page_size * 2, nullptr );
      std::cout <<"committed before allocation: " << allocator.get_committed_memory() << std::endl;
      for( unsigned int i = 0; i < page_size * 2; ++ i )
        {
          allocations[ i ] = go_new( char, arena );
        }

      std::cout <<"committed after allocation : " << allocator.get_committed_memory() << std::endl;
      for( unsigned int i = page_size * 2 - 1; i >= (page_size >> 1); -- i )
        {
          go_delete( allocations[ i ], arena );
        }
      std::cout <<"committed after delete     : " << allocator.get_committed_memory() << std::endl;
      allocator.purge();

      std::cout <<"committed after purge      : " << allocator.get_committed_memory() << std::endl;
      return 0;
    }
  }
}


int main( int argc, char* argv[] )
{
  return graphics_origin::test::execute( argc, argv );
}
