file( 
  GLOB_RECURSE
  HEADER_FILES
  "${GO_HEADERS_IN_DIR}/graphics-origin/tools/*.h")
	
file(
  GLOB_RECURSE
  SOURCE_FILES
  "${GO_SOURCES_IN_DIR}/tools/*.cc")
  
add_go_lib( tools ${SOURCE_FILES} ${HEADER_FILES} )
set_go_lib_properties( tools ${Boost_LIBRARIES} ${OpenCL_LIBRARIES} )	

install( TARGETS graphics_origin_tools
  CONFIGURATIONS Debug
  DESTINATION lib/debug )
  
install( TARGETS graphics_origin_tools
  CONFIGURATIONS Release
  DESTINATION lib/release )  