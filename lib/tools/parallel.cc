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

      if( g_parallelization_setup == parallelization_setup::none )
        {
          omp_set_num_threads( 1 );
        }
      else if( g_parallelization_setup == parallelization_setup::openmp )
        {
          omp_set_num_threads( omp_get_num_procs() );
        }
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


  static constexpr unsigned int cl_number_of_platform_attributes = 5;

  static const cl_platform_info cl_platform_attribute_types[ cl_number_of_platform_attributes ] = {
      CL_PLATFORM_NAME,
      CL_PLATFORM_VENDOR,
      CL_PLATFORM_VERSION,
      CL_PLATFORM_PROFILE,
      CL_PLATFORM_EXTENSIONS
  };
  static const char* const cl_platform_attribute_names[ cl_number_of_platform_attributes ] = {
      "Name      : ",
      "Vendor    : ",
      "Version   : ",
      "Profile   : ",
      "Extensions: "
  };
  //end

  std::vector< cl::Platform >& g_cl_platforms()
  {
    static std::vector< cl::Platform > platforms;
    return platforms;
  }

  std::vector< cl::Device >& g_cl_cpu_devices()
  {
    static std::vector< cl::Device > cpus;
    return cpus;
  }

  std::vector< cl::Device >& g_cl_gpu_devices()
  {
    static std::vector< cl::Device > gpus;
    return gpus;
  }

  const std::vector< cl::Platform >& get_cl_platforms()
  {
    return g_cl_platforms();
  }

  const std::vector< cl::Device >& get_cl_cpu_devices()
  {
    return g_cl_cpu_devices();
  }

  const std::vector< cl::Device >& get_cl_gpu_devices()
  {
    return g_cl_gpu_devices();
  }



  static std::string get_cl_device_type_string( cl_device_type type )
  {
    std::string result = "unknown";
    if( type & CL_DEVICE_TYPE_CPU )
      {
        result = "CPU";
      }
    else if( type & CL_DEVICE_TYPE_GPU )
      {
        result = "GPU";
      }
    else if( type & CL_DEVICE_TYPE_ACCELERATOR )
      {
        result = "accelerator";
      }

    if( type & CL_DEVICE_TYPE_DEFAULT )
      result += " [default]";
  }

# endif

  void initialize_parallel_setup()
  {
    LOG( info, "[OpenMP] number of devices: " << omp_get_num_devices() );
    LOG( info, "[OpenMP] default device: " << omp_get_default_device() );
    LOG( info, "[OpenMP] maximum threads to use on default device: " << omp_get_num_procs() )
# ifdef GO_USE_OPENCL
    //fixme: why in debug mode, this code crashes?
    {
      std::vector< cl::Platform >& platforms = g_cl_platforms();
      clcheck( cl::Platform::get( &platforms ));
      LOG( info, "[OpenCL] number of platforms: " << platforms.size() );

      for( unsigned int i = 0; i < platforms.size();  )
        {
          LOG( info, "[OpenCL] Platform #" << i );

          std::string info;
          for( unsigned int platform_attribute_number = 0;
              platform_attribute_number < cl_number_of_platform_attributes;
              ++platform_attribute_number )
            {
              clcheck(platforms[i].getInfo( cl_platform_attribute_types[ platform_attribute_number ], &info ));
              LOG( info, "  + " << cl_platform_attribute_names[ platform_attribute_number ] << info );
            }

          std::vector< cl::Device > devices;
          platforms[i].getDevices( CL_DEVICE_TYPE_ALL , &devices);

          for( unsigned int device_number = 0; device_number < devices.size(); ++ device_number )
            {
              LOG( info, "  + Device #" << device_number );

              clcheck(devices[device_number].getInfo( CL_DEVICE_NAME, &info ));
              LOG( info, "    - Name: " << info );

              clcheck(devices[device_number].getInfo( CL_DEVICE_VENDOR, &info ));
              LOG( info, "    - Vendor: " << info );

              clcheck(devices[device_number].getInfo( CL_DRIVER_VERSION, &info ));
              LOG( info, "    - Driver: " << info );

              clcheck(devices[device_number].getInfo( CL_DEVICE_VERSION, &info ));
              LOG( info, "    - Version: " << info );

              cl_device_type type;
              clcheck(devices[device_number].getInfo( CL_DEVICE_TYPE, &type ));
              LOG( info, "    - Type: " << get_cl_device_type_string( type ) );
              if( type & CL_DEVICE_TYPE_GPU )
                g_cl_gpu_devices().push_back( devices[device_number] );
              else if( type & CL_DEVICE_TYPE_CPU )
                g_cl_cpu_devices().push_back( devices[device_number] );

              cl_uint max_compute_units = 0;
              clcheck(devices[device_number].getInfo( CL_DEVICE_MAX_COMPUTE_UNITS, &max_compute_units ));
              LOG( info, "    - Max Compute Units: " << max_compute_units );

              size_t max_group_size = 0;
              clcheck(devices[device_number].getInfo( CL_DEVICE_MAX_WORK_GROUP_SIZE, &max_group_size ));
              LOG( info, "    - Max Work Group Size: " << max_group_size );

              cl_uint dimension = 0;
              clcheck(devices[device_number].getInfo( CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, &dimension ));
              LOG( info, "    - Max Work Item Dim: " << dimension );

              std::vector< size_t > max_work_item_sizes;
              clcheck(devices[device_number].getInfo( CL_DEVICE_MAX_WORK_ITEM_SIZES, &max_work_item_sizes ));

              info = "(";
              for( unsigned int j = 0; j + 1 < max_work_item_sizes.size(); ++ j )
                {
                  info += std::to_string( max_work_item_sizes[j] ) + ", ";
                }
              LOG( info, "    - Max Work Item Sizes: " << info << max_work_item_sizes.back() << ')');
            }
          if( devices.empty() )
            {
              LOG( info, "  + Failed to get devices for this platform. Removing it from the list...");
              std::swap( platforms[i], platforms.back() );
              platforms.pop_back();
            }
          else ++ i;
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
