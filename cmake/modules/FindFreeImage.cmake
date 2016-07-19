# FindFreeImage.cmake
# ===================
# Find the Free Image library
#
# Filepath
# --------
# FREEIMAGE_DIR hint to find the installation path of the GLEW library, i.e. where
#   to find include/FreeImage.h or FreeImage.h
# GO_EXTLIBS_DIR general location hint for Graphics Origin external libraries
#
# Variables defined
# -----------------
# FREEIMAGE_INCLUDE_DIRS include directories (to add to include_directories())
# FREEIMAGE_LIBRARIES    libraries to link against Free Image (to add to target_link_libraries())
# FREEIMAGE_FOUND        true if Free Image has been found and can be used
find_path( FREEIMAGE_INCLUDE_DIR FreeImage.h
  PATHS
    ${FREEIMAGE_DIR}
	${FREEIMAGE_DIR}/include
	${GO_EXTLIBS_DIR}
	/usr/local/include
	/usr/include
	/opt/include
	/opt/freeimage/include
)

if( FREEIMAGE_INCLUDE_DIR )

  find_library( FREEIMAGE_LIBRARY
	NAMES
	  freeimage FreeImage
	PATHS
	  ${FREEIMAGE_INCLUDE_DIR}/../lib
	  ${FREEIMAGE_INCLUDE_DIR}
      /usr/local
	  /usr
	  /opt
    PATH_SUFFIXES
      lib64 lib
  )
  
  if( FREEIMAGE_LIBRARY )
	set( FREEIMAGE_INCLUDE_DIRS ${FREEIMAGE_INCLUDE_DIRS} )
	set( FREEIMAGE_LIBRARIES ${FREEIMAGE_LIBRARY} )
	set( FREEIMAGE_FOUND TRUE )
	
	if( NOT FreeImage_FIND_QUIETLY )
	  message( STATUS "Found FreeImage:" )
	  message( STATUS "  - include  : ${FREEIMAGE_INCLUDE_DIRS}")
	  message( STATUS "  - libraries: ${FREEIMAGE_LIBRARIES}")
	endif()
  
  else()
	set( FREEIMAGE_FOUND FALSE )
	if( FreeImage_FIND_REQUIRED )
	  message( FATAL_ERROR "Cannot find library Free Image around ${FREEIMAGE_INCLUDE_DIR}")
	else()
	  message( SEND_ERROR "Cannot find library Free Image around ${FREEIMAGE_INCLUDE_DIR}")
	endif()
  endif()
	  

else()
  if( FreImage_FIND_REQUIRED )
	message( FATAL_ERROR "Cannot find Free Image. Make sure it is installed or set"
	  " FREEIMAGE_DIR to give a hint about its location")
  elseif( NOT FreeImage_FIND_QUIETLY )
	message( SEND_ERROR "Cannot find Free Image. Make sure it is installed or set"
	  " FREEIMAGE_DIR to give a hint about its location")
  endif()
endif()

mark_as_advanced( FREEIMAGE_INCLUDE_DIR FREEIMAGE_LIBRARY )
