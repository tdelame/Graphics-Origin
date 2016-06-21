# NAME name of the graphics tutorial
#   There should be a NAME.cc file and a NAME/ directory in the current source directory.
#   All common sources, sources in NAME/ and NAME.cc are automatically taken into account.
#   QML and Shader files in the NAME/ directory will be copied to the binary directory.
#
# SHADERS list of shaders outside of the NAME/ directory required to launch the tutorial
#
# MESHES list of mesh files required for this tutorial
#
# TEXTURES list of texture files required for this tutorial
#
# LIBRARIES list of libraries to link the tutorial with
function(add_graphics_tutorial)
  set(options "")
  set(oneValueArgs NAME)
  set(multiValueArgs SHADERS MESHES TEXTURES LIBRARIES)
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
  set( TUTORIAL_TEXTURES "${AGT_TEXTURES}" )
  
  add_executable( ${tutorial_name} ${TUTORIAL_SOURCES})
  target_link_libraries( ${tutorial_name} ${AGT_LIBRARIES})
  set_go_tutorial_properties(
    NAME ${tutorial_name} 
    MODULE 4_graphics )
  
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
    
    #get_target_property( tutorial_location $<TARGET_FILE:${tutorial_name}> LOCATION )
    #get_filename_component( tutorial_destination $<TARGET_FILE_DIR:${tutorial_name}> PATH )
    set( _destination $<TARGET_FILE_DIR:${tutorial_name}>/shaders )
    #message( STATUS "destination = ${_destination}" )
    
    #foreach( _file ${TUTORIAL_SHADERS} )
    #      list( APPEND _output ${_destination}/${_file} )
    #endforeach()
    set( _output ${CMAKE_BINARY_DIR}/tutorial/4_graphics/${CMAKE_BUILD_TYPE}/${tutorial_name}_shaders.stamp)
    add_custom_command( TARGET ${tutorial_name} #OUTPUT ${_output}
      DEPENDS ${TUTORIAL_SHADERS}
      COMMENT "Copying shader files for tutorial ${tutorial_name}"
      COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/tutorial/4_graphics/${CMAKE_BUILD_TYPE}
      COMMAND ${CMAKE_COMMAND} -E touch ${_output}
      COMMAND ${CMAKE_COMMAND} -E make_directory ${_destination}
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${TUTORIAL_SHADERS} ${_destination}
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
  
  if( NOT "${TUTORIAL_TEXTURES}" STREQUAL "" ) 
    set( _destination $<TARGET_FILE_DIR:${tutorial_name}>/textures )
    add_custom_command( TARGET ${tutorial_name}
        DEPENDS ${TUTORIAL_TEXTURES}
        COMMENT "Copying texture files for tutorial ${tutorial_name}"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${_destination}
        COMMAND ${CMAKE_COMMAND} -E copy ${TUTORIAL_TEXTURES} ${_destination}
      )
  endif()  
endfunction()

  add_graphics_tutorial(
    NAME transparency
    SHADERS 
      ${GO_SHADERS_DIR}/textured_mesh.vert ${GO_SHADERS_DIR}/textured_mesh.frag
    MESHES
      ${GO_MESHES_DIR}/Bunny.obj
    TEXTURES
      ${GO_TEXTURES_DIR}/TexturedBunny.png
    LIBRARIES
      ${GO_APPLICATION_LIBRARIES} 
  )
  
  add_graphics_tutorial(
    NAME transparency2
    SHADERS 
      ${GO_SHADERS_DIR}/textured_mesh.vert ${GO_SHADERS_DIR}/textured_mesh.frag
    MESHES
      ${GO_MESHES_DIR}/Bunny.obj
    TEXTURES
      ${GO_TEXTURES_DIR}/TexturedBunny.png
    LIBRARIES
      ${GO_APPLICATION_LIBRARIES} 
  )  

# get it here: https://github.com/qknight/libnoise
find_package( LibNoise  REQUIRED QUIET )

if( LIBNOISE_FOUND )
  include_directories( ${LIBNOISE_INCLUDE_DIR} )

  add_graphics_tutorial(
    NAME island_map
    SHADERS
      ${GO_SHADERS_DIR}/flat.vert ${GO_SHADERS_DIR}/flat.frag
    LIBRARIES
      ${GO_APPLICATION_LIBRARIES} ${GO_CGAL_LIBRARIES} ${LIBNOISE_LIBRARIES}
  )
else()
  message( STATUS "4_graphics/island_map not built because LibNoise was not found")
endif()
  
  