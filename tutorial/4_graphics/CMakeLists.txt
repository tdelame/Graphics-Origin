# NAME name of the graphics tutorial
#   There should be a NAME.cc file and a NAME/ directory in the current source directory.
#   All common sources, sources in NAME/ and NAME.cc are automatically taken into account.
#   QML and Shader files in the NAME/ directory will be copied to the binary directory.
#
# SHADERS list of shaders outside of the NAME/ directory required to launch the tutorial
#
# MESHES list of mesh files required for this tutorial
#
# LIBRARIES list of libraries to link the tutorial with
function(add_graphics_tutorial)
  set(options "")
  set(oneValueArgs NAME)
  set(multiValueArgs SHADERS MESHES LIBRARIES)
  cmake_parse_arguments( AGT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

  file(GLOB_RECURSE TUTORIAL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/${AGT_NAME}/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/${AGT_NAME}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/common/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/common/*.h
    ${AGT_NAME}.cc
  )
  file(GLOB_RECURSE TUTORIAL_QML
    ${CMAKE_CURRENT_SOURCE_DIR}/${AGT_NAME}/*.qml
    ${GO_QML_DIR}/*.qml
    ${GO_QML_DIR}/qmldir
  )
  file(GLOB_RECURSE TUTORIAL_SHADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/${AGT_NAME}/*.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/${AGT_NAME}/*.geom
    ${CMAKE_CURRENT_SOURCE_DIR}/${AGT_NAME}/*.frag
  )
  set( tutorial_name 4_${AGT_NAME})
  set( ${TUTORIAL_SHADERS} "${TUTORIAL_SHADERS} ${AGT_SHADERS}" )
  set( ${TUTORIAL_MESHES} "${AGT_MESHES}" )
  
  
  add_executable( ${tutorial_name}
    ${TUTORIAL_SOURCES} ${TUTORIAL_QML} ${TUTORIAL_SHADERS} ${TUTORIAL_MESHES}
  )
 
  target_link_libraries( ${tutorial_name} ${AGT_LIBRARIES} )
  
  set_go_tutorial_properties( ${tutorial_name} )
  
  if( "${TUTORIAL_QML}" )
    add_custom_command( TARGET ${tutorial_name}
      POST_BUILD
        COMMENT "Copying QML files to binary directory" 
        COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${tutorial_name}>/qml
        COMMAND ${CMAKE_COMMAND} -E copy ${TUTORIAL_QML} $<TARGET_FILE_DIR:${tutorial_name}>/qml
    )
  endif()
  
  if( "${TUTORIAL_SHADERS}" )
    add_custom_command( TARGET ${tutorial_name}
      COMMENT "Copying GLSL Shaders to binary directory"
      COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${tutorial_name}>/shaders
      COMMAND ${CMAKE_COMMAND} -E copy ${TUTORIAL_SHADERS} $<TARGET_FILE_DIR:${tutorial_name}>/shaders
    )
  endif()
  
  if( "${TUTORIAL_MESHES}" ) 
    add_custom_command( TARGET ${tutorial_name}
      COMMENT "Copying mesh files to binary directory"
      COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${tutorial_name}>/meshes 
      COMMAND ${CMAKE_COMMAND} -E copy ${TUTORIAL_MESHES} $<TARGET_FILE_DIR:${tutorial_name}>/meshes
    )
  endif()
endfunction()

add_graphics_tutorial(
  NAME transparency
  SHADERS 
    ${GO_SHADERS_DIR}/mesh.vert ${GO_SHADERS_DIR}/mesh.frag
    ${GO_SHADERS_DIR}/transparent_window.vert ${GO_SHADERS_DIR}/transparent_window.geom ${GO_SHADERS_DIR}/transparent_window.frag
  MESHES
    ${GO_MESHES_DIR}/armadillo.off
  LIBRARIES
    ${GO_APPLICATION_LIBRARIES}
)

set( CGAL_CORE_PACKAGE_DIR "/usr"
	CACHE FILEPATH "Variable to try to find CGAL_CORE package i.e. the directory where include/CGAL/CORE can be found" )

find_package( GMP REQUIRED )
find_package( MPFR REQUIRED )
find_package( TBB REQUIRED COMPONENTS tbbmalloc )
if( NOT GMP_FOUND )
  message( fatal "GMP not found")
endif()
if( NOT MPFR_FOUND )
  message( fatal "MPFR not found")
endif()

include_directories( ${GMP_INCLUDE_DIR} ${MPFR_INCLUDE_DIR} ${TBB_INCLUDE_DIRS})
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCGAL_DISABLE_ROUNDING_MATH_CHECK ${TBB_DEFINITIONS}")
set( GO_CGAL_LIBRARIES ${GMP_LIBRARIES} ${MPFR_LIBRARIES} -lCGAL -lCGAL_Core ${TBB_LIBRARIES} ${CGAL_CORE_LIBRARY})

# get it here: https://github.com/qknight/libnoise
find_package( LibNoise REQUIRED )
include_directories( ${LIBNOISE_INCLUDE_DIR} )

add_graphics_tutorial(
  NAME island_map
  SHADERS
    ${GO_SHADERS_DIR}/flat.vert ${GO_SHADERS_DIR}/flat.frag
  LIBRARIES
    ${GO_APPLICATION_LIBRARIES} ${GO_CGAL_LIBRARIES} ${LIBNOISE_LIBRARIES}
)
  