# include "common.h"

namespace graphics_origin {
  namespace geometry {
    namespace test {
      void add_test_suite()
      {
        test_suite* suite = BOOST_TEST_SUITE("GEOMETRY LIBRARY");
        ADD_TO_MASTER( suite );
      }

    }
  }
}
