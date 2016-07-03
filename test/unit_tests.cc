# include "unit_tests/common.h"
# include "../../graphics-origin/tools/log.h"

namespace graphics_origin {
  namespace tools {
    namespace test {
      extern void add_test_suite();
    }
  }
  namespace geometry {
    namespace test {
      extern void add_test_suite();
    }
  }
  namespace application {
    namespace test {
      extern void add_test_suite();
    }
  }

  namespace test {
    static bool initialize_tests()
    {
      master_test_suite().p_name.value = "Graphics-Origin test suite";
      tools::test::add_test_suite();
      geometry::test::add_test_suite();
      application::test::add_test_suite();
      return true;
    }
  }
}

int main( int argc, char* argv[] )
{
  int result = boost::unit_test::unit_test_main(
      &graphics_origin::test::initialize_tests,
      argc, argv );
  graphics_origin::tools::flush_log();
  return result;
}
