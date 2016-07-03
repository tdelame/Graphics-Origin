# include "common.h"
# include "../../graphics-origin/tools/memory.h"
namespace graphics_origin {
  namespace tools {
    namespace test {

      static void no_guard_front_word_size()
      {
        const size_t word_size = bounds_checking_policy::no::size_front;
        BOOST_REQUIRE_EQUAL(
            word_size,
            0u);
      }

      static void no_guard_front_no_throw()
      {
        bounds_checking_policy::no checker;
        uint32_t word = 0;
        BOOST_REQUIRE_NO_THROW( checker.guard_front( &word ));
      }

      static void no_guard_front_does_nothing()
      {
        bounds_checking_policy::no checker;
        uint32_t word = 0;
        checker.guard_front( &word );
        BOOST_REQUIRE_EQUAL( word, 0u );
      }

      static void no_guard_front_write_no_word()
      {
        bounds_checking_policy::no checker;
        uint32_t words[5] = { 1, 2, 3, 4, 5 };
        checker.guard_front( words + 2 );
        BOOST_REQUIRE_EQUAL( words[0], 1u );
        BOOST_REQUIRE_EQUAL( words[1], 2u );
        BOOST_REQUIRE_EQUAL( words[2], 3u );
        BOOST_REQUIRE_EQUAL( words[3], 4u );
        BOOST_REQUIRE_EQUAL( words[4], 5u );
      }

      static void no_check_front_no_throw()
      {
        bounds_checking_policy::no checker;
        uint32_t word = 0;
        BOOST_REQUIRE_NO_THROW( checker.check_front( &word ));
      }

      static void no_guard_back_word_size()
      {
        const size_t word_size = bounds_checking_policy::no::size_back;
        BOOST_REQUIRE_EQUAL(
            word_size,
            0u);
      }

      static void no_guard_back_no_throw()
      {
        bounds_checking_policy::no checker;
        uint32_t word = 0;
        BOOST_REQUIRE_NO_THROW( checker.guard_back( &word ));
      }

      static void no_guard_back_does_nothing()
      {
        bounds_checking_policy::no checker;
        uint32_t word = 0;
        checker.guard_back( &word );
        BOOST_REQUIRE_EQUAL( word, 0u );
      }

      static void no_guard_back_write_no_word()
      {
        bounds_checking_policy::no checker;
        uint32_t words[5] = { 1, 2, 3, 4, 5 };
        checker.guard_back( words + 2 );
        BOOST_REQUIRE_EQUAL( words[0], 1u );
        BOOST_REQUIRE_EQUAL( words[1], 2u );
        BOOST_REQUIRE_EQUAL( words[2], 3u );
        BOOST_REQUIRE_EQUAL( words[3], 4u );
        BOOST_REQUIRE_EQUAL( words[4], 5u );
      }

      static void no_check_back_no_throw()
      {
        bounds_checking_policy::no checker;
        uint32_t word = 0;
        BOOST_REQUIRE_NO_THROW( checker.check_back( &word ));
      }

      static void per_allocation_guard_front_word_size()
      {
        BOOST_REQUIRE_EQUAL(
            sizeof(bounds_checking_policy::per_allocation::front_word),
            sizeof(uint32_t));
      }

      static void per_allocation_guard_front_no_throw()
      {
        bounds_checking_policy::per_allocation checker;
        uint32_t word = 0;
        BOOST_REQUIRE_NO_THROW(checker.guard_front( &word ));
      }

      static void per_allocation_guard_front_write_expected_word()
      {
        bounds_checking_policy::per_allocation checker;
        uint32_t word = 0;
        checker.guard_front( &word );
        BOOST_REQUIRE_EQUAL(
            word,
            0xFEEDFACEu);
      }

      static void per_allocation_guard_front_write_only_one_word()
      {
        bounds_checking_policy::per_allocation checker;
        uint32_t words[5] = { 1, 2, 3, 4, 5 };
        checker.guard_front( words + 2 );
        BOOST_REQUIRE_EQUAL( words[0], 1u );
        BOOST_REQUIRE_EQUAL( words[1], 2u );
        BOOST_REQUIRE_EQUAL( words[2], 0xFEEDFACEu );
        BOOST_REQUIRE_EQUAL( words[3], 4u );
        BOOST_REQUIRE_EQUAL( words[4], 5u );
      }

