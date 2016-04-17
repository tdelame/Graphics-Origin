#**********************
# cmake configuration *
#*******************************************************************************
cmake_minimum_required( VERSION 3.1.0 )

set( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )

# Graphics-Origin project
project( Graphics-Origin )
set( Graphics-Origin_VERSION_MAJOR 1 )
set( Graphics-Origin_VERSION_MINOR 0 )

if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE "Release" 
    CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo." FORCE)
endif(NOT CMAKE_BUILD_TYPE)

#****************************
# directories configuration *
#*******************************************************************************
set( GO_HEADERS_IN_DIR "${CMAKE_SOURCE_DIR}" )
set( GO_SOURCES_IN_DIR "${CMAKE_SOURCE_DIR}/lib" )
set( GO_TUTORIAL_IN_DIR "${CMAKE_SOURCE_DIR}/tutorial" )
set( GO_TEST_IN_DIR "${CMAKE_SOURCE_DIR}/test" )
set( GO_EXTLIBS_DIR "${CMAKE_SOURCE_DIR}/extlibs" )
set( GO_OUT_DIR "${CMAKE_BINARY_DIR}")

set( GO_LIBRARY_OUT_DIR_DEBUG "${GO_OUT_DIR}/Debug/lib" )
set( GO_APPLICATION_OUT_DIR_DEBUG "${GO_OUT_DIR}/Debug/bin" )
set( GO_TUTORIAL_OUT_DIR_DEBUG "${GO_OUT_DIR}/Debug/tutorial" )
set( GO_TEST_OUT_DIR_DEBUG "${GO_OUT_DIR}/Debug/test" )

set( GO_LIBRARY_OUT_DIR_RELEASE "${GO_OUT_DIR}/Release/lib" )
set( GO_APPLICATION_OUT_DIR_RELEASE "${GO_OUT_DIR}/Release/bin" )
set( GO_TUTORIAL_OUT_DIR_RELEASE "${GO_OUT_DIR}/Release/tutorial" )
set( GO_TEST_OUT_DIR_RELEASE "${GO_OUT_DIR}/Release/test" )

set( GO_LIBRARY_OUT_DIR_RELWITHDEBINFO "${GO_OUT_DIR}/RelWithDebInfo/lib" )
set( GO_APPLICATION_OUT_DIR_RELWITHDEBINFO "${GO_OUT_DIR}/RelWithDebInfo/bin" )
set( GO_TUTORIAL_OUT_DIR_RELWITHDEBINFO "${GO_OUT_DIR}/RelWithDebInfo/tutorial" )
set( GO_TEST_OUT_DIR_RELWITHDEBINFO "${GO_OUT_DIR}/RelWithDebInfo/test" )

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
  GO_USE_THRUST
  "Set to ON to use CUDA Thrust library. If set to false, parallel versions of some algorithms might not be available"
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
  
option(
  GO_BUILD_APPLICATION_TUTORIAL
  "Set to ON to build the tutorial for the application library."
  ON)
  
option(
	GO_BUILD_GEOMETRY_TUTORIAL
	"Set to ON to build the tutorial for the geometry library."
	ON)

if( NOT GO_USE_QT )
  set( GO_BUILD_APPLICATION OFF)
  set( GO_BUILD_APPLICATION_TUTORIAL OFF )
else()
  if( NOT GO_BUILD_APPLICATION )
    set( GO_USE_QT OFF )
    set( GO_BUILD_APPLICATION_TUTORIAL OFF)
  endif()
endif()

#*********************
# external libraries *
#*******************************************************************************  
# openmesh
find_package( OpenMesh REQUIRED )
include_directories( ${OPENMESH_INCLUDE_DIR} )
link_directories( ${OPENMESH_LIBRARY_DIR} )

# boost
set( Boost_USE_MULTITHREADED ON )
set( Boost_USE_STATIC_LIBS OFF )
set( Boost_USE_STATIC_RUNTIME OFF )
find_package( Boost
  REQUIRED
  COMPONENTS serialization filesystem log log_setup locale random system )
include_directories( ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_DIRS} )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" )


