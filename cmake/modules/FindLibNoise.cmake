# FindLibNoise.cmake
# ==================
# Find the libnoise library.
#
# Filepath
# --------
#   LIBNOISE_DIR hint to find the LibNoise installation path, i.e. where to find
#     either noise/noise.h or include/noise/noise.h
# Variables defined
# -----------------
#   LIBNOISE_FOUND        true if LibNoise was found
#   LIBNOISE_INCLUDE_DIRS include directory for LibNoise headers (to add to include_directories())
#   LIBNOISE_LIBRARIES    LibNoise libraries (to add to target_link_libraries())

find_path( LIBNOISE_INCLUDE_DIRS noise/noise.h
  PATHS
    /usr/local/include
    /usr/include
    /opt/local/include
    /opt/include
    $ENV{LIBNOISE_DIR}
    $ENV{LIBNOISE_DIR}/include
    ${LIBNOISE_DIR}
    ${LIBNOISE_DIR}/include
)

find_library( LIBNOISE_LIBRARY
  NAMES   
    noise  
  PATHS
    /usr/local
    /usr 
    /opt/local
    /opt
    $ENV{LIBNOISE_DIR}
    ${LIBNOISE_DIR}
  PATH_SUFFIXES
    lib64 lib 
)

find_library( LIBNOISEUTILS_LIBRARY
  NAMES   
    noiseutils  
  PATHS
    /usr/local
    /usr 
    /opt/local
    /opt
    $ENV{LIBNOISE_DIR}
    ${LIBNOISE_DIR}
  PATH_SUFFIXES
    lib64 lib 
)

if( LIBNOISE_INCLUDE_DIRS AND LIBNOISE_LIBRARY AND LIBNOISEUTILS_LIBRARY )
  set( LIBNOISE_FOUND TRUE )
  set( LIBNOISE_LIBRARIES ${LIBNOISE_LIBRARY} ${LIBNOISEUTILS_LIBRARY} )
  if( NOT LibNoise_FIND_QUIETLY )
    message( STATUS "LibNoise found (headers in: ${LIBNOISE_INCLUDE_DIRS} and libraries are ${LIBNOISE_LIBRARIES})")
  endif()
else()
  set( LIBNOISE_FOUND FALSE )
  if( LibNoise_FIND_REQUIRED )
  	message( SEND_ERROR "Cannot find libnoise library. Make sure it is installed and set LIBNOISE_DIR to give a hint about its location. "
  	"You can download it there: https://github.com/qknight/libnoise" )
  endif() 
endif()