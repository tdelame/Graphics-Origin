#*******************************************************************************
# GRAPHICS ORIGIN BUILDING SYSTEM                                              *
#*******************************************************************************
# Options
# -------
#   GO_USE_OPENCL                 Set to ON to enable parallelization with OpenCL
#   GO_BUILD_APPLICATION          Set to ON to build the application library
#   GO_BUILD_TOOLS_TUTORIAL       Set to ON to build the tutorial for the tools library
#   GO_BUILD_APPLICATION_TUTORIAL Set to ON to build the tutorial for the application library
#   GO_BUILD_GEOMETRY_TUTORIAL    Set to ON to build the tutorial for the geometry library
#   GO_BUILD_GRAPHICS_TUTORIAL    Set to ON to build tutorials on Computer Graphics with GO libraries
#   GO_BUILD_TEST                 Set to ON to build tests
#
# Finding libraries
# -----------------
# Set those variables to help cmake finding the libraries you want to use.
#   BOOST_ROOT            where boost has been installed
#   BOOST_INCLUDEDIR      Preferred include directory e.g. <prefix>/include
#   BOOST_LIBRARYDIR      Preferred library directory e.g. <prefix>/lib
#   Boost_NO_SYSTEM_PATHS Set to ON to disable searching in locations not specified by these hint variables. Default is OFF.
#   OPENMESH_DIR          where to find OpenMesh/Core/Mesh/PolyMeshT.hh
#   LIBNOISE_DIR          where to find either noise/noise.h or include/noise/noise.h
#   GLEW_DIR              where to find GL/glew.h
#   FREEIMAGE_DIR         where to find FreeImage.h
#   CMAKE_PREFIX_PATH     use this variable to specify where to find the Qt installation root directory
#
# Build types
# -----------
# Valid values of CMAKE_BUILD_TYPE:
# - Release
# - RelWithDebInfo
# - Debug
 
#**********************
# cmake configuration *
#*******************************************************************************
cmake_minimum_required( VERSION 3.1.0 )

set( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )

# Graphics-Origin project
project( Graphics-Origin )
set( Graphics-Origin_VERSION_MAJOR 1 )
set( Graphics-Origin_VERSION_MINOR 0 )

if( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
  if( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE "Release" 
      CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo." FORCE)
  endif(NOT CMAKE_BUILD_TYPE)
endif()

#****************************
# directories configuration *
#*******************************************************************************
set( GO_HEADERS_IN_DIR "${CMAKE_SOURCE_DIR}" )
set( GO_SOURCES_IN_DIR "${CMAKE_SOURCE_DIR}/lib" )
set( GO_TUTORIAL_IN_DIR "${CMAKE_SOURCE_DIR}/tutorial" )
set( GO_TEST_IN_DIR "${CMAKE_SOURCE_DIR}/test" )
set( GO_SHADERS_DIR "${CMAKE_SOURCE_DIR}/shaders" )
set( GO_TEXTURES_DIR "${CMAKE_SOURCE_DIR}/textures" )
set( GO_MESHES_DIR "${CMAKE_SOURCE_DIR}/meshes" )
set( GO_QML_DIR "${CMAKE_SOURCE_DIR}/qml" )
set( GO_OUT_DIR "${CMAKE_BINARY_DIR}")

set( GO_LIBRARY_OUT_DIR_DEBUG "${GO_OUT_DIR}/debug/lib" )
set( GO_APPLICATION_OUT_DIR_DEBUG "${GO_OUT_DIR}/debug/bin" )
set( GO_TUTORIAL_OUT_DIR_DEBUG "${GO_OUT_DIR}/debug/tutorial" )
set( GO_TEST_OUT_DIR_DEBUG "${GO_OUT_DIR}/debug/test" )

set( GO_LIBRARY_OUT_DIR_RELEASE "${GO_OUT_DIR}/release/lib" )
set( GO_APPLICATION_OUT_DIR_RELEASE "${GO_OUT_DIR}/release/bin" )
set( GO_TUTORIAL_OUT_DIR_RELEASE "${GO_OUT_DIR}/release/tutorial" )
set( GO_TEST_OUT_DIR_RELEASE "${GO_OUT_DIR}/release/test" )

set( GO_LIBRARY_OUT_DIR_RELWITHDEBINFO "${GO_OUT_DIR}/relwithdebinfo/lib" )
set( GO_APPLICATION_OUT_DIR_RELWITHDEBINFO "${GO_OUT_DIR}/relwithdebinfo/bin" )
set( GO_TUTORIAL_OUT_DIR_RELWITHDEBINFO "${GO_OUT_DIR}/relwithdebinfo/tutorial" )
set( GO_TEST_OUT_DIR_RELWITHDEBINFO "${GO_OUT_DIR}/relwithdebinfo/test" )

#*********************
# projection options *
#*******************************************************************************
option(
  GO_USE_OPENCL
  "Set to ON to enable parallelization with OpenCL (not recommended for now)."
  OFF)

option(
  GO_BUILD_APPLICATION
  "Set to ON to build the application library."
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
  GO_BUILD_GRAPHICS_TUTORIAL
  "Set to ON to build tutorials about Computer Graphics with GO libraries."
  ON)  
  
option(
  GO_BUILD_GEOMETRY_TUTORIAL
  "Set to ON to build the tutorial for the geometry library."
  ON)
  
option(
  GO_BUILD_TEST
  "Set to ON to build some tests."
  ON)  

option(
  GO_BUILD_DOC
  "Set to ON to build documentation."
  OFF) 

#*********************
# external libraries *
#*******************************************************************************
# reset flags
set( CMAKE_CXX_FLAGS "" )

# external header-only libraries: glm, nanoflann, spectra, Cuda/Thrust
include_directories( "${CMAKE_SOURCE_DIR}/graphics-origin/extlibs" )
  
# OpenMP
#   OpenMP_CXX_FLAGS - flags to add to the CXX compiler for OpenMP support
#   OPENMP_FOUND - true if openmp is detected
find_package( OpenMP QUIET )
if( OPENMP_FOUND )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}" )
else()
  message( SEND_ERROR "Cannot find OpenMP to parallelize code. Disable OpenMP or help cmake to find it." )
endif()

# OpenCL
#   OpenCL_FOUND          - True if OpenCL was found
#   OpenCL_INCLUDE_DIRS   - include directories for OpenCL
#   OpenCL_LIBRARIES      - link against this library to use OpenCL
if( GO_USE_OPENCL )
  find_package( OpenCL )
  if( OpenCL_FOUND )
    include_directories( ${OpenCL_INCLUDE_DIRS} )
  else()
    message( SEND_ERROR "Cannot find OpenCL to parallelize code. Disable OpenCL or help cmake to find it." )
  endif()
endif()

# openmesh
find_package( OpenMesh REQUIRED QUIET )
include_directories( ${OPENMESH_INCLUDE_DIR} )
link_directories( ${OPENMESH_LIBRARY_DIR} )

# boost
set( Boost_USE_MULTITHREADED ON )
set( Boost_USE_STATIC_LIBS OFF )
set( Boost_USE_STATIC_RUNTIME OFF )
find_package( Boost
  REQUIRED QUIET
  COMPONENTS filesystem log log_setup locale random system )
include_directories( ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_DIRS} )
set( GO_Boost_LIBRARIES ${Boost_LIBRARIES} )

if( GO_BUILD_APPLICATION )
  # qt
  set( CMAKE_INCLUDE_CURRENT_DIR ON )
  set( CMAKE_AUTOMOC ON )
  find_package( Qt5Core REQUIRED QUIET )
  find_package( Qt5Quick REQUIRED QUIET )
  find_package( Qt5Gui REQUIRED QUIET )
  find_package( Qt5Qml REQUIRED QUIET )
  find_package( Qt5Network REQUIRED QUIET )
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
  find_package( GLEW REQUIRED QUIET )
  include_directories( ${GLEW_INCLUDE_DIRS} )
  
  # gl
  find_package( OpenGL REQUIRED QUIET )
  include_directories( ${OPENGL_INCLUDE_DIR} )
  
  # freeImage
  find_package( FreeImage REQUIRED QUIET )
  include_directories( ${FREEIMAGE_INCLUDE_DIRS} )
endif()

#*************** 
# global flags *
#*******************************************************************************
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
  set( CMAKE_CXX_FLAGS 
    "${CMAKE_CXX_FLAGS} -frounding-math -DGLM_FORCE_SSE2 -DGLM_FORCE_RADIANS -DBOOST_LOG_DYN_LINK -DBOOST_ALL_DYN_LINK"
    CACHE STRING "GNU C++ compiler flags" FORCE )
  set( CMAKE_CXX_FLAGS_RELEASE 
    "${CMAKE_CXX_FLAGS_RELEASE} -O3 -msse2 -UDEBUG -DNDEBUG -DNO_DEBUG -DCGAL_NDEBUG" 
    CACHE STRING "GNU C++ compiler extra flags for release build type" FORCE )
  set( CMAKE_CXX_FLAGS_DEBUG 
    "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb -Wall -Wextra -DDEBUG" 
    CACHE STRING "GNU C++ compiler extra flags for debug build type" FORCE )
 	set( CMAKE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lgomp" )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTHRUST_DEVICE_SYSTEM=THRUST_DEVICE_SYSTEM_OMP -DTHRUST_HOST_SYSTEM=THRUST_HOST_SYSTEM_OMP" )