if( GO_BUILD_APPLICATION )
  # qt
  set( CMAKE_INCLUDE_CURRENT_DIR ON )
  set( CMAKE_AUTOMOC ON )
  find_package( Qt5Core REQUIRED )
  find_package( Qt5Quick REQUIRED )
  find_package( Qt5Gui REQUIRED )
  find_package( Qt5Qml REQUIRED )
  find_package( Qt5Network REQUIRED )
  include_directories( ${Qt5Core_INCLUDE_DIRS} )
  include_directories( ${Qt5Quick_INCLUDE_DIRS} )
  include_directories( ${Qt5Gui_INCLUDE_DIRS} )
  include_directories( ${Qt5Qml_INCLUDE_DIRS} )
  include_directories( ${Qt5Network_INCLUDE_DIRS} )
  link_directories( ${Qt5Core_INCLUDE_DIRS} )
  link_directories( ${Qt5Quick_LIBRARY_DIRS} )
  link_directories( ${Qt5Gui_LIBRARY_DIRS} )
  link_directories( ${Qt5Qml_LIBRARY_DIRS} )
  link_directories( ${Qt5Network_LIBRARY_DIRS} )
  
  # glew
  find_package( GLEW REQUIRED )
  include_directories( ${GLEW_INCLUDE_DIRS} )
  
  # gl
  find_package( OpenGL REQUIRED )
  include_directories( ${OPENGL_INCLUDE_DIR} )
endif()

# cuda
if( GO_USE_CUDA )
  find_package( CUDA REQUIRED )
  if( NOT CUDA_FOUND )
    set( GO_USE_CUDA OFF )
  else()
    include_directories( ${CUDA_TOOLKIT_INCLUDE} )
    set( CUDA_LIBRARIES 
      "${CUDA_CUDA_LIBRARY}" "${CUDA_CUDART_LIBRARY}"
      CACHE STRING "Libraries to link with if you want to use CUDA")
  endif()
endif() 








set( GLM_INCLUDE_DIRS "${GO_EXTLIBS_DIR}" ) 
include_directories( ${GLM_INCLUDE_DIRS} )

