#**********************
# cmake configuration *
#*******************************************************************************
cmake_minimum_required( VERSION 3.0 )

# Graphics-Origin project
project( Graphics-Origin )
set( Graphics-Origin_VERSION_MAJOR 1 )
set( Graphics-Origin_VERSION_MINOR 0 )

if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE "Release" 
    CACHE STRING "Choose the type of build, options are: Debug Release." FORCE)
endif(NOT CMAKE_BUILD_TYPE)

#****************************
# directories configuration *
#*******************************************************************************
set( GO_INCLUDE_IN_DIR "${CMAKE_SOURCE_DIR}" )
set( GO_LIBRARY_IN_DIR "${CMAKE_SOURCE_DIR}/lib" )
set( GO_TUTORIAL_IN_DIR "${CMAKE_SOURCE_DIR}/tutorial" )
set( GO_TEST_IN_DIR "${CMAKE_SOURCE_DIR}/test" )
set( GO_EXTLIBS_DIR "${CMAKE_SOURCE_DIR}/extlibs" )

set( GO_EXECUTABLE_OUT_DIR "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin" )
set( GO_LIBRARY_OUT_DIR "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib" )
set( GO_TUTORIAL_OUT_DIR "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/tutorial/bin" )
set( GO_TEST_OUT_DIR "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/test/bin" )

set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${GO_LIBRARY_OUT_DIR} )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${GO_LIBRARY_OUT_DIR} )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${GO_EXECUTABLE_OUT_DIR} )


#*********************
# projection options *
#*******************************************************************************
option(
  GO_USE_CGAL
  "Use CGAL libraries"
  ON)
  
option(
  GO_USE_QT
  "Set to ON to use QT libraries. If set to false, the application library cannot be built"
  ON)  
  
option(
  GO_USE_CUDA
  "Set to ON to use CUDA libraries. If set to false, some functionnalities won't be available"
  ON)  
  
option(
  GO_BUILD_APPLICATION
  "Set to ON to build the application library. Since this library depends on the QT framework,
  it will be built if both BUILD_APPLICATION and USE_QT are set to ON"
  ON)

if( NOT GO_USE_QT )
  set( GO_BUILD_APPLICATION OFF)
else()
  if( NOT GO_BUILD_APPLICATION )
    set( GO_USE_QT OFF )
  endif()
endif()

#***************
# global flags *
#*******************************************************************************
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frounding-math -std=c++14 -fPIC -fopenmp")
set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -03 -msse2" )
set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -00 -ggdb -Wall -Wextra" )

set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lgomp")

  
#*********************
# external libraries *
#*******************************************************************************  
# openmesh

# boost

# qt

# cuda

# opengl glew


set( GO_GLM_INCLUDE_DIR
  "${GO_EXTLIBS_DIR}" ) 
  
if( GO_USE_CUDA )
  find_package( CUDA )
  if( NOT CUDA_FOUND )
    set( GO_USE_CUDA OFF )
  else()
    set( GO_CUDA_LIBRARIES "${CUDA_CUDA_LIBRARY} ${CUDA_CUDART_LIBRARY}" )
    set( GO_CUDA_INCLUDES "${CUDA_TOOLKIT_INCLUDE}" )
  endif()
endif() 

find_package( OPENME
 



#configure_file( "${PROJECT_SOURCE_DIR}/graphics-origin.h.in" "${PROJECT_BINARY_DIR}/graphics-origin.h")
#install ( TARGETS geometry  DESTINATION bin)
#install (  FILES truc.h   DESTINATION include)
#include_directories( "${PROJECT_BINARY_DIR}" )
#add_subdirectory()
#add_library( geometry .cxx)
#add_library( tools  ...)