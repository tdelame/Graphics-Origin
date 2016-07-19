# ifndef GRAPHICS_ORIGIN_TESTS_TEST_H_
# define GRAPHICS_ORIGIN_TESTS_TEST_H_
# define BOOST_TEST_ALTERNATIVE_INIT_API
# include "../../graphics-origin/graphics_origin.h"
# include <boost/test/unit_test.hpp>

using boost::unit_test::framework::master_test_suite;
using boost::unit_test::test_suite;

# define ADD_TEST_CASE( test ) suite->add( BOOST_TEST_CASE( test ) )
# define ADD_TO_MASTER( suite ) master_test_suite().add( suite )
# define ADD_TO_SUITE( s ) suite->add( s() )

# define REAL_TEST_CLOSE( observed, expected, small, pct_tol, type ) \
  if( std::abs( expected ) < small )                                 \
    {                                                                \
      BOOST_##type##_SMALL( observed, small );                       \
    }                                                                \
  else                                                               \
    {                                                                \
      BOOST_##type##_CLOSE( expected, observed, pct_tol );           \
    }
# define REAL_CHECK_CLOSE( observed, expected, small, pct_tol ) \
    REAL_TEST_CLOSE( observed, expected, small, pct_tol, CHECK )
# define REAL_REQUIRE_CLOSE( observed, expected, small, pct_tol ) \
    REAL_TEST_CLOSE( observed, expected, small, pct_tol, REQUIRE )
# endif
