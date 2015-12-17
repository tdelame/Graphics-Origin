/* Created on: 2015-12-17
 *     Author: T.Delame (tdelame@gmail.com)
 */
# ifndef GRAPHICS_ORIGIN_PROJECT_H_
# define PROJECT_PROJECT_H_
# include <float.h>
# define REAL_MIN DBL_MIN
# define REAL_MAX DBL_MAX
# if defined ( _WIN32 ) || defined( _WIN64 )
#   define PROJECT_API __declspec( dllexport )
# else
#   ifdef __APPLE__
#       define PROJECT_API
#   else
#        define PROJECT_API
#   endif
# endif
# define PROJECT_NAMESPACE graphics_origin
# define BEGIN_PROJECT_NAMESPACE namespace PROJECT_NAMESPACE {
# define END_PROJECT_NAMESPACE }
BEGIN_PROJECT_NAMESPACE
typedef double real;
typedef float gpu_real;
END_PROJECT_NAMESPACE
# endif
