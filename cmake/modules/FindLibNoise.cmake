# FindLibNoise.cmake
# ==================
# Find the libnoise library.
#
# Filepath
# --------
#   LIBNOISE_DIR hint to find the LibNoise installation path, i.e. where to find
#     either noise/noise.h or include/noise/noise.h
# Variables defines
# -----------------
#   LIBNOISE_FOUND       true if LibNoise was found
#   LIBNOISE_INCLUDE_DIR include directory for LibNoise headers (to add to include_directories())
#   LIBNOISE_LIBRARIES   LibNoise libraries (to add to target_link_libraries())

find_path( LIBNOISE_INCLUDE_DIR noise/noise.h
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

if( LIBNOISE_INCLUDE_DIR AND LIBNOISE_LIBRARY AND LIBNOISEUTILS_LIBRARY )
  set( LIBNOISE_FOUND ON )
  set( LIBNOISE_LIBRARIES ${LIBNOISE_LIBRARY} ${LIBNOISEUTILS_LIBRARY} )
  message( STATUS "LibNoise found (headers in ${LIBNOISE_INCLUDE_DIR} and libraries are ${LIBNOISE_LIBRARIES})")
else()
  set( LIBNOISE_FOUND OFF )
  if( LibNoise_FIND_REQUIRED AND NOT LibNoise_FIND_QUIET )
  	message( FATAL "Cannot find libnoise library" )
  endif() 
endif()