#***************
# global flags *
#*******************************************************************************
if( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
  set( CMAKE_CXX_FLAGS 
    "-frounding-math -std=c++14 -fPIC -fopenmp -DGLM_FORCE_SSE2 -DGLM_FORCE_RADIANS -DBOOST_LOG_DYN_LINK -DBOOST_ALL_DYN_LINK"
    CACHE STRING "GNU C++ compiler flags" FORCE )
  set( CMAKE_CXX_FLAGS_RELEASE 
    "${CMAKE_CXX_FLAGS_RELEASE} -O3 -msse2 -UDEBUG -DNDEBUG -DNO_DEBUG" 
    CACHE STRING "GNU C++ compiler extra flags for release build type" FORCE )
  set( CMAKE_CXX_FLAGS_DEBUG 
    "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb -Wall -Wextra -DDEBUG" 
    CACHE STRING "GNU C++ compiler extra flags for debug build type" FORCE )
  set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lgomp" )
  
  if( ${GO_USE_CUDA} )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGO_USE_CUDA -DGO_USE_CUDA_THRUST" FORCE )
  endif()
elseif( CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" )
  set( CMAKE_CXX_FLAGS
    "/DGO_EXPORTS /EHsc /openmp /DGLM_FORCE_SSE2 /DGLM_FORCE_RADIANS /DBOOST_LOG_DYN_LINK /DBOOST_ALL_DYN_LINK"
	CACHE STRING "MSVC C++ compiler flags" FORCE )
  set( CMAKE_CXX_FLAGS_RELEASE
    "${CMAKE_CXX_FLAGS_RELEASE} /Ox /UDEBUG /DNDEBUG /DNO_DEBUG"
	CACHE STRING "MSVC C++ compiler extrat flags for release build type" FORCE )
  set( CMAKE_CXX_FLAGS_DEBUG 
    "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /DDEBUG" 
    CACHE STRING "MSVC C++ compiler extra flags for debug build type" FORCE )	
  if( ${GO_USE_CUDA} )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DGO_USE_CUDA /DGO_USE_CUDA_THRUST" FORCE )
  endif()	
else()
  message( SEND_ERROR "Compiler ${CMAKE_CXX_COMPILER_ID} not managed")
endif()



  

include_directories( "${GO_HEADERS_IN_DIR}" )
configure_file( "${GO_SOURCES_IN_DIR}/graphics_origin.h.in" "${GO_HEADERS_IN_DIR}/graphics-origin/graphics_origin.h")

install( DIRECTORY graphics-origin DESTINATION include )
install( FILES ${CMAKE_SOURCE_DIR}/cmake/modules/FindGraphicsOrigin.cmake DESTINATION share/cmake/modules )


function( add_go_lib lib_name)
  add_library( 
    graphics_origin_${lib_name} SHARED ${ARGN} )
 
  message( STATUS
    "new shared library: graphics_origin_${lib_name} from")
  foreach( var ${ARGN})
    message( STATUS "   file ${var}")
  endforeach()
endfunction()

function( set_go_lib_properties lib_name )
  string( TOUPPER ${lib_name} lib_name_upper )
  
  set( GO_${lib_name_upper}_LIBRARIES
    "graphics_origin_${lib_name}" ${ARGN}
	CACHE STRING "Libraries to link with if you want to use the ${lib_name} module"
	FORCE )
  
  set( GO_${lib_name_upper}_LIBRARIES_DEBUG
    "${GO_OUT_DIR}/Debug/lib/graphics_origin_${lib_name}" ${ARGN}
	CACHE STRING "Libraries to link with if you want to use the ${lib_name} module in Debug mode"
	FORCE )
  set( GO_${lib_name_upper}_LIBRARIES_RELEASE
    "${GO_OUT_DIR}/Release/lib/graphics_origin_${lib_name}" ${ARGN}
	CACHE STRING "Libraries to link with if you want to use the ${lib_name} module in Release mode"
	FORCE )
  set( GO_${lib_name_upper}_LIBRARIES_RELWITHDEBINFO
    "${GO_OUT_DIR}/RelWithDebInfo/lib/graphics_origin_${lib_name}" ${ARGN}
	CACHE STRING "Libraries to link with if you want to use the ${lib_name} module in Release mode with Debug info"
	FORCE )	

  set_target_properties( graphics_origin_${lib_name}
	PROPERTIES
	IMPORTED_LOCATION_RELEASE "${GO_LIBRARY_OUT_DIR_RELEASE}/graphics_origin_${lib_name}"
	RUNTIME_OUTPUT_DIRECTORY_RELEASE "${GO_LIBRARY_OUT_DIR_RELEASE}"
	ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${GO_LIBRARY_OUT_DIR_RELEASE}"
	LIBRARY_OUTPUT_DIRECTORY_RELEASE "${GO_LIBRARY_OUT_DIR_RELEASE}"
    
	IMPORTED_LOCATION_DEBUG "${GO_LIBRARY_OUT_DIR_DEBUG}/graphics_origin_${lib_name}"
	RUNTIME_OUTPUT_DIRECTORY_DEBUG "${GO_LIBRARY_OUT_DIR_DEBUG}"
	ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${GO_LIBRARY_OUT_DIR_DEBUG}"
	LIBRARY_OUTPUT_DIRECTORY_DEBUG "${GO_LIBRARY_OUT_DIR_DEBUG}"
	
    IMPORTED_LOCATION_RELWITHDEBINFO "${GO_LIBRARY_OUT_DIR_RELWITHDEBINFO}/graphics_origin_${lib_name}"
	RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${GO_LIBRARY_OUT_DIR_RELWITHDEBINFO}"
	ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${GO_LIBRARY_OUT_DIR_RELWITHDEBINFO}"
	LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${GO_LIBRARY_OUT_DIR_RELWITHDEBINFO}"
  )
endfunction() 

function( set_go_tutorial_properties tutorial_name )
  set_target_properties( ${tutorial_name}
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY_RELEASE "${GO_TUTORIAL_OUT_DIR_RELEASE}"
	ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${GO_TUTORIAL_OUT_DIR_RELEASE}"
	LIBRARY_OUTPUT_DIRECTORY_RELEASE "${GO_TUTORIAL_OUT_DIR_RELEASE}"

	RUNTIME_OUTPUT_DIRECTORY_DEBUG "${GO_TUTORIAL_OUT_DIR_DEBUG}"
	ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${GO_TUTORIAL_OUT_DIR_DEBUG}"
	LIBRARY_OUTPUT_DIRECTORY_DEBUG "${GO_TUTORIAL_OUT_DIR_DEBUG}"
  
	RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${GO_TUTORIAL_OUT_DIR_RELWITHDEBINFO}"
	ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${GO_TUTORIAL_OUT_DIR_RELWITHDEBINFO}"
	LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${GO_TUTORIAL_OUT_DIR_RELWITHDEBINFO}"
  )
endfunction() 

add_subdirectory( "${GO_SOURCES_IN_DIR}/tools" )
add_subdirectory( "${GO_SOURCES_IN_DIR}/geometry" )
if( GO_BUILD_APPLICATION )
  add_subdirectory( "${GO_SOURCES_IN_DIR}/application" )
endif()

if( GO_BUILD_TOOLS_TUTORIAL )
  add_subdirectory( "${GO_TUTORIAL_IN_DIR}/1_tools" )
endif()

if( GO_BUILD_GEOMETRY_TUTORIAL )
	add_subdirectory( "${GO_TUTORIAL_IN_DIR}/2_geometry" )
endif()

if( GO_BUILD_APPLICATION_TUTORIAL )
  add_subdirectory( "${GO_TUTORIAL_IN_DIR}/3_application" )
endif()

message(
  "Graphics-Origin Options:\n"
  "GO_USE_CGAL    = ${GO_USE_CGAL}\n"
  "GO_USE_QT      = ${GO_USE_QT}\n"
  "GO_USE_CUDA    = ${GO_USE_CUDA}\n"
  "GO_USE_THRUST  = ${GO_USE_THRUST}\n"
)
