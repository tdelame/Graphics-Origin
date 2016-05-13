/*  Created on: May 7, 2016
 *      Author: T. Delame (tdelame@gmail.com) */
# include "../../graphics-origin/tools/parallel.h"
# include "../../graphics-origin/tools/log.h"
# include <mutex>
# include <vector>

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

# ifdef GO_USE_OPENCL
  void check_cl_error( const char* call, const char* file, const int line, cl_int error )
  {
    if( error != CL_SUCCESS )
      {
        LOG_WITH_LINE_FILE( error, "[OpenCL] Error discovered when calling "
          << call << ": " << error, line, file );
      }
  }

  // device_query.c
  // yohanes.gultom@gmail.com
  // Original source:
  // * http://stackoverflow.com/questions/17240071/what-is-the-right-way-to-call-clgetplatforminfo
  // * Banger, R, Bhattacharyya .K. "OpenCL Programming by Example". 2013. Packt publishing​. p43
#   define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

  static const cl_platform_info cl_attribute_types[5] = {
      CL_PLATFORM_NAME,
      CL_PLATFORM_VENDOR,
      CL_PLATFORM_VERSION,
      CL_PLATFORM_PROFILE,
      CL_PLATFORM_EXTENSIONS
  };

  static const char* const attributeNames[] = {
      "CL_PLATFORM_NAME",
      "CL_PLATFORM_VENDOR",
      "CL_PLATFORM_VERSION",
      "CL_PLATFORM_PROFILE",
      "CL_PLATFORM_EXTENSIONS"
  };

  static void log_cl_device_info(cl_device_id device)
  {
      char queryBuffer[1024];
      int queryInt;
      clcheck( clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(queryBuffer), &queryBuffer, NULL) );
      LOG( info, "   + CL_DEVICE_NAME: " << queryBuffer);
      queryBuffer[0] = '\0';

      clcheck(clGetDeviceInfo(device, CL_DEVICE_VENDOR, sizeof(queryBuffer), &queryBuffer, NULL));
      LOG( info, "   + CL_DEVICE_VENDOR: " << queryBuffer);
      queryBuffer[0] = '\0';

      clcheck(clGetDeviceInfo(device, CL_DRIVER_VERSION, sizeof(queryBuffer), &queryBuffer, NULL));
      LOG( info, "   + CL_DRIVER_VERSION: " << queryBuffer);
      queryBuffer[0] = '\0';

      clcheck(clGetDeviceInfo(device, CL_DEVICE_VERSION, sizeof(queryBuffer), &queryBuffer, NULL));
      LOG( info, "   + CL_DEVICE_VERSION: " << queryBuffer);
      queryBuffer[0] = '\0';

      clcheck(clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(int), &queryInt, NULL));
      LOG( info, "   + CL_DEVICE_MAX_COMPUTE_UNITS: " << queryInt);
  }
  //end

# endif

  void initialize_parallel_setup()
  {
# ifdef GO_USE_OPENMP
    LOG( info, "[OpenMP] number of devices: " << omp_get_num_devices() );
    LOG( info, "[OpenMP] default device: " << omp_get_default_device() );
    LOG( info, "[OpenMP] maximum threads to use on default device: " << omp_get_num_procs() )
# endif
# ifdef GO_USE_OPENCL
    {
      cl_uint nb_platforms = 0;
      clcheck(  clGetPlatformIDs( 0, 0, &nb_platforms ) );
      LOG( info, "[OpenCL] number of platforms: " <<  nb_platforms);

      std::vector<cl_platform_id> platform_ids( nb_platforms );
      clcheck( clGetPlatformIDs( nb_platforms, platform_ids.data(), &nb_platforms));

      // for each platform print all attributes
      int num_attributes = NELEMS(cl_attribute_types);
      for( cl_uint i = 0; i < nb_platforms; ++i)
        {
          LOG( info, "[OpenCL] Platform #" << i + 1 );
          for( int j = 0; j < num_attributes; ++j)
            {
              // get platform attribute value size
              size_t info_size = 0;
              clGetPlatformInfo(platform_ids[i], cl_attribute_types[j], 0, NULL, &info_size);
              char* info = (char*) malloc(info_size);
              // get platform attribute value
              clGetPlatformInfo(platform_ids[i], cl_attribute_types[j], info_size, info, NULL);
//              printf("  %d.%d %-11s: %s\n", i+1, j+1, attributeNames[j], info);
              free( info );
            }
          //Get the devices list and choose the device you want to run on
          cl_uint num_devices = 0;
          clcheck(clGetDeviceIDs( platform_ids[i], CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices));

          cl_device_id* device_list = device_list = (cl_device_id *) malloc(sizeof(cl_device_id)*num_devices);
          clcheck(clGetDeviceIDs( platform_ids[i], CL_DEVICE_TYPE_ALL, num_devices, device_list, NULL));
          for( cl_uint k = 0; k < num_devices; ++k )
            {
              log_cl_device_info( device_list[k] );
            }
          free(device_list);
      }
    }
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
