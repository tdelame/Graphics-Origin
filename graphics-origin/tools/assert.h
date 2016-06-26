# ifndef GRAPHICS_ORIGIN_ASSERT_H_
# define GRAPHICS_ORIGIN_ASSERT_H_
# include <iostream>
# include <stdio.h>
# include <cstdarg>
namespace graphics_origin {
  namespace tools {

    /**
     * A very nice idea from Stefan Reinalter
     * https://blog.molecular-matters.com/2011/07/22/an-improved-assert/
     */
    struct assert{
      // maybe I should use std::ostream here
      assert( const char* filename, int line_number, const char* format, ... )
      {
        printf( "%s (%i): [ASSERT] ", filename, line_number );
        va_list args;
        va_start( args, format );
        vprintf( format, args );
        printf("\n");
        va_end(args);
      }

      template< typename T >
      assert& add_variable( const char* name, const T& value )
      {
        std::cout << "  · variable " << name << " = " << value << "\n";
        return *this;
      }
    };


  }
}

#endif
