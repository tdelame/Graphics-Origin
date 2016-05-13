/*  Created on: May 7, 2016
 *      Author: T. Delame (tdelame@gmail.com) */
# include "../../graphics-origin/tools/parallel.h"
# include "../../graphics-origin/tools/log.h"
# include <mutex>

BEGIN_GO_NAMESPACE namespace tools {

  static parallelization_setup g_parallelization_setup = parallelization_setup::none;
  static std::mutex parallelization_setup_mutex;

  parallelization_setup get_parallelization_setup()
  {
    return g_parallelization_setup;
  }

  void set_parallelization_setup( parallelization_setup setup )
  {
    parallelization_setup_mutex.lock();
      g_parallelization_setup = setup;

# ifdef GO_USE_OPENMP
      if( g_parallelization_setup == parallelization_setup::none )
        {
          omp_set_num_threads( 1 );
        }
      else if( g_parallelization_setup == parallelization_setup::openmp )
        {
          omp_set_num_threads( omp_get_num_procs() );
        }
# endif


    parallelization_setup_mutex.unlock();
  }


# ifdef __linux__
  static void initialize_parallel_setup() __attribute__((constructor));
# endif

  void initialize_parallel_setup()
  {
# ifdef GO_USE_OPENMP
    LOG( info, "OpenMP number of devices: " << omp_get_num_devices() );
    LOG( info, "OpenMP default device: " << omp_get_default_device() );
    LOG( info, "maximum OpenMP threads to use on default device: " << omp_get_num_procs() )
# endif
  }

# ifdef _WIN32
  BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
    {
      if (fdwReason == DLL_PROCESS_ATTACH)
      {
        // equivalent of __attribute__((constructor))...
        initialize_parallel_setup();
        // return TRUE if succeeded, FALSE if you failed to initialize properly
        return TRUE;
      }
      else if (fdwReason == DLL_PROCESS_DETACH)
        {
        // equivalent of __attribute__((destructor))...
        }
      return TRUE;
    }
# endif

} END_GO_NAMESPACE
