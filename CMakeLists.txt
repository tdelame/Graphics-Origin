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
set( GO_HEADERS_IN_DIR "${CMAKE_SOURCE_DIR}" )
set( GO_SOURCES_IN_DIR "${CMAKE_SOURCE_DIR}/lib" )
set( GO_TUTORIAL_IN_DIR "${CMAKE_SOURCE_DIR}/tutorial" )
set( GO_TEST_IN_DIR "${CMAKE_SOURCE_DIR}/test" )
set( GO_EXTLIBS_DIR "${CMAKE_SOURCE_DIR}/extlibs" )

set( GO_EXECUTABLE_OUT_DIR "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin" )
set( GO_LIBRARY_OUT_DIR "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/lib" )
set( GO_TUTORIAL_OUT_DIR "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/tutorial" )
set( GO_TEST_OUT_DIR "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/test" )

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

option(
  GO_BUILD_TOOLS_TUTORIAL
  "Set to ON to build the tutorial for the tools library."
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
set( CMAKE_CXX_FLAGS 
  "${CMAKE_CXX_FLAGS} -frounding-math -std=c++14 -fPIC -fopenmp -DGLM_FORCE_SSE2 -DGLM_FORCE_RADIANS -DBOOST_LOG_DYN_LINK -DBOOST_ALL_DYN_LINK"
  CACHE STRING "C++ compiler flags" FORCE)
set( CMAKE_CXX_FLAGS_RELEASE 
  "${CMAKE_CXX_FLAGS_RELEASE} -O3 -msse2 -UDEBUG -DNDEBUG -DCGAL_NDEBUG -DNO_DEBUG -DEIGEN_NO_DEBUG" 
  CACHE STRING "C++ compiler extra flags for release build type" FORCE)
set( CMAKE_CXX_FLAGS_DEBUG 
  "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb -Wall -Wextra -DDEBUG" 
  CACHE STRING "C++ compiler extra flags for debug build type" FORCE )

set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lgomp")
include_directories( "${GO_HEADERS_IN_DIR}" )
  
#*********************
# external libraries *
#*******************************************************************************  
# openmesh

# boost
set( Boost_USE_MULTITHREADED ON )
set( Boost_USE_STATIC_LIBS OFF )
set( Boost_USE_STATIC_RUNTIME OFF )
find_package( Boost
  REQUIRED
  COMPONENTS serialization filesystem log log_setup locale random system )
include_directories( ${Boost_INCLUDE_DIRS} )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" )

# qt

# cuda
if( GO_USE_CUDA )
  find_package( CUDA )
  if( NOT CUDA_FOUND )
    set( GO_USE_CUDA OFF )
  else()
    include_directories( ${CUDA_TOOLKIT_INCLUDE} )
    set( CUDA_LIBRARIES 
      "${CUDA_CUDA_LIBRARY} ${CUDA_CUDART_LIBRARY}"
      CACHE STRING "Libraries to link with if you want to use CUDA")
  endif()
endif() 

# opengl glew


set( GLM_INCLUDE_DIRS "${GO_EXTLIBS_DIR}" ) 
  


add_subdirectory( "${GO_SOURCES_IN_DIR}/tools" )
add_subdirectory( "${GO_SOURCES_IN_DIR}/geometry" )

if( GO_BUILD_TOOLS_TUTORIAL )
  add_subdirectory( "${GO_TUTORIAL_IN_DIR}/1_tools" )
endif()

#configure_file( "${PROJECT_SOURCE_DIR}/graphics-origin.h.in" "${PROJECT_BINARY_DIR}/graphics-origin.h")