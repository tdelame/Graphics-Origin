# include "common.h"
# include "../../graphics-origin/tools/memory.h"
namespace graphics_origin {
  namespace tools {
    namespace test {

      static void on_stack_creation()
      {
        BOOST_REQUIRE_NO_THROW( memory_area::on_stack<100> a );
      }

      static void on_stack_begin_no_throw()
      {
        static constexpr size_t size = 100;
        memory_area::on_stack<size> a;
        BOOST_REQUIRE_NO_THROW( a.begin() );
      }

      static void on_stack_begin_expected_address()
      {
        static constexpr size_t size = 100;
        memory_area::on_stack<size> a;
        BOOST_REQUIRE_EQUAL( reinterpret_cast<void*>(&a), a.begin() );
      }

      static void on_stack_end_no_throw()
      {
        static constexpr size_t size = 100;
        memory_area::on_stack<size> a;
        BOOST_REQUIRE_NO_THROW( a.end() );
      }

      static void on_stack_end_expected_address()
      {
        static constexpr size_t size = 100;
        memory_area::on_stack<size> a;
        BOOST_REQUIRE_EQUAL( reinterpret_cast<char*>(&a) + size, reinterpret_cast<char*>(a.end()) );
      }

      static void on_stack_area_access_no_throw()
      {
        static constexpr size_t size = 100;
        memory_area::on_stack<size> a;
        for( size_t i = 0; i < size; ++ i )
          {
            BOOST_REQUIRE_NO_THROW( reinterpret_cast<char*>(a.begin())[i] = 'T' );
          }
      }

      static test_suite* on_stack_test_suite()
      {
        test_suite* suite = BOOST_TEST_SUITE("memory_area::on_stack");
        ADD_TEST_CASE( on_stack_creation );
        ADD_TEST_CASE( on_stack_begin_no_throw );
        ADD_TEST_CASE( on_stack_begin_expected_address );
        ADD_TEST_CASE( on_stack_end_no_throw );
        ADD_TEST_CASE( on_stack_end_expected_address );
        ADD_TEST_CASE( on_stack_area_access_no_throw );
        return suite;
      }

      static void on_heap_creation()
      {
        BOOST_REQUIRE_NO_THROW( memory_area::on_heap a(100) );
      }

      static void on_heap_begin_no_throw()
      {
        static constexpr size_t size = 100;
        memory_area::on_heap a(size);
        BOOST_REQUIRE_NO_THROW( a.begin() );
      }

      static void on_heap_end_no_throw()
      {
        static constexpr size_t size = 100;
        memory_area::on_heap a(size);
        BOOST_REQUIRE_NO_THROW( a.end() );
      }

      static void on_heap_end_expected_address()
      {
        static constexpr size_t size = 100;
        memory_area::on_heap a(size);
        BOOST_REQUIRE_EQUAL( reinterpret_cast<char*>(&a) + size, reinterpret_cast<char*>(a.end()) );
      }

      static void on_heap_area_access_no_throw()
      {
        static constexpr size_t size = 100;
        memory_area::on_heap a(size);
        for( size_t i = 0; i < size; ++ i )
          {
            BOOST_REQUIRE_NO_THROW( reinterpret_cast<char*>(a.begin())[i] = 'T' );
          }
      }

      static test_suite* on_heap_test_suite()
      {
        test_suite* suite = BOOST_TEST_SUITE("memory_area::on_heap");
        ADD_TEST_CASE( on_heap_creation );
        ADD_TEST_CASE( on_heap_begin_no_throw );
        ADD_TEST_CASE( on_heap_end_no_throw );
        ADD_TEST_CASE( on_heap_end_expected_address );
        ADD_TEST_CASE( on_heap_area_access_no_throw );
        return suite;
      }

      test_suite* memory_area_test_suite()
      {
        test_suite* suite = BOOST_TEST_SUITE("memory area");
        ADD_TO_SUITE( on_stack_test_suite );
        ADD_TO_SUITE( on_heap_test_suite );
        return suite;
      }

    }
  }
}