elseif( CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" )
  set( CMAKE_CXX_FLAGS
    "/EHsc /openmp /D_USE_MATH_DEFINES /DGLM_FORCE_SSE2 /DGLM_FORCE_RADIANS /DBOOST_LOG_DYN_LINK /DBOOST_ALL_DYN_LINK"
	CACHE STRING "MSVC C++ compiler flags" FORCE )
  set( CMAKE_CXX_FLAGS_RELEASE
    "${CMAKE_CXX_FLAGS_RELEASE} /Ox /UDEBUG /DNDEBUG /DNO_DEBUG"
	CACHE STRING "MSVC C++ compiler extrat flags for release build type" FORCE )
  set( CMAKE_CXX_FLAGS_DEBUG 
    "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /DDEBUG" 
    CACHE STRING "MSVC C++ compiler extra flags for debug build type" FORCE )	
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DTHRUST_DEVICE_SYSTEM=THRUST_DEVICE_SYSTEM_OMP /DTHRUST_HOST_SYSTEM=THRUST_HOST_SYSTEM_OMP" )
else()
  message( SEND_ERROR "Compiler ${CMAKE_CXX_COMPILER_ID} not managed")
endif()

#*********
# macros *
#*******************************************************************************
# in order to add a library, one should do the following
#  add_go_lib( lib_name list_of_sources )
#  set_go_lib_properties( lib_name list_of_libraries )
  
# add_go_lib new_library_name list_of_sources
#   create a new library from a list of sources/headers
function( add_go_lib lib_name)
  add_library( 
    graphics_origin_${lib_name} SHARED ${ARGN} )
endfunction()

# set_go_lib_properties library_name list_of_libraries_to_link
#   set the necessary properties of a library
#   link the library with required libraries
function( set_go_lib_properties lib_name )
  string( TOUPPER ${lib_name} lib_name_upper )
  set( GO_${lib_name_upper}_LIBRARIES
    "graphics_origin_${lib_name}" ${ARGN}
	CACHE STRING "Libraries to link with if you want to use the ${lib_name} module"
	FORCE )
  
  set_target_properties( graphics_origin_${lib_name}
	PROPERTIES
	COMPILE_DEFINITIONS "GO_EXPORTS"
	
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
  
  target_link_libraries( graphics_origin_${lib_name} ${ARGN} )
endfunction() 

#
# NAME
#
# MODULE
#
function( set_go_tutorial_properties )

  set( options "" )
  set( oneValueArgs NAME MODULE )
  set( multiValueArgs "" )
  cmake_parse_arguments( TUT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

  set_target_properties( ${TUT_NAME} 
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY_RELEASE "${GO_TUTORIAL_OUT_DIR_RELEASE}/${TUT_MODULE}"
	ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${GO_TUTORIAL_OUT_DIR_RELEASE}/${TUT_MODULE}"
	LIBRARY_OUTPUT_DIRECTORY_RELEASE "${GO_TUTORIAL_OUT_DIR_RELEASE}/${TUT_MODULE}"

	RUNTIME_OUTPUT_DIRECTORY_DEBUG "${GO_TUTORIAL_OUT_DIR_DEBUG}/${TUT_MODULE}"
	ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${GO_TUTORIAL_OUT_DIR_DEBUG}/${TUT_MODULE}"
	LIBRARY_OUTPUT_DIRECTORY_DEBUG "${GO_TUTORIAL_OUT_DIR_DEBUG}/${TUT_MODULE}"
  
	RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${GO_TUTORIAL_OUT_DIR_RELWITHDEBINFO}/${TUT_MODULE}"
	ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${GO_TUTORIAL_OUT_DIR_RELWITHDEBINFO}/${TUT_MODULE}"
	LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${GO_TUTORIAL_OUT_DIR_RELWITHDEBINFO}/${TUT_MODULE}"
  )
endfunction() 




include_directories( "${GO_HEADERS_IN_DIR}" )
configure_file( "${GO_SOURCES_IN_DIR}/graphics_origin.h.in" "${GO_HEADERS_IN_DIR}/graphics-origin/graphics_origin.h")

install( DIRECTORY graphics-origin DESTINATION include )
install( FILES ${CMAKE_SOURCE_DIR}/cmake/modules/FindGraphicsOrigin.cmake DESTINATION share/cmake/modules )
install( FILES ${CMAKE_SOURCE_DIR}/cmake/modules/FindLibNoise.cmake DESTINATION share/cmake/modules )
install( FILES ${CMAKE_SOURCE_DIR}/cmake/modules/FindOpenMesh.cmake DESTINATION share/cmake/modules )
install( FILES ${CMAKE_SOURCE_DIR}/cmake/modules/FindGLEW.cmake DESTINATION share/cmake/modules )
install( FILES ${CMAKE_SOURCE_DIR}/cmake/modules/FindFreeImage.cmake DESTINATION share/cmake/modules )


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

if( GO_BUILD_GRAPHICS_TUTORIAL )
  add_subdirectory( "${GO_TUTORIAL_IN_DIR}/4_graphics" )
endif()


if( GO_BUILD_TEST )
  find_package( Boost
    REQUIRED QUIET
    COMPONENTS unit_test_framework )
  set( GO_TEST_LIBRARIES ${Boost_LIBRARIES})

  # set_go_test_properties test_name
  function( set_go_test_properties test_name )
    set_target_properties( ${test_name}
	  PROPERTIES
	  RUNTIME_OUTPUT_DIRECTORY_RELEASE "${GO_TEST_OUT_DIR_RELEASE}"
	  ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${GO_TEST_OUT_DIR_RELEASE}"
	  LIBRARY_OUTPUT_DIRECTORY_RELEASE "${GO_TEST_OUT_DIR_RELEASE}"

	  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${GO_TEST_OUT_DIR_DEBUG}"
	  ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${GO_TEST_OUT_DIR_DEBUG}"
	  LIBRARY_OUTPUT_DIRECTORY_DEBUG "${GO_TEST_OUT_DIR_DEBUG}"
  
	  RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${GO_TEST_OUT_DIR_RELWITHDEBINFO}"
	  ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${GO_TEST_OUT_DIR_RELWITHDEBINFO}"
	  LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${GO_TEST_OUT_DIR_RELWITHDEBINFO}"
    )
  endfunction() 

  add_subdirectory( "${GO_TEST_IN_DIR}" )
endif()

# build documentation
if( ${GO_BUILD_DOC} )
  find_package( Doxygen )
  if( DOXYGEN_FOUND )
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        file(MAKE_DIRECTORY doc) 
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doc
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
  else()
    message( SEND_ERROR "Cannot find Doxygen to build the documentation" )
	set( GO_BUILD_DOC OFF )
  endif()
endif()

message(
  "Graphics-Origin Options:\n"
  "GO_USE_OPENCL                 = ${GO_USE_OPENCL}\n"
  "GO_BUILD_APPLICATION          = ${GO_BUILD_APPLICATION}\n"
  "GO_BUILD_TOOLS_TUTORIAL       = ${GO_BUILD_TOOLS_TUTORIAL}\n"
  "GO_BUILD_APPLICATION_TUTORIAL = ${GO_BUILD_APPLICATION_TUTORIAL}\n"
  "GO_BUILD_GEOMETRY_TUTORIAL    = ${GO_BUILD_GEOMETRY_TUTORIAL}\n"
  "GO_BUILD_GRAPHICS_TUTORIAL    = ${GO_BUILD_GRAPHICS_TUTORIAL}\n"
  "GO_BUILD_TEST                 = ${GO_BUILD_TEST}\n"
  "GO_BUILD_DOC                  = ${GO_BUILD_DOC}\n\n"
  
  "External libraries:\n"
  "* Boost\n"
  "   - include: ${Boost_INCLUDE_DIRS}\n"
  "   - libs:    ${GO_Boost_LIBRARIES}\n"
  "* OpenMesh\n"
  "   - include: ${OPENMESH_INCLUDE_DIRS}\n"
  "   - libs:    ${OPENMESH_LIBRARIES}\n"
  "* GLEW\n"
  "   - include: ${GLEW_INCLUDE_DIRS}\n"
  "   - libs:    ${GLEW_LIBRARIES}\n"
  "* FreeImage\n"
  "   - include: ${FREEIMAGE_INCLUDE_DIRS}\n"
  "   - libs:    ${FREEIMAGE_LIBRARIES}\n"
  "* Libnoise\n"
  "   - include: ${LIBNOISE_INCLUDE_DIRS}\n"
  "   - libs:    ${LIBNOISE_LIBRARIES}\n"
  "* Test libraries\n"
  "   - libs:    ${GO_TEST_LIBRARIES}\n"
  
)
