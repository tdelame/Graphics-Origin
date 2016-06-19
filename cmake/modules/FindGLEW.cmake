# FindGLEW.cmake
# ==============
# Find the GLEW library 
#
# Filepath
# --------
# GLEW_DIR hint to find the installation path of the GLEW library
#
# Variables defined
# -----------------
# GLEW_INCLUDE_DIRS include directories for GLEW (to add to include_directories())
# GLEW_LIBRARIES    libraries to link against GLEW (to add to target_link_libraries())
# GLEW_FOUND        true if GLEW has been found and can be used
find_path( GLEW_INCLUDE_DIR GL/glew.h
  PATHS 
    ${GLEW_DIR}
    ${GLEW_DIR}/include
    /usr/local/include
    /usr/include
    /opt/include
)

if( GLEW_INCLUDE_DIR )
  find_library( GLEW_LIBRARY NAMES GLEW glew 
  PATHS
    ${GLEW_INCLUDE_DIR}/../lib 
  )
  
  if( GLEW_LIBRARY )
    set( GLEW_INCLUDE_DIRS ${GLEW_INCLUDE_DIR} )
    set( GLEW_LIBRARIES ${GLEW_LIBRARY} )
    set( GLEW_FOUND TRUE )
    
    if( NOT GLEW_FIND_QUIETLY )
      message( STATUS "Found GLEW: ${GLEW_LIBRARIES}" )
    endif() 
  else()
    message( SEND_ERROR "Cannot find GLEW libraries..." )
  endif()

else()
  message( SEND_ERROR "Cannot find GLEW. Make sure it is installed and set GLEW_DIR variable to give a hint about its location")
endif()

mark_as_advanced( GLEW_INCLUDE_DIR GLEW_LIBRARY )
