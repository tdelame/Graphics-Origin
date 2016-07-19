# ifndef GRAPHICS_ORIGIN_ASSERT_H_
# define GRAPHICS_ORIGIN_ASSERT_H_
# include "../graphics_origin.h"
# include <iostream>
# include <stdio.h>
# include <cstdarg>
namespace graphics_origin {
  namespace tools {

    /**@brief Custom assertions.
     *
     * This comes from a very nice idea of Stefan Reinalter (
     * https://blog.molecular-matters.com/2011/07/22/an-improved-assert/). Let us
     * suppose you want to assert that a is smaller than b, where a and b are types
     * for which the operator<<(std::ostream) is defined. You can then use the
     * following macro:
     * \code{.cpp}
     * GO_ASSERT( a < b, "a is not smaller than b")(a,b)
     * \endcode
     *
     * When the assertion fails, you will see a message with the code location of
     * the assertion, the message "a is not smaller than b", and two lines with
     * the values of variables a and b. If the program is attached to a debugger,
     * you will be able to continue the execution. Otherwise, the program will
     * stop immediately.
     */
    struct Assert{
      Assert( const char* filename, int line_number, const char* message )
      {
        std::cerr << filename << " (" << line_number << ") [ASSERT] " << message << std::endl;
      }

      template< typename T >
      Assert& add_variable( const char* name, const T& value )
      {
        std::cout << "  · variable " << name << " = " << value << "\n";
        return *this;
      }
    };

# define GO_ASSERT_IMPL_VAR(variable) .add_variable(GO_STRINGIZE(variable), variable)
# define GO_ASSERT_EXPAND_ASSERTION( condition, format ) graphics_origin::tools::Assert(__FILE__,__LINE__,"Assertion \"" #condition "\" failed: " format )
# define GO_ASSERT_EXPAND_VARIABLES(...) \
	GO_PP_JOIN(GO_PP_EXPAND_ARGS_,GO_PP_VA_NUM_ARGS(__VA_ARGS__))(GO_ASSERT_IMPL_VAR,__VA_ARGS__), GO_BREAK )

# define GO_ASSERT( condition, format ) \
  (condition) ? (void)true : ((void)GO_ASSERT_EXPAND_ASSERTION(condition,format) GO_ASSERT_EXPAND_VARIABLES
  }
}

#endif
