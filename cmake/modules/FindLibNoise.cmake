# FindLibNoise.cmake
# ==================
# Find the libnoise library.
#
# Filepath
# --------
#   LIBNOISE_DIR hint to find the LibNoise installation path, i.e. where to find
#     either noise/noise.h or include/noise/noise.h
#   GO_EXTLIBS_DIR general location hint for Graphics Origin external libraries
# Variables defined
# -----------------
#   LIBNOISE_FOUND        true if LibNoise was found
#   LIBNOISE_INCLUDE_DIRS include directory for LibNoise headers (to add to include_directories())
#   LIBNOISE_LIBRARIES    LibNoise libraries (to add to target_link_libraries())

find_path( LIBNOISE_INCLUDE_DIRS noise/noise.h
  PATHS
    ${LIBNOISE_DIR}
    ${LIBNOISE_DIR}/include
	${GO_EXTLIBS_DIR}
	/usr/local/include
    /usr/include
    /opt/local/include
    /opt/include
    $ENV{LIBNOISE_DIR}
    $ENV{LIBNOISE_DIR}/include
)

if( LIBNOISE_INCLUDE_DIR )
  find_library( LIBNOISE_LIBRARY
    NAMES   
      noise  
    PATHS
	  ${LIBNOISE_INCLUDE_DIRS}/..
	  ${LIBNOISE_INCLUDE_DIRS}
      /usr/local
      /usr 
      /opt/local
      /opt
    PATH_SUFFIXES
      lib64 lib 
 )
 find_library( LIBNOISEUTILS_LIBRARY
    NAMES   
      noiseutils  
    PATHS
	  ${LIBNOISE_INCLUDE_DIRS}/..
	  ${LIBNOISE_INCLUDE_DIRS}
      /usr/local
      /usr 
      /opt/local
      /opt
    PATH_SUFFIXES
      lib64 lib 
 )
 
  if( LIBNOISE_LIBRARY AND LIBNOISEUTILS_LIBRARY )
    set( LIBNOISE_INCLUDE_DIRS ${LIBNOISE_INCLUDE_DIR} )
    set( LIBNOISE_LIBRARIES ${LIBNOISE_LIBRARY} ${LIBNOISEUTILS_LIBRARY} )
    set( LIBNOISE_FOUND TRUE )
  
    if( NOT LibNoise_FIND_QUIETLY )
   	    message( STATUS "Found Libnoise:" )
	    message( STATUS "  - include  : ${LIBNOISE_INCLUDE_DIRS}")
	    message( STATUS "  - libraries: ${LIBNOISE_LIBRARIES}")
    endif()
  else()
    set( LIBNOISE_FOUND FALSE )
    if( LibNoise_FIND_REQUIRED )
	  message( FATAL_ERROR "Cannot find library libnoise around ${LIBNOISE_INCLUDE_DIR}" )
	else()
	  message( SEND_ERROR "Cannot find library libnoise around ${LIBNOISE_INCLUDE_DIR}" )
	endif()
  endif()
else()
  if( LibNoise_FIND_REQUIRED )
    message( FATAL_ERROR "Cannot find libnoise. Make sure it is installed or set "
	  " LIBNOISE_DIR to give a hint about its location. You can download the library there: "
	  "https://github.com/qknight/libnoise" )
  elseif( NOT LibNoise_FIND_QUIETLY )
    message( SEND_ERROR "Cannot find libnoise. Make sure it is installed or set "
	  " LIBNOISE_DIR to give a hint about its location. You can download the library there: "
	  "https://github.com/qknight/libnoise" )
  endif() 
endif()

mark_as_advanced( LIBNOISE_LIBRARY LIBNOISEUTILS_LIBRARY LIBNOISE_INCLUDE_DIR )