/* Created on: 2015-12-17
 *     Author: T.Delame (tdelame@gmail.com)
 */
# ifndef GRAPHICS_ORIGIN_PROJECT_H_
# define GRAPHICS_ORIGIN_PROJECT_H_

# define GRAPHICS_ORIGIN_MAJOR @Graphics-Origin_VERSION_MAJOR@
# define GRAPHICS_ORIGIN_MINOR @Graphics-Origin_VERSION_MINOR@

# include <float.h>
# define REAL_MIN DBL_MIN
# define REAL_MAX DBL_MAX




# define GO_NAMESPACE graphics_origin
# define BEGIN_GO_NAMESPACE namespace GO_NAMESPACE {
# define END_GO_NAMESPACE }
BEGIN_GO_NAMESPACE
typedef double real;
typedef float gpu_real;
END_GO_NAMESPACE
# endif
