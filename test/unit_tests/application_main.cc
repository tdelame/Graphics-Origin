# include "common.h"

namespace graphics_origin {
  namespace application {
    namespace test {

      void add_test_suite()
      {
        test_suite* suite = BOOST_TEST_SUITE("APPLICATION LIBRARY");
        ADD_TO_MASTER( suite );
      }

    }
  }
}
