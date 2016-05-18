/*  Created on: May 2, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */
# ifndef GRAPHICS_ORIGIN_PARALLEL_H_
# define GRAPHICS_ORIGIN_PARALLEL_H_
# include "../graphics_origin.h"
# include <omp.h>
# ifdef GO_USE_OPENCL
#  include <CL/cl.hpp>

GO_API
void check_cl_error( const char* call, const char* file, const int line, cl_int error );

#  define clcheck( call ) check_cl_error( #call, __FILE__, __LINE__, call )
# endif

BEGIN_GO_NAMESPACE namespace tools {

  /**
   * We have three setups for parallelization:
   * - OpenCL (CPU/GPU)
   * - OpenMP (CPU only)
   * - none.
   *
   * Code for OpenMP can be reused when no parallelization is requested: we can
   * just set the max number of threads to 1. Since OpenMP is a feature of a
   * compiler, we can assume that the library is available. However, for OpenCL,
   * libraries are required to be installed. Thus, specific headers and functions
   * might not be available at compilation, this is way we have the following macro:
   * # ifdef GO_USE_OPENCL
   * //... do something with OpenCL
   * # endif
   *
   * When both OpenMP and OpenCL are available, we need a way to select which one
   * to use.
   *
   */

  /**@brief Available parallelization setups.
   *
   * Enumerate all the available parallelization setups.
   */
  typedef enum {
    none,
    openmp,
# ifdef GO_USE_OPENCL
    opencl
# endif
  } parallelization_setup;

  /**
   * Access to the current parallelization setup.
   * @return The current parallelization setup.
   */
  GO_API
  parallelization_setup get_parallelization_setup();

  /**
   * Set the current parallelization setup.
   * @param setup The new current setup.
   */
  GO_API
  void set_parallelization_setup( parallelization_setup setup );

# ifdef GO_USE_OPENCL
  GO_API
  const std::vector< cl::Platform >& get_cl_platforms();
# endif

} END_GO_NAMESPACE

# endif /* GRAPHICS_ORIGIN_PARALLEL_H_ */
