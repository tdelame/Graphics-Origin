file( 
  GLOB_RECURSE
  HEADER_FILES
  "${GO_HEADERS_IN_DIR}/graphics-origin/application/*.h")
	
file(
  GLOB_RECURSE
  SOURCE_FILES
  "${GO_SOURCES_IN_DIR}/application/*.cc")
  
add_go_lib( application ${SOURCE_FILES} ${HEADER_FILES} )
set_go_lib_properties( application ${GO_GEOMETRY_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES}
  ${Qt5Quick_LIBRARIES} ${Qt5Gui_LIBRARIES} ${Qt5Qml_LIBRARIES} ${Qt5Network_LIBRARIES} ${Qt5Core_LIBRARIES} 
  ${FreeImage_LIBRARIES})  
    
install( TARGETS graphics_origin_application
  CONFIGURATIONS Debug
  DESTINATION lib/debug )
  
install( TARGETS graphics_origin_application
  CONFIGURATIONS Release
  DESTINATION lib/release )