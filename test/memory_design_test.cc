# include "../graphics-origin/graphics_origin.h"
# include "../graphics-origin/tools/assert.h"
# include <stdlib.h>
# include <new>
namespace graphics_origin {

  namespace test {

    /**
     * I haven't touched important concepts for real-time engine, focusing on
     * having a (very) limited set of features to start to develop simple
     * applications.
     *
     * Memory management is one of such concept, and I will test the design
     * of memory management. Most of the ideas and source code come from
     * the Molecular Engine: have a look at https://blog.molecular-matters.com,
     * e.g. https://blog.molecular-matters.com/2011/07/07/memory-system-part-2/
     *
     */


    struct allocator {
      allocator(){}

      void* allocate( size_t size, const char* filename, int line_number )
      {
        (void)filename;
        (void)line_number;
        return malloc(size);
      }

      template<typename T >
      void deallocate( T* ptr )
      {
        free( ptr );
      }

    };

    namespace detail {

      template< typename T, class allocator >
      void deallocate( T* ptr, allocator& a )
      {
        ptr->~T();
        a.deallocate( ptr );
      }

      template< typename T, class allocator >
      T* new_array( allocator& a, const size_t& N, const char* file, int line, std::true_type )
      {
        return (T*)a.allocate(sizeof(T)*N, file, line);
      }

      template< typename T, class allocator >
      T* new_array( allocator& a, const size_t& N, const char* file, int line, std::false_type )
      {
        union
        {
          void* as_void;
          size_t* as_size_t;
          T* as_T;
        };

        as_void = a.allocate(sizeof(T)*N + sizeof(size_t), file, line);

        // store number of instances in first size_t bytes
        *as_size_t++ = N;

        // construct instances using placement new
        const T* const onePastLast = as_T + N;
        while (as_T < onePastLast)
          new (as_T++) T;

        // hand user the pointer to the first instance
        return (as_T - N);
      }

      template< typename T, class allocator >
      void delete_array( T* ptr, allocator& a, std::true_type )
      {
        a.deallocate( ptr );
      }

      template< typename T, class allocator >
      void delete_array( T* ptr, allocator& a, std::false_type )
      {
        union
        {
          size_t* as_size_t;
          T* as_T;
        };

        // user pointer points to first instance...
        as_T = ptr;

        // ...so go back size_t bytes and grab number of instances
        const size_t N = as_size_t[-1];

        // call instances' destructor in reverse order
        for (size_t i=N; i>0; --i)
          as_T[i-1].~T();

        a.deallocate(as_size_t-1);
      }

      template< typename T, class allocator >
      void delete_array( T* ptr, allocator& a )
      {
        delete_array( ptr, a, std::integral_constant<bool, std::is_pod<T>::value>());
      }
    }

    /**
     * optimize for POD, std::is_pod<T>::value
     */

# define go_new(type,an_allocator) new (an_allocator.allocate( \
   sizeof(type), \
   __FILE__, \
   __LINE__)) type

# define go_delete(object,an_allocator) detail::deallocate( object, an_allocator )

# define go_new_array(T,an_allocator) detail::new_array<std::remove_all_extents<T>::type>(an_allocator,\
  std::extent<T,0>::value, \
  __FILE__, \
  __LINE__, \
  std::integral_constant<bool,std::is_pod<T>::value>())

# define go_delete_array(object,an_allocator) detail::delete_array( object, an_allocator)

    static int execute( int argc, char* argv[] )
    {
      (void)argc;
      (void)argv;

      allocator test_allocator;

      int* pint = go_new(int,test_allocator);
      float* pfloat = go_new(float,test_allocator);

      *pint = 2;
      *pfloat = float(4.2);

      go_delete(pint, test_allocator);
      go_delete(pfloat, test_allocator);

      pint = go_new_array( int[5], test_allocator );
      pint[0] = 1;
      pint[1] = 2;
      pint[2] = 3;
      pint[3] = 4;
      pint[4] = 5;

      go_delete_array( pint, test_allocator );

      int a = 2;
      int b = 4;
      GO_ASSERT( a > b, "a was not greater than b ")(a,b);
      GO_ASSERT( b < a, "b was not less than a")(a,b);
      return 0;
    }
  }
}


int main( int argc, char* argv[] )
{
  return graphics_origin::test::execute( argc, argv );
}
