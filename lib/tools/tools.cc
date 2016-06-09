/*  Created on: Dec 17, 2015
 *      Author: T. Delame (tdelame@gmail.com)
 */

# include <graphics-origin/graphics_origin.h>
# include <graphics-origin/tools/any.h>

# include <string>
# include <boost/locale.hpp>

# ifdef _WIN32
#   include <windows.h>
# endif

BEGIN_GO_NAMESPACE namespace tools {

  void startup_init_log();
  void startup_init_parallel_setup();

  std::unordered_map < std::type_index, std::string >& get_type_to_name()
  {
    static std::unordered_map < std::type_index, std::string > type_to_name;
    return type_to_name;
  }

  std::unordered_map < std::string, detail::serialize_any_base* >& get_name_to_serializer()
  {
    static std::unordered_map < std::string, detail::serialize_any_base* > name_to_serializer;
    return name_to_serializer;
  }

# ifdef __linux__ 
  static void initialize_library() __attribute__((constructor));
  static void finalize_library() __attribute__((destructor));
# endif

  void initialize_library()
  {
	  static bool initialized = false;
	  if (initialized) return;

	  REGISTER_ANY_SERIALIZATION_TYPE(int, "int");
	  REGISTER_ANY_SERIALIZATION_TYPE(bool, "bool");
	  REGISTER_ANY_SERIALIZATION_TYPE(std::string, "string");
	  REGISTER_ANY_SERIALIZATION_TYPE(float, "float");
	  REGISTER_ANY_SERIALIZATION_TYPE(double, "double");
	  REGISTER_ANY_SERIALIZATION_TYPE(real, "real");

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
	  for (auto pair : get_name_to_serializer())
	  {
		  delete pair.second;
	  }
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
