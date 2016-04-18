file( 
  GLOB_RECURSE
  HEADER_FILES
  "${GO_HEADERS_IN_DIR}/graphics-origin/geometry/*.h")
	
file(
  GLOB_RECURSE
  SOURCE_FILES
  "${GO_SOURCES_IN_DIR}/geometry/*.cc")
  
add_go_lib( geometry ${SOURCE_FILES} ${HEADER_FILES} )
set_go_lib_properties( geometry ${GO_TOOLS_LIBRARIES} ${CUDA_LIBRARIES} ${OPENMESH_LIBRARIES} )  
  
  
install( TARGETS graphics_origin_geometry
  CONFIGURATIONS Debug
  DESTINATION lib/debug )
  
install( TARGETS graphics_origin_geometry
  CONFIGURATIONS Release
  DESTINATION lib/release )  

install( TARGETS graphics_origin_geometry
  CONFIGURATIONS RelWithDebInfo
  DESTINATION lib/relwithdebinfo )  