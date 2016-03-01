file( 
  GLOB_RECURSE
  HEADER_FILES
  "${GO_HEADERS_IN_DIR}/graphics-origin/geometry/*.h")
	
file(
  GLOB_RECURSE
  SOURCE_FILES
  "${GO_SOURCES_IN_DIR}/geometry/*.cc")
  
add_library( 
  graphics_origin_geometry SHARED
  ${SOURCE_FILES} ${HEADER_FILES})
	
set( GO_GEOMETRY_LIBRARIES 
  graphics_origin_geometry ${GO_TOOLS_LIBRARIES} ${CUDA_LIBRARIES} ${OPENMESH_LIBRARIES}
  CACHE STRING "Libraries to link with if you want to use the geometry module")
  
install( TARGETS graphics_origin_geometry
  CONFIGURATIONS Debug
  DESTINATION lib/debug )
  
install( TARGETS graphics_origin_geometry
  CONFIGURATIONS Release
  DESTINATION lib/release )  