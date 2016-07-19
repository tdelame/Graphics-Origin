# NAME name of the test
#  There should be a NAME.cc file and a NAME/ directory in the current source directory.
#  All sources in NAME/ and NAME.Cc are automatically taken into account.
#
# LIBRARIES list of libraries to link the test with
function(go_add_test)
  set(options "")
  set(oneValueArgs NAME)
  set(multiValueArgs LIBRARIES)
  cmake_parse_arguments(GAT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  
  file( GLOB_RECURSE TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/${GAT_NAME}/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/${GAT_NAME}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/${GAT_NAME}.cc
  )
  set( test_name ${GAT_NAME})
  
  add_executable( ${test_name} ${TEST_SOURCES})
  target_link_libraries( ${test_name} ${GAT_LIBRARIES})
  set_go_test_properties( ${test_name})
endfunction()

go_add_test( NAME memory_design_test )
go_add_test( NAME 0_design_test )

go_add_test( NAME unit_tests LIBRARIES ${GO_TOOLS_LIBRARIES} ${GO_GEOMETRY_LIBRARIES} ${GO_APPLICATION_LIBRARIES} ${GO_TEST_LIBRARIES} )

#go_add_test( NAME windows_cuda_thrust_bug )
