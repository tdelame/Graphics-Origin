/*  Created on: May 2, 2016
 *      Author: T. Delame (tdelame@gmail.com)
 */
# ifndef GRAPHICS_ORIGIN_PARALLEL_H_
# define GRAPHICS_ORIGIN_PARALLEL_H_
# include "../graphics_origin.h"
# ifdef GO_USE_OPENMP
#  include <omp.h>
# endif
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
   * Originally, it was CUDA only, but it was not general enough.
   *
   * Code for OpenMP can be reused when no parallelization is requested:
   * - if the system does not have OpenMP, pragmas are ignored
   * - if the system does have OpenMP, we can set the max number of threads
   * to 1.
   *
   * Still, some code might require including OpenMP header or use specific
   * functions. Thus, we need to be careful and use:
   * # ifdef GO_USE_OPENMP
   * //... do something
   * # endif
   *
   * A similar macro is available for OpenCL:
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
# ifdef GO_USE_OPENMP
    openmp,
# endif
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
