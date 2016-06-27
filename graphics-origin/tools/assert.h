# ifndef GRAPHICS_ORIGIN_ASSERT_H_
# define GRAPHICS_ORIGIN_ASSERT_H_
# include "../graphics_origin.h"
# include <iostream>
# include <stdio.h>
# include <cstdarg>
namespace graphics_origin {
  namespace tools {

    /**
     * A very nice idea from Stefan Reinalter
     * https://blog.molecular-matters.com/2011/07/22/an-improved-assert/
     */
    struct Assert{
      // maybe I should use std::ostream here
      Assert( const char* filename, int line_number, const char* format, ... )
      {
        printf( "%s (%i): [ASSERT] ", filename, line_number );
        va_list args;
        va_start( args, format );
        vprintf( format, args );
        printf("\n");
        va_end(args);
      }

      template< typename T >
      Assert& add_variable( const char* name, const T& value )
      {
        std::cout << "  · variable " << name << " = " << value << "\n";
        return *this;
      }
    };


//
//# define GO_ASSERT_IMPL_VAR(variable) .add_variable(GO_STRINGIZE(variable), variable)
//# define GO_PP_EXPAND_ARGS_1(op, a1)                 op(a1)
//# define GO_PP_EXPAND_ARGS_2(op, a1, a2)             op(a1) op(a2)
//# define GO_PP_EXPAND_ARGS_3(op, a1, a2, a3)         op(a1) op(a2) op(a3)
//# define GO_PP_EXPAND_ARGS_4(op, a1, a2, a3, a4)     op(a1) op(a2) op(a3) op(a4)
//# define GO_PP_EXPAND_ARGS_5(op, a1, a2, a3, a4, a5) op(a1) op(a2) op(a3) op(a4) op(a5)
//# define GO_PP_EXPAND_ARGS(op, ...)  GO_PP_JOIN( GO_PP_EXPAND_ARGS_, GO_PP_NUM_ARGS(__VA_ARGS__)) GO_PP_PASS_ARGS(op, __VA_ARGS__)
//# define GO_ASSERT_IMPL_VARS(...) GO_PP_EXPAND_ARGS GO_PP_PASS_ARGS(GO_ASSERT_IMPL_VAR, __VA_ARGS__), GO_BREAK)
//
//# define go_assert( condition, format ) \
//  (condition) ? \
//      (int)1 \
//    : (graphics_origin::tools::Assert(__FILE__,__LINE__, "Assertion \"" #condition "\" failed: " format) GO_ASSERT_IMPL_VARS
//
  }
}

#endif
