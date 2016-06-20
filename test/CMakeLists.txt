add_executable( 4_complex_planet_libnoise
  4_complex_planet_libnoise.cc )
  
  
find_package( LibNoise REQUIRED )
include_directories( ${LIBNOISE_INCLUDE_DIR} )  
target_link_libraries( 4_complex_planet_libnoise ${LIBNOISE_LIBRARIES} )
set_go_test_properties( 4_complex_planet_libnoise )

add_executable( 0_design_test
	0_design_test.cc )
target_link_libraries( 0_design_test ${GO_TOOLS_LIBRARIES} ${GO_GEOMETRY_LIBRARIES} ${GO_APPLICATION_LIBRARIES} )
set_go_test_properties( 0_design_test )