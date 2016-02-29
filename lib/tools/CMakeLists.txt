file( 
  GLOB_RECURSE
  HEADER_FILES
  "${GO_HEADERS_IN_DIR}/graphics-origin/tools/*.h")
	
file(
  GLOB_RECURSE
  SOURCE_FILES
  "${GO_SOURCES_IN_DIR}/tools/*.cc")
  
add_library( 
  graphics_origin_tools SHARED
  ${SOURCE_FILES} ${HEADER_FILES})
	
set( GO_TOOLS_LIBRARIES 
  graphics_origin_tools ${Boost_LIBRARIES} 
  CACHE STRING "Libraries to link with if you want to use the tools module")