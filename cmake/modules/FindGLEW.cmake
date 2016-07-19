# FindGLEW.cmake
# ==============
# Find the GLEW library 
#
# Filepath
# --------
# GLEW_DIR hint to find the installation path of the GLEW library
# GO_EXTLIBS_DIR general location hint for Graphics Origin external libraries
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
	${GO_EXTLIBS_DIR}
    /usr/local/include
    /usr/include
    /opt/include
)

if( GLEW_INCLUDE_DIR )
  find_library( GLEW_LIBRARY NAMES GLEW glew glew32
    PATHS
      ${GLEW_INCLUDE_DIR}/../lib 
	  ${GLEW_INCLUDE_DIR}
	  /usr/local
	  /usr
	  /opt
    PATH_SUFFIXES
      lib64 lib
  )
  
  if( GLEW_LIBRARY )
    set( GLEW_INCLUDE_DIRS ${GLEW_INCLUDE_DIR} )
    set( GLEW_LIBRARIES ${GLEW_LIBRARY} )
    set( GLEW_FOUND TRUE )
    
    if( NOT GLEW_FIND_QUIETLY )
	  message( STATUS "Found GLEW:" )
	  message( STATUS "  - include  : ${GLEW_INCLUDE_DIRS}")
	  message( STATUS "  - libraries: ${GLEW_LIBRARIES}")
    endif() 
  else()
    set( GLEW_FOUND FALSE )
	if( GLEW_FIND_REQUIRED )
	  message( FATAL_ERROR "Cannot find library GLEW around ${GLEW_INCLUDE_DIR}")
	else()
	  message( SEND_ERROR "Cannot find library GLEW around ${GLEW_INCLUDE_DIR}")
	endif()
  endif()

else()
  if( GLEW_FIND_REQUIRED )
	message( FATAL_ERROR "Cannot find GLEW. Make sure it is installed or set"
	  " GLEW_DIR to give a hint about its location")
  elseif( NOT GLEW_FIND_QUIETLY )
	message( SEND_ERROR "Cannot find GLEW. Make sure it is installed or set"
	  " GLEW_DIR to give a hint about its location")
  endif()
endif()

mark_as_advanced( GLEW_INCLUDE_DIR GLEW_LIBRARY )
