# include "common.h"
# include "../../graphics-origin/tools/memory.h"
namespace graphics_origin {
  namespace tools {
    namespace test {

      test_suite* memory_arena_test_suite();
      test_suite* bounds_checking_test_suite();

      test_suite* memory_test_suite()
      {
        test_suite* suite = BOOST_TEST_SUITE("memory");
        ADD_TO_SUITE( memory_arena_test_suite );
        ADD_TO_SUITE( bounds_checking_test_suite );
        return suite;
      }
    }
  }
}