      static void per_allocation_check_front_no_throw()
      {
        bounds_checking_policy::per_allocation checker;
        uint32_t word;
        checker.guard_front( &word );
        BOOST_REQUIRE_NO_THROW( checker.check_front(&word));
      }

      static void per_allocation_check_front()
      {
        bounds_checking_policy::per_allocation checker;
        uint32_t word;
        checker.guard_front( &word );
        checker.check_front(&word); // this would raise a signal if it fails
        BOOST_REQUIRE( true );
      }

      static void per_allocation_guard_back_word_size()
      {
        BOOST_REQUIRE_EQUAL(
            sizeof(bounds_checking_policy::per_allocation::back_word),
            sizeof(uint32_t));
      }

      static void per_allocation_guard_back_no_throw()
      {
        bounds_checking_policy::per_allocation checker;
        uint32_t word = 0;
        BOOST_REQUIRE_NO_THROW(checker.guard_back( &word ));
      }

      static void per_allocation_guard_back_write_expected_word()
      {
        bounds_checking_policy::per_allocation checker;
        uint32_t word = 0;
        checker.guard_back( &word );
        BOOST_REQUIRE_EQUAL(
            word,
            0x1BADC0DEu);
      }

      static void per_allocation_guard_back_write_only_one_word()
      {
        bounds_checking_policy::per_allocation checker;
        uint32_t words[5] = { 0, 0, 0, 0, 0 };
        checker.guard_back( words + 2 );
        BOOST_REQUIRE_EQUAL( words[0], 0u );
        BOOST_REQUIRE_EQUAL( words[1], 0u );
        BOOST_REQUIRE_EQUAL( words[2], 0x1BADC0DEu );
        BOOST_REQUIRE_EQUAL( words[3], 0u );
        BOOST_REQUIRE_EQUAL( words[4], 0u );
      }

      static void per_allocation_check_back_no_throw()
      {
        bounds_checking_policy::per_allocation checker;
        uint32_t word;
        checker.guard_back( &word );
        BOOST_REQUIRE_NO_THROW( checker.check_back(&word));
      }

      static void per_allocation_check_back()
      {
        bounds_checking_policy::per_allocation checker;
        uint32_t word;
        checker.guard_back( &word );
        checker.check_back(&word); // this would raise a signal if it fails
        BOOST_REQUIRE( true );
      }

      test_suite* bounds_checking_test_suite()
      {
        test_suite* suite = BOOST_TEST_SUITE("bounds checking");

        ADD_TEST_CASE( no_guard_front_word_size );
        ADD_TEST_CASE( no_guard_front_no_throw );
        ADD_TEST_CASE( no_guard_front_does_nothing );
        ADD_TEST_CASE( no_guard_front_write_no_word );
        ADD_TEST_CASE( no_check_front_no_throw );

        ADD_TEST_CASE( no_guard_back_word_size );
        ADD_TEST_CASE( no_guard_back_no_throw );
        ADD_TEST_CASE( no_guard_back_does_nothing );
        ADD_TEST_CASE( no_guard_back_write_no_word );
        ADD_TEST_CASE( no_check_back_no_throw );

        ADD_TEST_CASE( per_allocation_guard_front_word_size );
        ADD_TEST_CASE( per_allocation_guard_front_no_throw );
        ADD_TEST_CASE( per_allocation_guard_front_write_expected_word );
        ADD_TEST_CASE( per_allocation_guard_front_write_only_one_word );
        ADD_TEST_CASE( per_allocation_check_front_no_throw );
        ADD_TEST_CASE( per_allocation_check_front );

        ADD_TEST_CASE( per_allocation_guard_back_word_size );
        ADD_TEST_CASE( per_allocation_guard_back_no_throw );
        ADD_TEST_CASE( per_allocation_guard_back_write_expected_word );
        ADD_TEST_CASE( per_allocation_guard_back_write_only_one_word );
        ADD_TEST_CASE( per_allocation_check_back_no_throw );
        ADD_TEST_CASE( per_allocation_check_back );
        return suite;
      }
    }
  }
}



