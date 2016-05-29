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
    ${CMAKE_CURRENT_SOURCE_DIR}/${AGT_NAME}/*.tes
    ${CMAKE_CURRENT_SOURCE_DIR}/${AGT_NAME}/*.tcs
    ${CMAKE_CURRENT_SOURCE_DIR}/${AGT_NAME}/*.geom
    ${CMAKE_CURRENT_SOURCE_DIR}/${AGT_NAME}/*.frag
    ${AGT_SHADERS}
  )
  set( tutorial_name ${AGT_NAME})
  set( TUTORIAL_MESHES "${AGT_MESHES}" )
  
  add_executable( ${tutorial_name} ${TUTORIAL_SOURCES})
  target_link_libraries( ${tutorial_name} ${AGT_LIBRARIES})
  set_go_tutorial_properties( ${tutorial_name} )
  
  if( NOT "${TUTORIAL_QML}" STREQUAL "" )
    set( _destination $<TARGET_FILE_DIR:${tutorial_name}>/qml )
    add_custom_command( TARGET ${tutorial_name}
      DEPENDS ${TUTORIAL_QML}
      COMMENT "Copying QML files for tutorial ${tutorial_name}"
      COMMAND ${CMAKE_COMMAND} -E make_directory ${_destination}
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${TUTORIAL_QML} ${_destination}
    )
    #add_custom_target( ${tutorial_name}.qml DEPENDS ${_output} )
    #add_dependencies( ${tutorial_name} ${tutorial_name}.qml )
  endif()
  
  if( NOT "${TUTORIAL_SHADERS}" STREQUAL "" )
    set( _destination $<TARGET_FILE_DIR:${tutorial_name}>/shaders )
    #foreach( _file ${TUTORIAL_SHADERS} )
      #list( APPEND _output ${_destination}/${_file} )
    #endforeach()
    add_custom_command( TARGET ${tutorial_name}
      DEPENDS ${TUTORIAL_SHADERS}
      COMMENT "Copying shader files for tutorial ${tutorial_name}"
      COMMAND ${CMAKE_COMMAND} -E make_directory ${_destination}
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${TUTORIAL_SHADERS} ${_file} ${_destination}
    )
    #add_custom_target( ${tutorial_name}.shaders DEPENDS ${_output} )
    #add_dependencies( ${tutorial_name} ${tutorial_name}.shaders ) 
  endif()
  
  if( NOT "${TUTORIAL_MESHES}" STREQUAL "" ) 
    set( _destination $<TARGET_FILE_DIR:${tutorial_name}>/meshes )
    add_custom_command( TARGET ${tutorial_name}
        DEPENDS ${TUTORIAL_MESHES}
        COMMENT "Copying mesh files for tutorial ${tutorial_name}"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${_destination}
        COMMAND ${CMAKE_COMMAND} -E copy ${TUTORIAL_MESHES} ${_destination}
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
  