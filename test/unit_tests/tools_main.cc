# include "common.h"

namespace graphics_origin {
  namespace tools {
    namespace test {

      extern test_suite* memory_test_suite();

      void add_test_suite()
      {
        test_suite* suite = BOOST_TEST_SUITE("TOOLS LIBRARY");
        ADD_TO_SUITE( memory_test_suite );
        ADD_TO_MASTER( suite );
      }

    }
  }
}
