# include "../../graphics-origin/graphics_origin.h"
# include <string>
# include <boost/locale.hpp>

# ifdef _WIN32
#   include <windows.h>
# endif

BEGIN_GO_NAMESPACE namespace tools {

  void startup_init_log();
  void startup_init_parallel_setup();

# ifdef __linux__ 
  static void initialize_library() __attribute__((constructor));
  static void finalize_library() __attribute__((destructor));
# endif

  void initialize_library()
  {
	  static bool initialized = false;
	  if (initialized) return;

	  boost::locale::generator gen;
	  std::locale loc = gen("");
	  std::locale::global(loc);
	  std::wcout.imbue(loc);
	  std::ios_base::sync_with_stdio(false);

	  startup_init_log();
	  startup_init_parallel_setup();


	  initialized = true;
  }



  void finalize_library()
  {
  }


# ifdef _WIN32
  BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
  {
    if (fdwReason == DLL_PROCESS_ATTACH)
	    {
	     initialize_library();
	     return TRUE;
	    }
	  else if (fdwReason == DLL_PROCESS_DETACH)
	    {
	      finalize_library();
	    }
	  return TRUE;
  }
# endif


} END_GO_NAMESPACE
