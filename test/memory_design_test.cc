# include "../graphics-origin/graphics_origin.h"
# include "../graphics-origin/tools/memory.h"
namespace graphics_origin {

  namespace test {

    static int execute( int argc, char* argv[] )
    {
      (void)argc;
      (void)argv;

      static constexpr size_t area_size = 10000;
      char* area[ area_size ];

      tools::allocation_policy::linear allocator( area, area + area_size );

      tools::memory_arena<
        tools::allocation_policy::linear,
        tools::thread_policy::single_thread,
        tools::bounds_checking_policy::per_allocation,
        tools::memory_tracking_policy::no,
        tools::memory_tagging_policy::yes >
      memory_arena( &allocator );


      int* pint = go_new( int, memory_arena );

      *pint = 2;

      go_delete( pint, memory_arena );

      float* pfloat = go_new_array( float[4], memory_arena );

      pfloat[0] = 1;
      pfloat[1] = 2;
      pfloat[2] = 3;
      pfloat[3] = 4;

      go_delete_array( pfloat, memory_arena );

      return 0;
    }
  }
}


int main( int argc, char* argv[] )
{
  return graphics_origin::test::execute( argc, argv );
}
