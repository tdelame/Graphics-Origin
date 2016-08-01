# NAME name of the test
#  There should be a NAME.cc file and a NAME/ directory in the current source directory.
#  All sources in NAME/ and NAME.Cc are automatically taken into account.
#
# LIBRARIES list of libraries to link the test with
#
# MESHES list of mesh files required for this tutorial
function(go_add_test)
  set(options "")
  set(oneValueArgs NAME)
  set(multiValueArgs LIBRARIES MESHES)
  cmake_parse_arguments(GOT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  
  file( GLOB_RECURSE TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/${GOT_NAME}/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/${GOT_NAME}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/${GOT_NAME}.cc
  )
  set( test_name ${GOT_NAME})
  
  add_executable( ${test_name} ${TEST_SOURCES})
  target_link_libraries( ${test_name} ${GOT_LIBRARIES})
  set_go_test_properties( ${test_name})
  
  if( NOT "${GOT_MESHES}" STREQUAL "" ) 
    set( _destination $<TARGET_FILE_DIR:${test_name}>/meshes )
    add_custom_command( TARGET ${test_name}
        DEPENDS ${GOT_MESHES}
        COMMENT "Copying mesh files for test ${test_name}"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${_destination}
        COMMAND ${CMAKE_COMMAND} -E copy ${GOT_MESHES} ${_destination}
      )
  endif()
  
endfunction()

go_add_test( NAME memory_design_test )
go_add_test( NAME 0_design_test )

go_add_test( NAME unit_tests 
  LIBRARIES ${GO_TOOLS_LIBRARIES} ${GO_GEOMETRY_LIBRARIES} ${GO_APPLICATION_LIBRARIES} ${GO_TEST_LIBRARIES} )

#go_add_test( NAME windows_cuda_thrust_bug )

go_add_test( NAME 2_bvh_performances
  LIBRARIES ${GO_TOOLS_LIBRARIES} ${GO_GEOMETRY_LIBRARIES}
  MESHES ${GO_MESHES_DIR}/armadillo.off ${GO_MESHES_DIR}/Bunny.obj ${GO_MESHES_DIR}/spot_triangulated.obj
)
