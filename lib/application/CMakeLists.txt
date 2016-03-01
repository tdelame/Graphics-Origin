file( 
  GLOB_RECURSE
  HEADER_FILES
  "${GO_HEADERS_IN_DIR}/graphics-origin/application/*.h")
	
file(
  GLOB_RECURSE
  SOURCE_FILES
  "${GO_SOURCES_IN_DIR}/application/*.cc")
  
add_library( 
  graphics_origin_application SHARED
  ${SOURCE_FILES} ${HEADER_FILES})
	
set( GO_APPLICATION_LIBRARIES 
  graphics_origin_application ${GO_GEOMETRY_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES}
  ${Qt5Quick_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5Qml_LIBRARIES} ${Qt5Network_LIBRARIES} ${Qt5Core_LIBRARIES}
  CACHE STRING "Libraries to link with if you want to use the application module")
  
install( TARGETS graphics_origin_application
  CONFIGURATIONS Debug
  DESTINATION lib/debug )
  
install( TARGETS graphics_origin_application
  CONFIGURATIONS Release
  DESTINATION lib/release )